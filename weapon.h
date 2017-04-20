// weapon.h : contains the structure for weapons used in the game.

#pragma once

#include "stdafx.h"

enum WeaponType
{
	kPistol = 0,
	kShotgun = 1,
	kSMG = 2,
	kRifle = 3,
	kSniper = 4,
	kMG = 5,
	kGrenade = 6,
	kKnife = 7,
	kC4 = 9
};

struct Weapon
{
public:
	int16_t index;						// 0x0
	int16_t type;						// 0x2
	int8_t ___spacer01[2072];
	float perturb_min[10];				// 0x81C, 0x28
	int8_t ___spacer02[320];
	float perturb_max[10];				// 0x984, 0x28
	int8_t ___spacer03[320];
	float range;						// 0xAEC
	int8_t ___spacer14[12];
	float ammo_damage;					// 0xAFC
	int8_t ___spacer04[572];
	float knife_normal_range;			// 0xD3C
	int8_t ___spacer16[8];
	float knife_normal_angle;			// 0xD48
	int8_t ___spacer17[4];
	float knife_normal_ani_rate[7];		// 0xD50, 0x1C
	int8_t ___spacer12[16];
	float knife_normal_range2;			// 0xD7C
	int8_t ___spacer15[12];
	float knife_normal_angle2;			// 0xD8C
	int8_t ___spacer18[8];
	float knife_normal_ani_rate2[7];	// 0xD98, 0x1C
	int8_t ___spacer13[64];
	float movespeed_penalty;			// 0xDF4
	float damageratio_per_node[30];		// 0xDF8, 0x78
	int8_t ___spacer06[916];
	float detail_perturb_shot[10];		// 0x1204, 0x28
	int8_t ___spacer07[320];
	float detail_react_pitch_shot[10];	// 0x136C, 0x28
	int8_t ___spacer08[320];
	float detail_react_yaw_shot[10];	// 0x14D4, 0x28
	int8_t ___spacer09[9040];
	float shot_react_yaw[10];			// 0x384C, 0x28
	int8_t ___spacer10[428];
	float shot_react_pitch[10];			// 0x3A20, 0x28
	int8_t ___spacer11[1464];
};