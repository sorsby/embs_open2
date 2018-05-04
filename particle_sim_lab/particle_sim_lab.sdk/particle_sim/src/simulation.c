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
		struct Particle *p = &particles[i];

		p->x = p->x + p->vx;
		p->y = p->y + p->vy;

		if (p->x <= 5) {
			p->x = 5;
			p->vx = 0;
		}

		if (p->y <= 5) {
			p->y = 5;
			p->vy = 0;
		}

		if (p->x >= 1435) {
			p->x = 1435;
			p->vx = 0;
		}

		if (p->y >= 895) {
			p->y = 895;
			p->vy = 0;
		}

		for (j = 0; j < num_attractors; j++) {
			struct Attractor a = attractors[j];

			float exp1 = (a.x - p->x);
			float exp2 = (a.y - p->y);
			float exp = exp1*exp1 + exp2*exp2;
			float d = sqrt(exp);

			float x_norm;
			float y_norm;
			if (d < 500) {
				x_norm = (a.x - p->x)/d;
				y_norm = (a.y - p->y)/d;

				if (d < 1.0) {
					x_norm = x_norm * a.g;
					y_norm = y_norm * a.g;
				} else {
					x_norm = x_norm * (1/d) * a.g;
					y_norm = y_norm * (1/d) * a.g;
				}

				particles[i].vx = particles[i].vx + x_norm;
				particles[i].vy = particles[i].vy + y_norm;

//				printf("vx %f\n", particles[i].vx);
			}
		}
	}
//	printf("successful update\n");
}
