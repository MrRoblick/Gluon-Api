#pragma once
#include <lua_defs.h>

class CLuaShared
{
public:
    virtual ~CLuaShared() = 0;
    virtual void pad00() = 0;
    virtual void pad01() = 0;
    virtual void pad02() = 0;
    virtual LuaInterface* CreateLuaInterface() = 0;
    virtual void CloseLuaInterface() = 0;
    virtual LuaInterface* GetLuaInterface(LuaInterfaceType) = 0;
};