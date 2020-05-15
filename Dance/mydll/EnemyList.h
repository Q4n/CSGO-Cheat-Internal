#pragma once
#include "includes.h"

class EnemyList
{
public:
	EnemyList();
	~EnemyList();

	DWORD enemies[128]{ 0 }; //´æ·Å Player*
	DWORD idx = 0;

	void add(DWORD e) {
		enemies[idx] = e;
		enemies[++idx] = 0;
	}
	void del(DWORD index) {
		enemies[index]=enemies[--idx];
		enemies[idx] = 0;
	}
	void clear() {
		idx = 0;
		enemies[idx] = 0;
	}
	DWORD get(DWORD index) {
		return enemies[index];
	}
};

EnemyList::EnemyList()
{
}

EnemyList::~EnemyList()
{
}


