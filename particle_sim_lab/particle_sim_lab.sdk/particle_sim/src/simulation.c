/*
 * simulation.c
 *
 *  Created on: 3 May 2018
 *      Author: Matthew
 */
#include "simulation.h"
#include "stdio.h"
#include "math.h"

int i, j;

void updateSimulation(struct Particle *particles, struct Attractor *attractors,
		int num_particles, int num_attractors) {

	for (i = 0; i < num_particles; i++) {
		struct Particle p = particles[i];
		p.x = p.x + p.vx;
		p.y = p.y + p.vy;

		// Bound the simulation
		if (p.x <= 5 || p.x >= 1435) {
			p.vx = 0;
		} else if (p.y <= 5 || p.y >= 895) {
			p.vy = 0;
		}
//		printf("p %d|%d\n", p.x, p.y);

		for (j = 0; j < num_attractors; j++) {
			struct Attractor a = attractors[j];
			int exp = ((a.x - p.x)^2) + ((a.y - p.y)^2);
			int d = sqrt(exp);

			int x_norm;
			int y_norm;
			if (d < 500) {
				x_norm = (a.x - p.x)/d;
				y_norm = (a.y - p.y)/d;

				if (d < 1) {
					x_norm = x_norm * a.g;
					y_norm = y_norm * a.g;
				} else {
					x_norm = x_norm * (1/d) * a.g;
					y_norm = y_norm * (1/d) * a.g;
				}

				p.vx = p.vx + x_norm;
				p.vy = p.vy + y_norm;
//				printf("p2 %d|%d\n", p.x, p.y);
			}
		}
	}
//	printf("successful update\n");
}
