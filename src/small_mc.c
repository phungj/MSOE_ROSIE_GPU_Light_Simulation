/*
 * small_mc.cu
 *
 * This file contains the code for our final project, designed to simulate light propagation through
 * a medium using Monte Carlo methods.
 *
 * The original code was written by Scott Prahl and can be found here:
 * https://omlc.org/software/mc/small_mc.c 
 *
 * Author: Scott Prahl, Jonathan Paulick, Jonathan Phung
 * Creation Date: 10/22/22
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Absorption Coefficient in 1/cm
#define MU_A 5.0

// Scattering Coefficient in 1/cm
#define MU_S 95.0

// Scattering Anisotropy -1 <= g <= 1
#define G 0.5

// Index of refraction of medium
#define N 1.5

// Thickness of one bin layer
#define MICRONS_PER_BIN 20.0

// Number of NUMBER_OF_PHOTONS to simulate
#define NUMBER_OF_PHOTONS 100000l

// Specular reflection
#define RS ((N - 1.0) * (N - 1.0) / (N + 1.0) / (N + 1.0))

// TODO: Figure out what this means
#define ALBEDO (MU_S / (MU_S + MU_A))

// Cosine of critical angle
#define CRITICAL_ANGLE (sqrt(1.0 - 1.0 / N / N))

// TODO: Figure out what this means
#define BINS_PER_MFP (1e4 / MICRONS_PER_BIN / (MU_A + MU_S))

// TODO: Figure out what this means
#define BINS 101

/**
 * @struct Photon
 * @brief This struct represents a photon to be simulated.
 * @var Photon::xPosition The x-position of the photon, formerly x.
 * @var Photon::yPosition The y-position of the photon, formerly y.
 * @var Photon::zPosition The z-position of the photon, formerly z.
 * @var Photon::deltaXPosition The change in the x-position of the photon, formerly u.
 * @var Photon::deltaYPosition The change in the y-position of the photon, formerly v.
 * @var Photon::deltaZPosition The change in the z-position of the photon, formerly w.
 * @var Photon::weight The weight of the photon.
 */
 
typedef struct {
    double xPosition;
    double yPosition;
    double zPosition;
    double deltaXPosition;
    double deltaYPosition;
    double deltaZPosition;
    double weight;
} Photon;

/**
 * @brief This function initializes the given Photon to the starting values.
 * @param photon A pointer to the Photon to initialize.
 */
 
void initialize_photon(Photon *photon);

/**
 * TODO: Figure out what this means
 * @brief This function simulates the bouncing of a photon off the top surface.
 * @param photon A pointer to the Photon to simulate.
 * @param rd A statistic used in simulation
 */
 
void bounce(Photon* photon, double* rd);

// TODO: Doxygen
/**
 * TODO: Figure out what this means
 * @brief This function simulates the bouncing of a photon off the top surface.
 * @param photon A pointer to the Photon to simulate.
 * @param rd A statistic used in simulation
 */
void move(Photon* photon, double* rd);

/**
 * TODO: Figure out what this means
 * @brief This function simulates the bouncing of a photon off the top surface.
 * @param photon A pointer to the Photon to simulate.
 * @param rd A statistic used in simulation
 */
void absorb(Photon* photon, double bit, double heat[]);

/**
 * @brief This function initializes the given Photon to the starting values.
 * @param photon A pointer to the Photon to initialize.
 */
void scatter(Photon* photon);

/**
 * TODO: Figure out what the params mean
 * @brief This function will print the results and statistics of the Monte Carlo simulations.
 * @param rd A statistic used to calculate the results
 * @param bit A statistic used to calculate the results
 * @param heat An array of statistics used to calculate the results
 */
 
void print_results(double* rd, double bit, double heat[]);

int main(void) {
    // TODO: figure out what these mean
    double rd;
    double bit;
    double heat[BINS];

    Photon photon;
	
    for(int i = 1; i <= NUMBER_OF_PHOTONS; i++) {
        // TODO: Check these default values, uninitialized in original code
      	rd = 0;
        bit = 0;

      	initialize_photon(&photon);

        while(photon.weight > 0) {
            move(&photon, &rd);
            absorb(&photon, bit, heat);
            scatter(&photon);
	}
    }	

    print_results(&rd, bit, heat);

    return 0;
}

/**
 * @brief This function initializes the given Photon to the starting values.
 * @param photon A pointer to the Photon to initialize.
 */

void initialize_photon(Photon *photon) {
    photon->xPosition = 0;
    photon->yPosition = 0;
    photon->zPosition = 0;
    photon->deltaXPosition = 0;
    photon->deltaYPosition = 0;
    photon->deltaZPosition = 1.0;
    photon->weight = 1 - RS;
}

/**
 * TODO: Figure out what this means
 * @brief This function simulates the bouncing of a photon off the top surface.
 * @param photon A pointer to the Photon to simulate.
 * @param rd A statistic used in simulation
 */

void bounce(Photon* photon, double* rd) {
    // TODO: Figure out what these mean
    double t; 
    double temp;
    double temp1;
    double rf;

    photon->deltaZPosition = - photon->deltaZPosition;
    photon->zPosition = - photon->zPosition;

    // TODO: Figure out what this means
    // This conditional checks the total internal reflection of the photon
    if(photon->deltaZPosition > CRITICAL_ANGLE) {
        // Cosine of exit angle
	t = sqrt(1.0 - N * N * (1.0 - photon->deltaZPosition * photon->deltaZPosition));

      	temp = (t - N * photon->deltaZPosition) / (t + N * photon->deltaZPosition);
        temp1 = (photon->deltaZPosition - N * t) / ((photon->deltaZPosition + N * t));

	// Fresnel reflection
      	rf = (temp1 * temp1 + temp * temp) / 2.0;
	(*rd) += (1.0 - rf) * photon->weight;
	photon->weight -= (1.0 - rf) * photon->weight;
    }		
}

