//
// target.c
//
// Created: 2023-10-13
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

#include "target/target.h"

struct Target
{
};

static struct Target sTarget;

static void initializeBss();
static void initializeData();

/// @brief Sets the entire .bss section w/ NULL
static void initializeBss()
{
	extern long int *gBssStart;
	extern long int *gBssEnd;

	for (register long int *bssIt = gBssStart; bssIt < gBssEnd; ++bssIt)
	{
		*bssIt = 0;
	}
}

/// @brief Copies default values for .data section from flash memory
static void initializeData()
{
	extern int *gRamDataStart;
	extern int *gRamDataEnd;
	extern int *gFlashDataStart;

	for (register int *flashDataIt = gFlashDataStart, *ramDataIt = gRamDataStart;
		ramDataIt < gRamDataEnd;
		++flashDataIt, ++ramDataIt
	) {
		*ramDataIt = *flashDataIt;
	}
}

TargetHandle targetInitialize()
{
	initializeBss();
	initializeData();

	return (TargetHandle)&sTarget;
}
