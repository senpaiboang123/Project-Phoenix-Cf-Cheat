// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "memory.h"
#include "engine.h"
#include "log.h"

Log* file_log = new Log("phoenix-cf.log", true);
Engine* hacking_engine = new Engine(file_log);
uint32_t original_flipscreen;

__declspec(naked) void HookedFlipScreen()
{
	_asm
	{
		pushad
		pushfd
	}

	// run our hacking routine each time the game flips the screen buffers
	// might optimize this to not drop the framerate on "slow" machines
	hacking_engine->Run();
	junkasm
	
	_asm
	{
		popfd
		popad
		jmp original_flipscreen
	}
}

// prevent target from crashing, by not doing this inside the hooked function
uint32_t original_dispatch_message = reinterpret_cast<uint32_t>(&DispatchMessageA) + 5;
_declspec(naked) void HookedDispatchMessageA()
{
	static bool initialized = false;
	static bool failed = false;
	
	junkasm

	_asm
	{
		pushad
		pushfd
	}

	// check if game is ready and hack not yet initialized
	if (!initialized && !failed && GetModuleHandleA("CShell.dll") && GetModuleHandleA("ClientFx.fxd"))
	{
		// Initialize not so awesome hacking engine
		if (hacking_engine->Initialize(GetModuleHandleA("CShell.dll")))
		{
			// Backup the memory we're going to modify so we can restore it if needed
			// Also required to detour the game's checks for modified values
			hacking_engine->Backup();
			junkasm

			// Bypass the shitty 28_3 Client Error
			hacking_engine->DetourWeaponCheck();

			// Initialize fancy game hook
			hacking_engine->HookFlipScreen(reinterpret_cast<uint32_t>(&HookedFlipScreen), &original_flipscreen);
			initialized = true;
		}
		else
		{
			file_log->Write("Failed to initialize hacking engine.");
			failed = true;
			junkasm
		}
	}

	_asm
	{
		popfd
		popad
		mov edi, edi
		push ebp
		mov ebp, esp
		jmp original_dispatch_message
	}
}

bool HookDispatchMessageA(uint32_t routine)
{
	uint32_t address_of_function = reinterpret_cast<uint32_t>(&DispatchMessageA);
	uint8_t shell_code[] = "\xE9\x00\x00\x00\x00";
	junkasm
	uint32_t relative_address = routine - address_of_function - 5;
	file_log->Writef("Absolute HookedDispatchMessageA call address: 0x%X", relative_address);
	*reinterpret_cast<uint32_t*>(shell_code + 1) = relative_address;
	junkasm

	return PlaceBytes(reinterpret_cast<void*>(address_of_function), shell_code, 5);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		file_log->Write("Entry point called with DLL_PROCESS_ATTACH.");
		junkasm
		file_log->Write("Initializing DispatchMessageA hook.");
		if (!HookDispatchMessageA(reinterpret_cast<uint32_t>(&HookedDispatchMessageA)))
		{
			file_log->Write("Failed to setup DispatchMessageA hook.", kError);
			junkasm
			MessageBoxA(NULL, "Failed to initizalize hack", "Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		break; 

		// will probably not get called when using manual mapping
	case DLL_PROCESS_DETACH:
		junkasm
		file_log->Write("Entry point called with DLL_PROCESS_DETACH.");
		file_log->Close();
		break;
	}

	return TRUE;
}