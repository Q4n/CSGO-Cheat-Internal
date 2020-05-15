#include "Localplayer.h"
#include "Offsets.h"
#include "Vector3.h"
#include "includes.h"


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
	Vector3 delta = Vector3(other->x - myPos->x, other->y - myPos->y, other->z - myPos->z);

	return sqrt(pow(delta.x,2) + pow(delta.y, 2) + pow(delta.z, 2));
}

const double PI = 3.14159265358;

void LocalPlayer::AimAt(Vector3* target)
{
	if (!target)return;
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	Vector3 origin = *GetOrigin();
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);

	Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	float deltaVecLength = sqrt(pow(deltaVec.x, 2) + pow(deltaVec.y, 2) + pow(deltaVec.z, 2));

	float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
	float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

	if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
	{
		viewAngles->x = pitch;
		viewAngles->y = yaw;
	}
}

void LocalPlayer::SetView(float pitch, float yaw) {
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	if (pitch >= -89 && pitch <= 89)
	{
		viewAngles->x = pitch;
	}
	if (yaw >= -180 && yaw <= 180) {
		viewAngles->y = yaw;
	}
	Sleep(10);
}

int* LocalPlayer::GetCrosshairId()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iCrosshairId);
}

int* LocalPlayer::GetShortFire()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iShotsFired/*shotsFiredAddy*/);
}

int* LocalPlayer::GetMyWeapen()
{
	static uint32_t clientModule = (uint32_t)(GetModuleHandle("client_panorama.dll"));

	int weapon = *(int*)(*(uint32_t*)this+ hazedumper::netvars::m_hActiveWeapon);
	int weaponEntity = *(int*)(clientModule + hazedumper::signatures::dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);
	if (weaponEntity != NULL)
		return (int*)(weaponEntity + hazedumper::netvars::m_iItemDefinitionIndex);
	return nullptr;
}
