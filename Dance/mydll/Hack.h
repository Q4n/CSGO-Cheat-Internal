#pragma once
#include "includes.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "EnemyList.h"
#include "Offsets.h"
/*
每次循环:
	遍历所有敌人,
	瞄准,
	判断能否击中
	开枪
*/
class Vector3;
class MyHack
{
public:
	MyHack();
	~MyHack();
	void Init(); // 初始化
	void Update(); // 循环中更新参量, 开关
	void Exploit(); // 主代码

	bool CheckVaild(Player*);

	struct Var
	{
		bool isOpen = false;
	}var;

	struct Buttons {
		DWORD startBtn = VK_F2;
	}button;

	EnemyList enemies;
	uint32_t clientdll;
	LocalPlayer* plocal;
};

MyHack::MyHack()
{
	enemies.clear();
	clientdll = (uint32_t)(GetModuleHandle("client_panorama.dll"));
	plocal = LocalPlayer::Get();

}

MyHack::~MyHack()
{
}

inline void MyHack::Init()
{
	// 初始化获取所有敌人参量, 写入数组, 加快访问速度
	if (*(uintptr_t*)plocal) {
		uintptr_t localEnt = *(uintptr_t*)plocal;
		int localTeam = *plocal->GetTeam();

		// 暂时这么写, 等下需要动态更新player数量
		for (int i = 0; i < *Player::GetMaxPlayer(); i++)
		{
			Player* tmp = Player::GetPlayer(i);
			if (!*(uintptr_t*)tmp)continue;

			if (*tmp->GetTeam() != localTeam)
				enemies.add((DWORD)tmp);
		}
	}
}

inline void MyHack::Update()
{
	// 监听按钮事件
	if (GetAsyncKeyState(button.startBtn) & 1)
		var.isOpen = !var.isOpen;

	// 敌人数组为0时, 更新敌人
	if (enemies.idx == 0) {
		Sleep(1000); //下一回合?
		Init();
	}
}

inline void FixAngle(int myWeaponID,float currentDistance) {
	//Sleep(50); //clock18
	float delay;
	switch (myWeaponID & 0xff)
	{
	case 16: delay = 0.2; break; //m4a4
	case 11: delay = 0.15; break; // G3SG1, 不开镜还行, 开镜需要参数
	case 59:
	case 7: delay = 0.35; break; //ak
	case 40:delay = 0.2; break; //ssg, 开镜, 不开镜很难打
	case 9: delay = 0.2; break; //awp, 0.15很难修复镜面
	case 39: delay = 0.15; break; //sg, 不开镜, 开镜0.1有点慢
	case 13: delay = 0.4; break; //加利尔
	case 10: delay = 0.1; break; //famas
	case 2: delay = 0.3; break; //双枪
	case 8: delay = 0.15; break; //aug, 开镜可以0.1
	case 14: delay = 0.3; break; //m249
	case 28: delay = 0.3; break; //内格夫
	case 34: delay = 0.3; break; //mp9
	case 61: delay = 0.25; break; //mp9
	case 38: delay = 0.15; break; //scar 开镜可以0.1
	default: delay = 0.2;
	}
	printf("id:%d\tdelay:%f\tins:%f\n", myWeaponID, delay, currentDistance);
	Sleep(delay * currentDistance);
}

inline void MyHack::Exploit()
{
	if (!var.isOpen)return;

	DWORD i = 0;
	for (; i < enemies.idx; i++) {
		Player* enemy = (Player*)enemies.get(i);
		if (!enemy)break;
		if (*(uintptr_t*)enemy)
		{
			uintptr_t localPlayer = *(uintptr_t*)plocal;
			if (!localPlayer)return; //local player empty

			// 检查Player是否满足条件
			if (!CheckVaild(enemy)) {
				// 更新敌人信息
				enemies.del(i);
				continue;
			}
			// 我死了
			if (plocal->GetHealth() == 0) continue;

			int* pweapen = plocal->GetMyWeapen();
			if (!pweapen)continue;

AIM:
			plocal->AimAt(enemy->GetBonePos(8)); //瞄准
			float currentDistance = plocal->GetDistance(enemy->GetOrigin());


			Sleep(10);
			int crosshairId = *(int*)plocal->GetCrosshairId();
			if (crosshairId <= 64 && crosshairId > 0) //人物id
			{
				*(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 5; //射击
				Sleep(87); // 按多久松开, 由本机电脑决定, 这个参数需要微调
				*(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 4; //停止射击
				Sleep(10); // 等待生命值刷新

				// 回滚准心
				FixAngle(*pweapen,currentDistance);
				//static int ti = 0;
				//ti++;
				//printf("time%d: %d, %d\n", ti, *enemy->GetHealth(),*plocal->GetShortFire());
				if (*enemy->GetHealth() > 0) {
					//printf("%d-time not kill\n", ti);
					goto  AIM;
				}
			}
		}
	}

}

inline bool MyHack::CheckVaild(Player* currentPlayer)
{
	LocalPlayer* localPlayer = plocal;
	if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
	{
		return false;
	}

	if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
	{
		return false;
	}

	if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
	{
		return false;
	}

	return true;
}
