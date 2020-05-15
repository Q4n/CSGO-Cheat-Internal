#include "includes.h"
#include "Hack.h"

#undef _DEBUG

DWORD WINAPI OnDllAttach(PVOID base) {
#ifdef _DEBUG
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    SetConsoleTitleA("DEBUG-console");
#endif
    std::cout << "Go" << std::endl;
    MyHack* hack = new MyHack();
    hack->Init();
    while (!(GetAsyncKeyState(VK_END) & 1)) {
        hack->Update();

        hack->Exploit();
    }
    delete hack;
    FreeLibraryAndExitThread(static_cast<HMODULE>(base), 0);
    return 0;
}
DWORD WINAPI OnDllDetach() {
#ifdef _DEBUG
    fclose(stdin);
    fclose(stdout);
    HWND hw = GetConsoleWindow();
    FreeConsole();
    PostMessageW(hw, WM_CLOSE, 0, 0);
#endif
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(0, 0, OnDllAttach, hModule, 0, 0));
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        OnDllDetach();
    }
    return TRUE;
}

