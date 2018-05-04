/*
 * simulation.h
 *
 *  Created on: 3 May 2018
 *      Author: Matthew
 */

#ifndef SRC_SIMULATION_H_
#define SRC_SIMULATION_H_

struct Particle {
	int x;
	int y;
	int vx;
	int vy;
};

struct Attractor {
	int x;
	int y;
	int g;
};

void updateSimulation(struct Particle *particles, struct Attractor *attractors,
		int num_particles, int num_attractors);

#endif /* SRC_SIMULATION_H_ */