#include "includes.h"
#include "Localplayer.h"

Hack::~Hack() {
	this->FontF->Release();
	this->LineL->Release();
}

void Hack::Init() {
	client = (uintptr_t)GetModuleHandle("client_panorama.dll");
	engine = (uintptr_t)GetModuleHandle("engine.dll");
	entList = (EntList*)(client + dwEntityList);
	localEnt = entList->ents[0].ent;
	//localEnt = (Ent*)LocalPlayer::Get();
}

void Hack::Update() {
	entList = (EntList*)(client + dwEntityList);
	localEnt = entList->ents[0].ent;
	//localEnt = (Ent*)LocalPlayer::Get();
	memcpy(&viewMatrix, (PBYTE*)(client + dwViewMatrix), sizeof(viewMatrix));
	this->CheckButtons();
}

void Hack::CheckButtons() {
	if (GetAsyncKeyState(button.showMenuBtn) & 1)
		settings.showMenu = !settings.showMenu;
	if (GetAsyncKeyState(button.showTeamatesBtn) & 1)
		settings.showTeamates = !settings.showTeamates;
	if (GetAsyncKeyState(button.snaplinesBtn) & 1)
		settings.snaplines = !settings.snaplines;
	if (GetAsyncKeyState(button.box2DBtn) & 1)
		settings.box2D = !settings.box2D;
	_asm
	{
		__emit 0xEB
		__emit 0xFF
		__emit 0xC0
		__emit 0x48
	}
	if (GetAsyncKeyState(button.status2DBtn) & 1)
		settings.status2D = !settings.status2D;
	if (GetAsyncKeyState(button.statusTextBtn) & 1)
		settings.statusText = !settings.statusText;
	if (GetAsyncKeyState(button.box3DBtn) & 1)
		settings.box3D = !settings.box3D;
	if (GetAsyncKeyState(button.velEspBtn) & 1)
		settings.velEsp = !settings.velEsp;
	if (GetAsyncKeyState(button.headlineEspBtn) & 1)
		settings.headlineEsp = !settings.headlineEsp;
	_asm
	{
		jz _P2
		jnz _P2
		_P1 :
		__emit 0xE8
	}
_P2:
	if (GetAsyncKeyState(button.rcsCrosshairBtn) & 1)
		settings.rcsCrosshair = !settings.rcsCrosshair;
	if (GetAsyncKeyState(button.aimbot) & 1)
		settings.aimbot = !settings.aimbot;
	if (GetAsyncKeyState(button.radar) & 1)
		settings.radar = !settings.radar;
	if (GetAsyncKeyState(button.triggerbot) & 1)
		settings.triggerbot = !settings.triggerbot;
	if (GetAsyncKeyState(button.rcs) & 1)
		settings.rcs = !settings.rcs;
	if (GetAsyncKeyState(button.rise) & 1) {
		settings.rise = (++settings.rise)%3;
		if(!settings.rise)
			SetView(0.f, 10000.f);
	}
}

bool Hack::CheckValidEnt(Ent* ent) {
	if (ent == nullptr)
		return false;
	if (ent == localEnt)
		return false;
	_asm
	{
		call _P1
		_P1 :
		add[esp], 5
			retn
	}
	if (ent->iHealth <= 0)
		return false;
	if (ent->isDormant)
		return false;
	return true;
}

bool Hack::WorldToScreen(Vec3 pos, Vec2& screen) {
	Vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	_asm
	{
		call _P1
		_P1 :
		add[esp], 5
			retn
	}
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

Vec3 Hack::GetBonePos(Ent* ent, int bone) {
	uintptr_t bonePtr = ent->boneMatrix;
	Vec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	_asm
	{
		__emit 0xEB
		__emit 0xFF
		__emit 0xC0
		__emit 0x48
	}
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);
	return bonePos;
}

Vec3 Hack::TransformVec(Vec3 src, Vec3 ang, float d) {
	Vec3 newPos;
	newPos.x = src.x + (cosf(TORAD(ang.y)) * d);
	_asm
	{
		__emit 0xEB
		__emit 0xFF
		__emit 0xC0
		__emit 0x48
	}
	newPos.y = src.y + (sinf(TORAD(ang.y)) * d);
	newPos.z = src.z + (tanf(TORAD(ang.x)) * d);
	return newPos;
}