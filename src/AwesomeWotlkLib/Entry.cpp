#include "BugFixes.h"
#include "CommandLine.h"
#include "NamePlates.h"
#include "Misc.h"
#include "Hooks.h"
#include "Inventory.h"
#include "Item.h"
#include "Spell.h"
#include "UnitAPI.h"
#include <Windows.h>
#include <Detours/detours.h>
#include "VoiceChat.h"


static int lua_debugbreak(lua_State* L)
{
    if (IsDebuggerPresent())
        DebugBreak();
    return 0;
}

static int lua_openawesomewotlk(lua_State* L)
{
    lua_pushnumber(L, 1.f);
    lua_setglobal(L, "AwesomeWotlk");
#ifdef _DEBUG
    lua_pushcfunction(L, lua_debugbreak);
    lua_setglobal(L, "debugbreak");
#endif
    return 0;
}

static void OnAttach()
{
//#ifdef _DEBUG
//    system("pause");
//    FreeConsole();
//#endif

    *(DWORD*)0x00B6AF54 = 1; // TOSAccepted = 1
    *(DWORD*)0x00B6AF5C = 1; // EULAAccepted = 1

    // Initialize modules
    DetourTransactionBegin();
    Hooks::initialize();
    BugFixes::initialize();
    CommandLine::initialize();
    Inventory::initialize();
    Item::initialize();
    NamePlates::initialize();
    Misc::initialize();
    UnitAPI::initialize();
    Spell::initialize();
    VoiceChat::initialize();
    DetourTransactionCommit();

    // Register base
    Hooks::FrameXML::registerLuaLib(lua_openawesomewotlk);
}

int __stdcall DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        OnAttach();
    return 1;
}