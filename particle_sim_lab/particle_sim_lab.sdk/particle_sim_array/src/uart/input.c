/*
 * input.c
 *
 *  Created on: 4 May 2018
 *      Author: ms1516
 */

#include "xuartps_hw.h"
#include "stdio.h"

int readInput(char *prompt) {
	int num = 0;
	puts(prompt);
	char byte = '\0';
	while (byte != '\r') {
		byte = XUartPs_RecvByte(STDIN_BASEADDRESS);
		if (byte >= '0' && byte <= '9') {
			num = num * 10 + (byte - '0');
		}
		XUartPs_SendByte(STDIN_BASEADDRESS, byte);
	}
	printf("%d\n", num);
	return num;
}
