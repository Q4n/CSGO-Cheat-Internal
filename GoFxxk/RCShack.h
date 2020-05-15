#pragma once
#include "includes.h"
#include "Offsets.h"
#include "Localplayer.h"
struct vec3 {
    float x, y, z;
};

// ÎÞºó×ù
void RCShack() {
    static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll"); // get our engine module
    static uint32_t clientModule = (uint32_t)GetModuleHandle("client_panorama.dll"); // get our client module
    static vec3 oldPunch = { 0,0,0 };
    _asm
    {
        __emit 0xEB
        __emit 0xFF
        __emit 0xC0
        __emit 0x48
    }
    uintptr_t* localPlayer = (uintptr_t*)LocalPlayer::Get();  // get our local player (from entity list) *dont fkn judge my ass*
    vec3* viewAngles = (vec3*)(*(uintptr_t*)(engineModule + hazedumper::signatures::dwClientState/*dwClientState*/) + hazedumper::signatures::dwClientState_ViewAngles/*dwClientState_viewAngles*/); // get our view angles
    _asm
    {
        __emit 0xEB
        __emit 0xFF
        __emit 0xC0
        __emit 0x48
    }
    int shotsFired = *(int*)(*localPlayer + hazedumper::netvars::m_iShotsFired/*shotsFiredAddy*/); // get shotsFired
    vec3 tempAngle = { 0,0,0 }, punchAngle = *(vec3*)(*localPlayer + hazedumper::netvars::m_aimPunchAngle/*aimPunchAngle*/); // create temp angle get our aimpunchangle *m_aimPunchAngle = 0x302C*
    if (shotsFired > 1 && GetAsyncKeyState(VK_LBUTTON)) { // do this so we can still 1 tap
        tempAngle.x = (viewAngles->x + oldPunch.x) - (punchAngle.x * 2); // https://www.youtube.com/watch?v=dQw4w9WgXcQ <- important video for understanding logic of rcs
        tempAngle.y = (viewAngles->y + oldPunch.y) - (punchAngle.y * 2); // 2 = amount of recoil control. 0 = none, 2 = full *must be in sync with below*
        while (tempAngle.y > 180) { tempAngle.y -= 360; }   // normalize angle
        while (tempAngle.y < -180) { tempAngle.y += 360; }  // ^
        if (tempAngle.x > 89.0f) { tempAngle.x = 89.0f; }   // ^
        _asm
        {
            __emit 0xEB
            __emit 0xFF
            __emit 0xC0
            __emit 0x48
        }
        if (tempAngle.x < -89.0f) { tempAngle.x = -89.0f; } // ^
        oldPunch.x = punchAngle.x * 2; // 2 = amount of recoil control. 0 = none, 2 = full *must be in sync with above*
        oldPunch.y = punchAngle.y * 2;
        *viewAngles = tempAngle;
    }
    else {
        oldPunch.x = 0; // reset old punch
        _asm
        {
            __emit 0xEB
            __emit 0xFF
            __emit 0xC0
            __emit 0x48
        }
        oldPunch.y = 0;
    }
}