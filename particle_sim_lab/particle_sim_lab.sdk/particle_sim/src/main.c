#include <stdio.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "zybo_vga/display_ctrl.h"
#include "vga.h"

int main(void) {
    init_platform();
	setupVGA();

	while (1) {
		updateFrame();
	}

    cleanup_platform();
	return 0;
}