// TODO: Doxygen
/* move to next scattering or absorption event */
/**
 * TODO: Figure out what this means
 * @brief This function simulates the bouncing of a photon off the top surface.
 * @param photon A pointer to the Photon to simulate.
 * @param rd A statistic used in simulation
 */

void move(Photon* photon, double* rd) {
    double d = -log((rand()+1.0)/(RAND_MAX+1.0));

    photon->xPosition += d * photon->deltaXPosition;
    photon->yPosition += d * photon->deltaYPosition;
    photon->zPosition += d * photon->deltaZPosition;  
    if ( photon->zPosition<=0 ) bounce(photon, rd);
}

// TODO: Doxygen
// Absorb light in the medium
void absorb (Photon* photon, double bit, double heat[]) {
    int bin=photon->zPosition * BINS_PER_MFP;

    if (bin >= BINS) bin = BINS-1;	
    heat[bin] += (1.0-ALBEDO) * photon->weight;
    photon->weight *= ALBEDO;
    if (photon->weight < 0.001){ /* Roulette */
        bit -= photon->weight;
        if (rand() > 0.1 * RAND_MAX) photon->weight = 0; else photon->weight /= 0.1;
        bit += photon->weight;
    }
}

// TODO: Doxygen
// Scatter photon and establish new direction
void scatter(Photon* photon) {
    double x1, x2, x3, t, mu;

    for(;;) {								/*new direction*/
        x1 = 2.0 * rand() / RAND_MAX - 1.0; 
      	x2 = 2.0 * rand()/ RAND_MAX - 1.0; 
	if ((x3 = x1 * x1 + x2 * x2) <= 1) break;
    }	
    if (G==0) {  /* isotropic */
        photon->deltaXPosition = 2.0 * x3 -1.0;
	photon->deltaYPosition = x1 * sqrt((1-photon->deltaXPosition * photon->deltaXPosition) /x3);
	photon->deltaZPosition = x2 * sqrt((1-photon->deltaXPosition * photon->deltaXPosition) /x3);
	return;
    } 

    mu = (1 - G * G) / (1 - G + 2.0 * G * rand() / RAND_MAX);
    mu = (1 + G * G - mu * mu) / 2.0 / G;
    if ( fabs(photon->deltaZPosition) < 0.9 ) {	
        t = mu * photon->deltaXPosition + sqrt((1 - mu * mu) / (1 - photon->deltaZPosition *
              photon->deltaZPosition) / x3) * (x1 * photon->deltaXPosition * photon->deltaZPosition
              - x2 * photon->deltaYPosition);

      	photon->deltaYPosition = mu * photon->deltaYPosition + sqrt((1 - mu * mu) / (1 - 
              photon->deltaZPosition * photon->deltaZPosition) / x3) * (x1 * 
              photon->deltaYPosition * photon->deltaZPosition + x2 *
              photon->deltaXPosition);
	      
        photon->deltaZPosition = mu * photon->deltaZPosition - sqrt((1 - mu * mu) * (1 - 
              photon->deltaZPosition * photon->deltaZPosition) / x3) * x1;

    } else {
        t = mu * photon->deltaXPosition + sqrt((1 - mu * mu) / (1 - photon->deltaYPosition * 
              photon->deltaYPosition) / x3) * (x1 * photon->deltaXPosition * photon->deltaYPosition
              + x2 * photon->deltaZPosition);

      	photon->deltaZPosition = mu * photon->deltaZPosition + sqrt((1 - mu * mu) / (1 - 
              photon->deltaYPosition) / x3) * (x1 * photon->deltaYPosition * 
              photon->deltaZPosition - x2 * photon->deltaXPosition);

        photon->deltaYPosition = mu * photon->deltaYPosition - sqrt((1 - mu * mu) * (1 - 
              photon->deltaYPosition * photon->deltaYPosition) / x3) * x1;

    }

    photon->deltaXPosition = t;
}

/**
 * TODO: Figure out what the params mean
 * @brief This function will print the results and statistics of the Monte Carlo simulations.
 * @param rd A statistic used to calculate the results
 * @param bit A statistic used to calculate the results
 * @param heat An array of statistics used to calculate the results
 */

void print_results(double* rd, double bit, double heat[]) {
    printf("Small Monte Carlo by Scott Prahl (https://omlc.org)\n");
    printf("1 W/cm^2 Uniform Illumination of Semi-Infinite Medium\n\n");

    printf("Scattering = %8.3f/cm\n", MU_S);
    printf("Absorption = %8.3f/cm\n", MU_A);
    printf("Anisotropy = %8.3f\n", G);
    printf("Refraction Index = %8.3f\n", N);
    printf("Number of Photons = %8ld\n\n", NUMBER_OF_PHOTONS);

    printf("Specular Reflection = %10.5f\n", RS);
    printf("Backscattered Reflection = %10.5f\n\n", (*rd) / (bit + NUMBER_OF_PHOTONS));

    printf("Depth         Heat\n[microns]     [W/cm^3]\n");

    for (int i = 0; i < BINS - 1; i++) {
	printf("%6.0f    %12.5f\n", i * MICRONS_PER_BIN, heat[i] / MICRONS_PER_BIN * 1e4 / (bit + 
              NUMBER_OF_PHOTONS));
    }

    printf("Extra Heat [W/cm^3]  %12.5f\n", heat[BINS - 1] / (bit + NUMBER_OF_PHOTONS));
}
