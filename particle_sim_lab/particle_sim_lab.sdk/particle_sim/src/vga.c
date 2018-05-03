/*
 * vga.c
 *
 *  Created on: 3 May 2018
 *      Author: ms1516
 */

#include "zybo_vga/display_ctrl.h"

// Frame size (based on 1440x900 resolution, 32 bits per pixel)
#define MAX_FRAME (1440*900)
#define FRAME_STRIDE (1440*4)

DisplayCtrl dispCtrl; // Display driver struct
u32 frameBuf[DISPLAY_NUM_FRAMES][MAX_FRAME]; // Frame buffers for video data
void *pFrames[DISPLAY_NUM_FRAMES]; // Array of pointers to the frame buffers

int x, y;
u32 stride;
u32 width;
u32 height;

u32 *frame;
int right;
int down;
int xpos;
int ypos;
u32 buff;

void setupVGA() {
	// Initialise an array of pointers to the 2 frame buffers
	int i;
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
		pFrames[i] = frameBuf[i];

	// Initialise the display controller
	DisplayInitialize(&dispCtrl, XPAR_AXIVDMA_0_DEVICE_ID, XPAR_VTC_0_DEVICE_ID, XPAR_VGA_AXI_DYNCLK_0_BASEADDR, pFrames, FRAME_STRIDE);

	// Start with the first frame buffer (of two)
	DisplayChangeFrame(&dispCtrl, 0);

	// Set the display resolution
	DisplaySetMode(&dispCtrl, &VMODE_1440x900);

	// Enable video output
	DisplayStart(&dispCtrl);

	stride = dispCtrl.stride / 4;
	width = dispCtrl.vMode.width;
	height = dispCtrl.vMode.height;

	right = 1;
	down = 1;
	xpos = width/2;
	ypos = height/2;
	buff = dispCtrl.curFrame;
}

void updateFrame() {
	// Switch the frame we're modifying to be back buffer (1 to 0, or 0 to 1)
	buff = !buff;
	frame = dispCtrl.framePtr[buff];

	// Clear the frame to white
	memset(frame, 0xFF, MAX_FRAME*4);

	drawSquare(xpos, ypos, 32, 0x000000FF);

	// Flush everything out to DDR
	Xil_DCacheFlush();

	// Switch active frame to the back buffer
	DisplayChangeFrame(&dispCtrl, buff);

	// Wait for the f0x00FF0000rame to switch (after active frame is drawn) before continuing
	DisplayWaitForSync(&dispCtrl);
}

void drawSquare(int startx, int starty, int size, int colour) {
	for (x = startx; x < startx+size; x++) {
		for (y = starty; y < starty+size; y++) {
			frame[y*stride + x] = colour;
		}
	}
}
