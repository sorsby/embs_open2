/*
 * ethernet.c
 *
 *  Created on: 4 May 2018
 *      Author: Matthew
 */

#include <stdio.h>
#include "xparameters.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "ethernet_platform.h"
#include "../util/util.h"
#include "xtime_l.h"
#include "../simulation/simulation.h"
#include "../main.h"
#include "../def.h"

#define TIMER_START 500000000

#define DATA_OBJECT_LENGTH 22

unsigned char ms1516_mac_address[] = { 0x00, 0x11, 0x22, 0x33, 0x00, 0x26 };
ip_addr_t serverIP;
int serverPort;
int hasAllData = FALSE;

int scenarioId;
int numParts = 0;
int nextPart = 0;

char* currentRequest;
struct udp_pcb *recv_pcb;

XTime startTime, endTime, executionTime;

int numParticles = 0;
int numAttractors = 0;

float ram[RAM_SIZE];

int networkInUse = FALSE;

void sendMessage(char *message);
void udp_part_get_handler(void *arg, struct udp_pcb *pcb, struct pbuf *p,
		struct ip_addr *addr, u16_t port);

void startTimer() {
	XTime_GetTime(&startTime);
}

float getTimePassed() {
	XTime_GetTime(&endTime);
	executionTime = (endTime - startTime);
	return 1.0 * executionTime / COUNTS_PER_SECOND;
}

void resetTimer() {
	startTime = 0;
	endTime = 0;
	executionTime = 0;
}

void resetEthernet() {
	udp_remove(recv_pcb);
	networkInUse = FALSE;
	hasAllData = FALSE;
}

void requestScenario(int id) {
	recv_pcb = udp_new();
	if (!recv_pcb) {
		printf("Error creating PCB\n");
	}
	//Listen on port serverPort
	udp_bind(recv_pcb, IP_ADDR_ANY, serverPort);
	udp_recv(recv_pcb, udp_part_get_handler, NULL);

	networkInUse = TRUE;
	scenarioId = id;
	nextPart = 0;
	numParts = 0;

	printf("Get ScenarioID: %d, from the server.\n", id);
	char buffer[5];
	char message[13];
	snprintf(buffer, 5, "%04d", id);
	sprintf(message, "NUMPARTS,%s", buffer);

	sendMessage(message);
}

void requestPart(int partId) {
	printf("Getting part: %d\n", partId);
	char buffera[5];
	char bufferb[5];
	snprintf(buffera, 5, "%04d", scenarioId);
	snprintf(bufferb, 5, "%04d", partId);

	char message[18];
	sprintf(message, "GETPART,%s,%s", buffera, bufferb);

	sendMessage(message);
}

//void udp_numparts_get_handler(void *arg, struct udp_pcb *pcb, struct pbuf *p,
//		struct ip_addr *addr, u16_t port) {
//	if (p) {
//		printf("Message: %s\n", (char *) p->payload);
//		resetTimer();
//
//		numParts = strToInt(getNumParts((char *) p->payload));
//
//		udp_remove(pcb);
//		pbuf_free(p);
//
//		requestPart(0);
//	}
//}

int getNext( cur) {
	return cur + 1;
}

void udp_part_get_handler(void *arg, struct udp_pcb *pcb, struct pbuf *p,
		struct ip_addr *addr, u16_t port) {
	if (p) {
		printf("Message: %s\n", (char *) p->payload);
		resetTimer();

		char response[2000];
		strcpy(response, (char *) p->payload);

		char* type = getRequestType(response);
		puts(type);
		if (strcmp(type, "NUMP") == 0) {
			numParts = strToInt(getNumParts(response));
			requestPart(0);
			return;
		} else {

			char* pResponseParser;
			pResponseParser = response;
			pResponseParser += 15; //move pointer to first data

			char lenStr[5];
			strncpy(lenStr, pResponseParser, 5);
			lenStr[4] = '\0';
			int dataLength = atoi(lenStr);

			pResponseParser += 5; //move to first object

			//loop length of data
			int objectCount = 0;
			while (objectCount < (dataLength / DATA_OBJECT_LENGTH)) {
				int ramOffset;

				char particle[12];
				char attractor[17];

				char *px;
				char *py;
				char *pg;

				char x[5];
				char y[5];
				char g[5];
				switch (*pResponseParser) {

				case 'P':
					strncpy(particle, pResponseParser, 12);
					particle[11] = '\0';

					px = &particle[2];
					strncpy(x, px, 5);
					x[4] = '\0';

					py = &particle[7];
					strncpy(y, py, 5);
					y[4] = '\0';

					ramOffset = numParticles * PARTICLE_SIZE;
					ram[ramOffset] = atoi(x);
					ram[ramOffset + 1] = atoi(y);
					ram[ramOffset + 2] = 0;
					ram[ramOffset + 3] = 0;

					numParticles += 1;

					break;
				case 'A':
					strncpy(attractor, pResponseParser, 17);
					attractor[16] = '\0';

					px = &attractor[2];
					strncpy(x, px, 5);
					x[4] = '\0';

					py = &attractor[7];
					strncpy(y, py, 5);
					y[4] = '\0';

					pg = &attractor[12];
					strncpy(g, pg, 5);
					g[4] = '\0';

					ramOffset = PARTICLE_END + numAttractors * ATTRACTOR_SIZE;
					ram[ramOffset + 1] = atoi(x);
					ram[ramOffset + 2] = atoi(y);
					ram[ramOffset + 3] = atof(g);

					numAttractors += 1;

					break;
				}
				objectCount += 1;
				pResponseParser += 22;

			}

			pbuf_free(p);

			if (nextPart == numParts - 1) {
				printf("num particles: %d\n", numParticles);
				printf("num attractor: %d\n", numAttractors);
				populateSimulationFromNetworkArray(ram, numParticles,
						numAttractors);
				hasAllData = TRUE;
				resetEthernet();
				//all parts received
				//populate simulation
			} else if (nextPart == 0) {
				nextPart += 1;
			} else {
				nextPart += 1;
				requestPart(nextPart);
			}
		}
	}
}

void sendMessage(char *message) {
	//Create a protocol control block (PCB)
	struct udp_pcb *send_pcb = udp_new();
	//Create a packet buffer
	struct pbuf * reply = pbuf_alloc(PBUF_TRANSPORT, strlen(message), PBUF_REF);
	reply->payload = message;
	reply->len = strlen(message);

	//Send it and clean up
	udp_sendto(send_pcb, reply, &serverIP, serverPort);
//	printf("error: %d\n", er);

	strcpy(currentRequest, message);
	startTimer();

	pbuf_free(reply);
	udp_remove(send_pcb);
}

void handleEthernet() {
	if (networkInUse == TRUE) {
		handle_ethernet();
		if (hasAllData == FALSE && getTimePassed() > 10.0) {
			resetTimer();
			puts("Request timed out after 10 seconds, retrying...");
			puts(currentRequest);
			sendMessage(currentRequest);
		}
	}
}

void setupEthernet() {
	init_ethernet_platform(ms1516_mac_address, NULL, NULL);
	IP4_ADDR(&serverIP, 192, 168, 10, 1);
	serverPort = 51200;
}