/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file Random.cpp
	@brief wrapper of random functions
*/
#include "base.h"
#include "cRandom.h"
#include <stdlib.h>

// NOTE: replace to CorsacBase random

void cRandom::SetRandomSeed(int rng)
{
	srand(rng);
}

int cRandom::Random() { return rand(); }
