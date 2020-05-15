#include "Localplayer.h"
#include "Offsets.h"
#include "Vector3.h"

#include <cstdint>
#include <Windows.h>
#include <math.h>

LocalPlayer* LocalPlayer::Get()
{
	static uint32_t clientModule = (uint32_t)(GetModuleHandle("client_panorama.dll"));
	static LocalPlayer* localPlayer = (LocalPlayer*)(clientModule + hazedumper::signatures::dwLocalPlayer);

	return localPlayer;
}

Vector3* LocalPlayer::GetOrigin()
{
	return (Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
}

Vector3* LocalPlayer::GetViewOffset()
{

	return (Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecViewOffset);
}

int* LocalPlayer::GetHealth()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iHealth);
}

int* LocalPlayer::GetTeam()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iTeamNum);
}

float LocalPlayer::GetDistance(Vector3* other)
{
	Vector3* myPos = GetOrigin();
	_asm
	{
		__emit 0xEB
		__emit 0xFF
		__emit 0xC0
		__emit 0x48
	}
	Vector3 delta = Vector3(other->x - myPos->x, other->y - myPos->y, other->z - myPos->z);

	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

double PI = 3.14159265358;

void LocalPlayer::AimAt(Vector3* target)
{
	if (!target)return;
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	Vector3 origin = *GetOrigin();
	_asm
	{
		__emit 0xEB
		__emit 0xFF
		__emit 0xC0
		__emit 0x48
	}
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);

	Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
	float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

	if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
	{
		viewAngles->x = pitch;
		viewAngles->y = yaw;
	}
}


// ÉèÖÃÊÓ½Ç
void SetView(float pitch, float yaw) {
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	if (pitch >= -89 && pitch <= 89)
	{
		viewAngles->x = pitch;
	}
	_asm
	{
		call _P1
		_P1 :
		add[esp], 5
			retn
	}

	if (yaw >= -180 && yaw <= 180) {
		viewAngles->y = yaw;
	}
	Sleep(10);
}