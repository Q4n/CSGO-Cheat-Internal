#pragma once
#include "Player.h"
#include "Localplayer.h"
#include <cstdint>

// 获取最近的敌人
Player* GetClosestEnemy()
{
	LocalPlayer* localPlayer = LocalPlayer::Get();
	if (!localPlayer)return NULL;
	float closestDitance = 1000000;
	int closesDistanceIndex = -1;

	for (int i = 1; i < *Player::GetMaxPlayer(); i++)
	{
		Player* currentPlayer = Player::GetPlayer(i);

		if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
		{
			continue;
		}

		if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
		{
			continue;
		}

		if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
		{
			continue;
		}

		float currentDistance = localPlayer->GetDistance(currentPlayer->GetOrigin());
		if (currentDistance < closestDitance)
		{
			_asm
			{
				__emit 0xEB
				__emit 0xFF
				__emit 0xC0
				__emit 0x48
			}
			closestDitance = currentDistance;
			closesDistanceIndex = i;
		}
	}

	if (closesDistanceIndex == -1)
	{
		return NULL;
	}
	return Player::GetPlayer(closesDistanceIndex);
}

// 设置视角为头部
void RunAimbot()
{
	Player* closestEnemy = GetClosestEnemy();

	if (closestEnemy)
	{
		_asm
		{
			__emit 0xEB
			__emit 0xFF
			__emit 0xC0
			__emit 0x48
		}
		LocalPlayer::Get()->AimAt(closestEnemy->GetBonePos(8));
	}
	Sleep(10);
}