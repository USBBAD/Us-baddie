//
// target.c
//
// Created: 2023-10-13
//  Author: Dmitry Murashov (dmtr <DOT> murashov <AT> GMAIL)
//

#include "target/target.h"

struct Target {
};

static struct Target sTarget;

TargetHandle targetInitialize()
{
	return (TargetHandle)&sTarget;
}
