// modelnode.h : contains the structure for modelnodes used in the game.

#pragma once

#include "stdafx.h"

struct ModelNode
{
	int8_t ___spacer01[4];
	char name[32];
	float damage_factor;
	float relative_pos[3];
	float priority;
	float dimension[3];
	uint32_t color[4];
	int32_t type;
	int8_t ___spacer02[68];
};