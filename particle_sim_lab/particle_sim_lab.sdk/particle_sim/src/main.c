#include <stdio.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "zybo_vga/display_ctrl.h"
#include "vga.h"
#include "simulation.h"
#include "fps.h"

int i;
int num_particles = 1000;
int num_attractors = 2;

struct Particle *particles;
struct Attractor *attractors;

void freeMemory() {
	free(particles);
	free(attractors);
}

void populateSimulation() {
	particles = malloc(num_particles * sizeof(struct Particle));
	attractors = malloc(num_attractors * sizeof(struct Attractor));
	for (i = 0; i < num_particles; i++) {
		struct Particle particle = {0, 0, 0, 0};
		particles[i] = particle;
	}
	for (i = 0; i < num_attractors; i++) {
		struct Attractor attractor = {700+(i*100), 400+(i*100), -1};
		attractors[i] = attractor;
	}
}

int main(void) {
    init_platform();
	setupVGA();
	populateSimulation();

	while (1) {
		updateSimulation(particles, attractors, num_particles, num_attractors);
		updateFrame();
		frame_timer();
	}

    cleanup_platform();
    freeMemory();
	return 0;
}
