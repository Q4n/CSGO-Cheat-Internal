#pragma once

#include "includes.h"
#include "Localplayer.h"
#include "Player.h"
#include "Offsets.h"

// 雷达
void Radar() {
	uintptr_t* localEntPtr = (uintptr_t*)LocalPlayer::Get();

	if (localEntPtr) {
		uintptr_t localEnt = *localEntPtr;
		int localTeam = *(int*)(localEnt + hazedumper::netvars::m_iTeamNum);
		_asm
		{
			call _P1
			_P1 :
			add[esp], 5
				retn
		}
		for (int i = 0; i < 64; i++)
		{
			uintptr_t ent = *(uintptr_t*)Player::GetPlayer(i);
			if (ent)
			{
				*(DWORD*)(ent + hazedumper::netvars::m_bSpotted) = true;
			}
		}
	}
}

// 自动扳机
void TriggerBot() {
	static uint32_t clientdll = (uint32_t)(GetModuleHandle("client_panorama.dll"));
	_asm
	{
		__emit 0xEB
		__emit 0xFF
		__emit 0xC0
		__emit 0x48
	}
	uintptr_t* localEntPtr = (uintptr_t*)LocalPlayer::Get();
	if (*(uint32_t*)localEntPtr) { return; }
	if (localEntPtr) {
		uintptr_t localEnt = *localEntPtr;
		int localTeam = *(int*)(localEnt + hazedumper::netvars::m_iTeamNum);
		_asm
		{
			call _P1
			_P1 :
			add[esp], 5
				retn
		}
		int crosshairId = *(int*)(localEnt + hazedumper::netvars::m_iCrosshairId);

		if (crosshairId <= 64 && crosshairId != 0)
		{
			uintptr_t crossHairEnt = *(uintptr_t*)(clientdll + hazedumper::signatures::dwEntityList + (crosshairId - 1) * 0x10);

			if (crossHairEnt)
			{
				int crosshairTeam = *(int*)(crossHairEnt + hazedumper::netvars::m_iTeamNum);
				int crosshairLifeState = *(int*)(crossHairEnt + hazedumper::netvars::m_lifeState);

				if ((localTeam != crosshairTeam) && (crosshairLifeState == 0))
				{
					*(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 5;
					Sleep(5); // 扳机参数, 貌似不能太短
					*(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 4;
				}
				else *(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 4;
			}
		}
	}
}

