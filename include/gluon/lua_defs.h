#pragma once
#include <Windows.h>

struct lua_State;
typedef int (*luaL_loadstring_proc)(lua_State* L, const char* s);
typedef int (*lua_pcall_proc)(lua_State* L, int nargs, int nresults, int errfunc);

static const auto lua_shared_dll = GetModuleHandleA("lua_shared.dll");
static const auto luaL_loadstring = reinterpret_cast<luaL_loadstring_proc>(GetProcAddress(lua_shared_dll, "luaL_loadstring"));
static const auto lua_pcall = reinterpret_cast<lua_pcall_proc>(GetProcAddress(lua_shared_dll, "lua_pcall"));

class LuaInterface {
public:
    DWORD pad00;
    lua_State* state;
};
enum class LuaInterfaceType : uint8_t
{
    CLIENT = 0,
    SERVER = 1,
    MENU = 2
};
typedef void* (*CreateInterface_proc)(const char* pName, int* pReturnCode);

static const auto CreateInterface = reinterpret_cast<CreateInterface_proc>(GetProcAddress(lua_shared_dll, "CreateInterface"));