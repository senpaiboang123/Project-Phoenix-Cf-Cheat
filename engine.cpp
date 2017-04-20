// engine.cpp: Contains code for the super not so awesome hacking-engine.

#include "engine.h"
#include "memory.h"

Weapon* Engine::old_weapons_[2000];

Engine::Engine(Log* log)
{
	this->file_log_ = log;
	this->menu_ = new Menu();

	// build menu
	this->file_log_->Write("Building menu.");
	char *option_on_off[] = { "Off", "On" };
	char *option_golden_normal[] = { "Off", "Golden", "Normal" };
	junkasm
	char *option_knife_speed[] = { "Off", "2x", "4x", "6x" };
	char *option_knife_range[] = { "Off", "250", "350", "500" };
	char *option_movespeed_penalty[] = { "Off", "115%", "125%", "145%" };
	this->menu_->AddItem(&this->no_recoil_status_, "No recoil", option_on_off, 2);
	this->menu_->AddItem(&this->headshot_status_, "Always headshot", option_golden_normal, 3);
	this->menu_->AddItem(&this->max_damage_status_, "Max. damage", option_on_off, 2);
	junkasm
	this->menu_->AddItem(&this->no_nade_damage_status_, "No grenade damage", option_on_off, 2);
	this->menu_->AddItem(&this->ranged_knife_status_, "Increased knife range", option_knife_range, 4);
	this->menu_->AddItem(&this->speed_knife_status_, "Increased knife speed", option_knife_speed, 4);
	this->menu_->AddItem(&this->knife_angle_status_, "Max. knife angle", option_on_off, 2);
	this->menu_->AddItem(&this->movespeed_penalty_status_, "Movement speed", option_movespeed_penalty, 4);
	this->menu_->AddItem(&this->wallhack_status_, "Wallhack", option_on_off, 2);
	this->menu_->AddItem(&this->seeghost_status_, "See ghost", option_on_off, 2);
}

Engine::~Engine()
{
}

