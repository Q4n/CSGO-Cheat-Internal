#pragma once

#include "hack.h"

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR col);

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);
void DrawLine(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color);

void DrawEspBox2D(Vec2 top, Vec2 bot, int thickness, D3DCOLOR color);

void DrawEspBox3D(Vec3 top, Vec3 bot, float a, int width, int thickness, D3DCOLOR color);

void DrawText(const char* text, float x, float y, D3DCOLOR color);