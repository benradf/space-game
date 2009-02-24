#ifndef LUAUTIL_HPP
#define LUAUTIL_HPP


#include <sys/time.h>
#include "luaobject.hpp"


namespace Lua {

template<typename T>
void protectedCall(lua_State* lua, int args = 0, int rets = 0, int err = 0);

class Util : public LuaObject {
    public:
        Util(lua_State* lua);
        
    private:
        int newStack(lua_State* lua);
        int newQueue(lua_State* lua);
        int newTimer(lua_State* lua);
};


class Stack : public LuaObject {
    public:
        Stack(lua_State* lua);
        
    private:
        int empty(lua_State* lua);
        int size(lua_State* lua);
        int push(lua_State* lua);
        int pop(lua_State* lua);
        int top(lua_State* lua);
};


class Timer : public LuaObject {
    public:
        Timer(lua_State* lua);
        
    private:
        int reset(lua_State* lua);
        int elapsed(lua_State* lua);
        
        timeval _start;
};


}  // namespace Lua


template<typename T>
inline void Lua::protectedCall(lua_State* lua, int args, int rets, int err)
{
    bool defaultError = (err == 0);

    if (defaultError) {
        lua_getglobal(lua, "debug");
        if (!lua_isnil(lua, -1)) {
            lua_getfield(lua, -1, "traceback");
            if (!lua_isnil(lua, -1)) {
                err = -args - 2;
                lua_insert(lua, err - 1);
                lua_pop(lua, 1);
            } else {
                lua_pop(lua, 2);
            }
        } else {
            lua_pop(lua, 1);
        }
    }

    if (lua_pcall(lua, args, rets, err) != 0) {
        if (lua_isstring(lua, -1)) {
            throw T(lua_tostring(lua, -1));
        } else {
            throw T("unknown lua error");
        }
    }

    if (defaultError)
        lua_remove(lua, -rets - 1);
}


#endif  // LUAUTIL_HPP