bool Engine::Initialize(HMODULE cshell)
{
	this->file_log_->Write("Initializing hacking engine.");

	junkasm
	// assuming cshell is a valid handle
	this->cshell_base_ = reinterpret_cast<uint32_t>(cshell);
	this->file_log_->Writef("Client Shell base address: 0x%X", this->cshell_base_);

	// weapons
	this->address_of_weapon_array_ = *reinterpret_cast<uint32_t*>(*reinterpret_cast<uint32_t*>(FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x8B\x4F\x04\xA1\x00\x00\x00\x00\x85\xC0\x8B\x51\x04\x8B\x6A\x04"), "xxxx????xxxxxxxx") + 4));
	junkasm
	this->weaponcheck_ = FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\xE8\x00\x00\x00\x00\x8B\xF0\x83\xC4\x04\x85\xF6\x0F\x84\x00\x00\x00\x00\xD9\x86\x00\x00\x00\x00"), "x????xxxxxxxxx????xx????");
	if (!this->address_of_weapon_array_ || !this->weaponcheck_)
		return false;

	junkasm
	this->weapons_ = reinterpret_cast<Weapon**>(this->address_of_weapon_array_);
	this->file_log_->Writef("Address of weapon array: 0x%X", this->address_of_weapon_array_ - this->cshell_base_);
	junkasm
	this->file_log_->Writef("Address of GetWeaponByIndex function: 0x%X", this->weaponcheck_ - this->cshell_base_);

	// model nodes
	this->address_of_modelnode_array_ = *reinterpret_cast<uint32_t*>(*reinterpret_cast<uint32_t*>(FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x00\x00\x00\x00\xD9\x5C\x07\x00\x8B\x4E\x00\x8B\x51\x00\x8B\x42\x00"), "????xxx?xx?xx?xx?")));
	if (!this->address_of_modelnode_array_)
		return false;

	junkasm
	this->model_nodes_ = reinterpret_cast<ModelNode*>(this->address_of_modelnode_array_);
	this->file_log_->Writef("Address of ModelNode array: 0x%X", this->address_of_modelnode_array_ - this->cshell_base_);

	// FlipScreen
	this->flipscreen_ = FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x56\x8B\xF1\x80\x7E\x2C\x00\x74\x7D\x8B\x0D\x00\x00\x00\x00"), "xxxxxxxxxxx????");
	junkasm
	this->call_to_flipscreen_ = FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\xE8\x00\x00\x00\x00\xC6\x46\x00\x00\x5F\x5E\x33\xC0\x5B\x83\xC4\x00"), "x????xx??xxxxxxx?");
	if (!this->flipscreen_ || !this->call_to_flipscreen_)
		return false;

	this->file_log_->Writef("Address of FlipScreen function: 0x%X", this->flipscreen_ - this->cshell_base_);
	junkasm
	this->file_log_->Writef("Call to FlipScreen function: 0x%X", this->call_to_flipscreen_ - this->cshell_base_);

	// wallhack
	this->address_of_wall_array_ = *reinterpret_cast<uint32_t*>(FindPattern(0x400000, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x75\x00\x83\x0D\x00\x00\x00\x00\x01\xB8\x00\x00\x00\x00\xE8"), "x?xx????xx????x") + 0xA);

	if (!this->UpdateDirectDevice())
		return false;

	junkasm
	this->file_log_->Write("Hacking engine successfully initialized.");
	this->initialized_ = true;
	return true;
}

void Engine::HookFlipScreen(uint32_t routine, uint32_t* original)
{
	this->file_log_->Write("Setting up game hook: FlipScreen.");

	// let the caller know what the address of the original flipscreen function is
	*original = this->flipscreen_;

	junkasm
	uint32_t relative_address = routine - this->call_to_flipscreen_ - 5;
	*reinterpret_cast<uint32_t*>(this->call_to_flipscreen_ + 1) = relative_address;
}

void Engine::Backup()
{
	for (int32_t i = 0; i < 2000; i++)
	{
		if (this->weapons_[i] && this->weapons_[i]->index == i)
		{
			Engine::old_weapons_[i] = new Weapon;
			memcpy(reinterpret_cast<void*>(Engine::old_weapons_[i]), reinterpret_cast<void*>(this->weapons_[i]), sizeof(Weapon));
		}
	}

	junkasm

	for (int32_t i = 0; i < 300; i++)
	{
		memcpy(reinterpret_cast<void*>(&this->old_model_nodes_[i]), reinterpret_cast<void*>(&this->model_nodes_[i]), sizeof(ModelNode));
	}
}

void Engine::Run()
{
	// make sure our device is updated
	// TODO: add check if player is in game, we don't have to update it there
	this->UpdateDirectDevice();

	// navigate through menu and render it
	this->menu_->Navigate();
	junkasm
	this->menu_->Render(this->direct_device_);

	junkasm

	// trying to not drop the fps on slow machines, this might result in a more or less
	// noticeable delay when toggling features
	/*if (difftime(time(0), this->last_run_) < 0.2)
		return;*/

	// wallhack
	memcpy_s(reinterpret_cast<void*>(this->address_of_wall_array_ + 0xA4), 6, this->wallhack_status_ ? reinterpret_cast<void*>("\x00\x00\x00\x00\x01\x00") : reinterpret_cast<void*>("\x01\x01\x01\x01\x01\x01"), 6);

	junkasm

	*reinterpret_cast<uint32_t*>(this->address_of_wall_array_ + 0xB8) = this->seeghost_status_ ? 0xE : 5;

	if (this->address_of_modelnode_array_)
	{
		// turns every hit into a golden or normal headshot
		if (this->headshot_status_ > 0 && this->headshot_status_ <= 2)
		{
			for (int32_t i = 0; i < 300; i++)
			{
				junkasm
				this->model_nodes_[i].type = this->headshot_status_;
			}
		}
		else
		{
			for (int32_t i = 0; i < 300; i++)
			{
				this->model_nodes_[i].type = this->old_model_nodes_[i].type;
			}
		}
	}

	junkasm

	if (this->address_of_weapon_array_)
	{
		// loop through all weapons
		for (int32_t i = 0; i < 2000; i++)
		{
			// check if it's actually a valid weapon
			if (this->weapons_[i] && this->weapons_[i]->index == i)
			{
				// full (and perfect) no recoil, will give client error 28_3 if not bypassed
				for (int32_t y = 0; y < 10; y++)
				{
					junkasm
					this->weapons_[i]->perturb_max[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->perturb_max[y];
					this->weapons_[i]->perturb_min[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->perturb_min[y];
					this->weapons_[i]->detail_perturb_shot[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->detail_perturb_shot[y];
					junkasm
					this->weapons_[i]->detail_react_pitch_shot[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->detail_react_pitch_shot[y];
					this->weapons_[i]->detail_react_yaw_shot[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->detail_react_yaw_shot[y];
					this->weapons_[i]->shot_react_yaw[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->shot_react_yaw[y];
					this->weapons_[i]->shot_react_pitch[y] = this->no_recoil_status_ ? 0.0f : this->old_weapons_[i]->shot_react_pitch[y];
				}

				// max weapon damage
				// TODO: filter for damage instead of weapon type.
				if (this->weapons_[i]->type != kKnife && this->weapons_[i]->type != kSniper && this->weapons_[i]->type != kPistol)
				{
					for (int32_t y = 0; y < 30; y++)
					{
						junkasm
						this->weapons_[i]->damageratio_per_node[y] = this->max_damage_status_ && this->headshot_status_ ? 1.48f : this->old_weapons_[i]->damageratio_per_node[y];
					}
				}

				// movespeed hack
				if (this->weapons_[i]->type != kMG && this->weapons_[i]->type != kSniper)
				{
					static float movespeed_penalty_values[] = { 0.0f, -0.15f, -0.25f, -0.45f };
					this->weapons_[i]->movespeed_penalty = this->movespeed_penalty_status_ ? movespeed_penalty_values[this->movespeed_penalty_status_] : this->old_weapons_[i]->movespeed_penalty;
				}

				junkasm

				// no grenade damage
				if (this->weapons_[i]->type == kGrenade)
				{
					this->weapons_[i]->ammo_damage = this->no_nade_damage_status_ ? 0.0f : this->old_weapons_[i]->ammo_damage;
				}

				if (this->weapons_[i]->type == kKnife)
				{
					// knife range
					static float ranged_knife_values[] = { 0.0f, 250.0f, 350.0f, 500.0f };
					this->weapons_[i]->range = this->ranged_knife_status_ ? ranged_knife_values[this->ranged_knife_status_] : this->old_weapons_[i]->range;
					for (int32_t y = 0; y < 3; y++)
					{
						this->weapons_[i]->knife_normal_range = this->ranged_knife_status_ ? ranged_knife_values[this->ranged_knife_status_] : this->old_weapons_[i]->knife_normal_range;
						junkasm
						this->weapons_[i]->knife_normal_range2 = this->ranged_knife_status_ ? ranged_knife_values[this->ranged_knife_status_] : this->old_weapons_[i]->knife_normal_range2;
					}

					// 360 knife angle
					this->weapons_[i]->knife_normal_angle = this->knife_angle_status_ ? 9999.0f : this->old_weapons_[i]->knife_normal_angle;
					this->weapons_[i]->knife_normal_angle2 = this->knife_angle_status_ ? 9999.0f : this->old_weapons_[i]->knife_normal_angle2;

					// speed knife
					for (int32_t y = 0; y < 7; y++)
					{
						junkasm
						static float speed_knife_values[] = { 1.0f, 2.0f, 4.0f, 6.0f };
						this->weapons_[i]->knife_normal_ani_rate[y] = this->speed_knife_status_ ? speed_knife_values[this->speed_knife_status_] : this->old_weapons_[i]->knife_normal_ani_rate[y];
						this->weapons_[i]->knife_normal_ani_rate2[y] = this->speed_knife_status_ ? speed_knife_values[this->speed_knife_status_] : this->old_weapons_[i]->knife_normal_ani_rate2[y];
					}
				}
			}
		}
	}

	junkasm

	//this->last_run_ = time(0);
}

void Engine::DetourWeaponCheck()
{
	uint32_t relative_address = reinterpret_cast<uint32_t>(&Engine::GetOldWeaponByIndex) - this->weaponcheck_ - 5;
	junkasm
	*reinterpret_cast<uint32_t*>(this->weaponcheck_ + 1) = relative_address;
}

bool Engine::UpdateDirectDevice()
{
	if (!this->address_of_device_)
		this->address_of_device_ = *reinterpret_cast<uint32_t*>(*reinterpret_cast<uint32_t*>(FindPattern(0x400000, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x8B\x35\xFF\xFF\xFF\xFF\x8B\xEE\xE8\xFF\xFF\xFF\xFF\x8B\x45\x00\x8B\x08\x8B\x91\xFF\xFF\xFF\xFF\x57\x6A\xFF\x53"), "xx????xxx????xxxxxxx????xx?x") + 2));

	junkasm

	this->direct_device_ = *reinterpret_cast<LPDIRECT3DDEVICE9*>(this->address_of_device_);

	junkasm

	return (this->direct_device_ != nullptr);
}