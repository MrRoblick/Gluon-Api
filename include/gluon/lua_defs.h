#pragma once
#include <Windows.h>
#include <cstdint>
#include <types.h>

struct lua_State;
typedef int (*lua_CFunction) (lua_State* L);
typedef const char* (*lua_Reader) (lua_State* L, void* ud, size_t* sz);
typedef int (*lua_Writer) (lua_State* L, const void* p, size_t sz, void* ud);
typedef void* (*lua_Alloc) (void* ud, void* ptr, size_t osize, size_t nsize);
typedef double lua_Number;
typedef ptrdiff_t lua_Integer;

#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN	1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION	6
#define LUA_TUSERDATA	7
#define LUA_TTHREAD		8

#define LUA_MULTRET	(-1)

static const auto lua_shared_dll = GetModuleHandleA("lua_shared.dll");

#define LUA_DEF(ret, name, ...) \
    typedef ret(*name##_t)(__VA_ARGS__); \
    static const auto name = reinterpret_cast<name##_t>(GetProcAddress(lua_shared_dll, #name));


LUA_DEF(lua_State*, lua_newstate, lua_Alloc f, void* ud)
LUA_DEF(void, lua_close, lua_State* L)
LUA_DEF(lua_State*, lua_newthread, lua_State* L)
LUA_DEF(lua_CFunction, lua_atpanic, lua_State* L, lua_CFunction panicf)


LUA_DEF(int, lua_gettop, lua_State* L)
LUA_DEF(void, lua_settop, lua_State* L, int idx)
LUA_DEF(void, lua_pushvalue, lua_State* L, int idx)
LUA_DEF(void, lua_remove, lua_State* L, int idx)
LUA_DEF(void, lua_insert, lua_State* L, int idx)
LUA_DEF(void, lua_replace, lua_State* L, int idx)
LUA_DEF(int, lua_checkstack, lua_State* L, int sz)
LUA_DEF(void, lua_xmove, lua_State* from, lua_State* to, int n)


LUA_DEF(int, lua_isnumber, lua_State* L, int idx)
LUA_DEF(int, lua_isstring, lua_State* L, int idx)
LUA_DEF(int, lua_iscfunction, lua_State* L, int idx)
LUA_DEF(int, lua_isuserdata, lua_State* L, int idx)
LUA_DEF(int, lua_type, lua_State* L, int idx)
LUA_DEF(const char*, lua_typename, lua_State* L, int tp)

LUA_DEF(int, lua_equal, lua_State* L, int idx1, int idx2)
LUA_DEF(int, lua_rawequal, lua_State* L, int idx1, int idx2)
LUA_DEF(int, lua_lessthan, lua_State* L, int idx1, int idx2)

LUA_DEF(lua_Number, lua_tonumber, lua_State* L, int idx)
LUA_DEF(lua_Integer, lua_tointeger, lua_State* L, int idx)
LUA_DEF(int, lua_toboolean, lua_State* L, int idx)
LUA_DEF(const char*, lua_tolstring, lua_State* L, int idx, size_t* len)
LUA_DEF(size_t, lua_objlen, lua_State* L, int idx)
LUA_DEF(lua_CFunction, lua_tocfunction, lua_State* L, int idx)
LUA_DEF(void*, lua_touserdata, lua_State* L, int idx)
LUA_DEF(lua_State*, lua_tothread, lua_State* L, int idx)
LUA_DEF(const void*, lua_topointer, lua_State* L, int idx)

LUA_DEF(void, lua_pushnil, lua_State* L)
LUA_DEF(void, lua_pushnumber, lua_State* L, lua_Number n)
LUA_DEF(void, lua_pushinteger, lua_State* L, lua_Integer n)
LUA_DEF(void, lua_pushlstring, lua_State* L, const char* s, size_t l)
LUA_DEF(void, lua_pushstring, lua_State* L, const char* s)
LUA_DEF(void, lua_pushcclosure, lua_State* L, lua_CFunction fn, int n)
LUA_DEF(void, lua_pushboolean, lua_State* L, int b)
LUA_DEF(void, lua_pushlightuserdata, lua_State* L, void* p)
LUA_DEF(int, lua_pushthread, lua_State* L)


LUA_DEF(void, lua_gettable, lua_State* L, int idx)
LUA_DEF(void, lua_getfield, lua_State* L, int idx, const char* k)
LUA_DEF(void, lua_rawget, lua_State* L, int idx)
LUA_DEF(void, lua_rawgeti, lua_State* L, int idx, int n)
LUA_DEF(void, lua_createtable, lua_State* L, int narr, int nrec)
LUA_DEF(void*, lua_newuserdata, lua_State* L, size_t sz)
LUA_DEF(int, lua_getmetatable, lua_State* L, int objindex)
LUA_DEF(void, lua_getfenv, lua_State* L, int idx)


LUA_DEF(void, lua_settable, lua_State* L, int idx)
LUA_DEF(void, lua_setfield, lua_State* L, int idx, const char* k)
LUA_DEF(void, lua_rawset, lua_State* L, int idx)
LUA_DEF(void, lua_rawseti, lua_State* L, int idx, int n)
LUA_DEF(int, lua_setmetatable, lua_State* L, int objindex)
LUA_DEF(int, lua_setfenv, lua_State* L, int idx)


LUA_DEF(int, lua_call, lua_State* L, int nargs, int nresults)
LUA_DEF(int, lua_pcall, lua_State* L, int nargs, int nresults, int errfunc)
LUA_DEF(int, lua_cpcall, lua_State* L, lua_CFunction func, void* ud)
LUA_DEF(int, lua_load, lua_State* L, lua_Reader reader, void* dt, const char* chunkname)
LUA_DEF(int, lua_dump, lua_State* L, lua_Writer writer, void* data)


LUA_DEF(int, luaL_loadstring, lua_State* L, const char* s)
LUA_DEF(int, luaL_loadbuffer, lua_State* L, const char* buff, size_t sz, const char* name)
LUA_DEF(int, luaL_loadfile, lua_State* L, const char* filename)
LUA_DEF(int, luaL_ref, lua_State* L, int t)
LUA_DEF(void, luaL_unref, lua_State* L, int t, int ref)


inline void lua_pop(lua_State* L, int n) {
    lua_settop(L, -(n)-1);
}

inline bool lua_isnil(lua_State* L, int n) {
    return (lua_type(L, (n)) == LUA_TNIL);
}

inline bool lua_isboolean(lua_State* L, int n) {
    return (lua_type(L, (n)) == LUA_TBOOLEAN);
}

inline bool lua_istable(lua_State* L, int n) {
    return (lua_type(L, (n)) == LUA_TTABLE);
}

inline const char* lua_tostring(lua_State* L, int idx) {
    return lua_tolstring(L, idx, NULL);
}

inline void lua_pushcfunction(lua_State* L, lua_CFunction f) {
    lua_pushcclosure(L, f, 0);
}

enum class LuaInterfaceType : uint8_t
{
    CLIENT = 0,
    SERVER = 1,
    MENU = 2
};
class LuaInterface {
public:
    DWORD pad00;
    lua_State* state;
};

static const auto LuaSharedCreateInterface = reinterpret_cast<CreateInterface_proc>(GetProcAddress(lua_shared_dll, "CreateInterface"));
