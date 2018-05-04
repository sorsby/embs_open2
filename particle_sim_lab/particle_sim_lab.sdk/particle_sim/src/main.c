#include <stdio.h>
#include <stdlib.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "zybo_vga/display_ctrl.h"
#include "vga.h"
#include "simulation.h"
#include "fps.h"

int i;
int num_particles = 1000;
int num_attractors = 50;

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
		int random_x = rand() % 1440;
		int random_y = rand() % 900;
//		printf("1 %d, %d\n", random_x, random_y);
		struct Particle particle = {random_x, random_y, 0, 0};
		particles[i] = particle;
	}

	for (i = 0; i < num_attractors; i++) {
		int random_x = rand() % 1440;
		int random_y = rand() % 900;
//		printf("2 %d, %d\n", random_x, random_y);
		struct Attractor attractor = {random_x, random_y, 1};
		attractors[i] = attractor;
	}
}

int main(void) {
    init_platform();
	setupVGA();
	populateSimulation();

	while (1) {
		updateSimulation(particles, attractors, num_particles, num_attractors);
		updateFrame(particles, attractors, num_particles, num_attractors);
		frame_timer();
		printf("%f\n", get_fps());
	}

    cleanup_platform();
    freeMemory();
	return 0;
}
