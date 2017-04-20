// engine.h: include file for my not so awesome hacking-engine.

#pragma once

#include "stdafx.h"
#include "weapon.h"
#include "modelnode.h"
#include "log.h"
#include "menu.h"

class Engine
{
public:
	Engine(Log* log);
	~Engine();

	bool Initialize(HMODULE cshell);
	void HookFlipScreen(uint32_t routine, uint32_t* original);
	void Backup();
	void Run();
	void DetourWeaponCheck();

	static Weapon* GetOldWeaponByIndex(int32_t index)
	{
		return Engine::old_weapons_[index];
	}

private:
	bool UpdateDirectDevice();

private:
	static Weapon *old_weapons_[2000];
	
	Menu* menu_;
	LPDIRECT3DDEVICE9 direct_device_;
	uint32_t address_of_device_;

	Log* file_log_;
	uint32_t cshell_base_;
	uint32_t flipscreen_;
	uint32_t call_to_flipscreen_;
	uint32_t address_of_weapon_array_;
	uint32_t weaponcheck_;
	Weapon **weapons_;
	uint32_t address_of_modelnode_array_;
	uint32_t address_of_wall_array_;
	ModelNode *model_nodes_;
	ModelNode old_model_nodes_[300];
	time_t last_run_;
	bool initialized_ = false;

	int32_t no_recoil_status_;
	int32_t headshot_status_;
	int32_t max_damage_status_;
	int32_t no_nade_damage_status_;
	int32_t speed_knife_status_;
	int32_t ranged_knife_status_;
	int32_t knife_angle_status_;
	int32_t movespeed_penalty_status_;
	int32_t wallhack_status_;
	int32_t seeghost_status_;
};