#include "luautil.hpp"


////////// Lua::Util //////////

Lua::Util::Util(lua_State* lua)
    : LuaObject(lua)
{
    registerMethod(lua, &Util::newStack, "new_stack");
    registerMethod(lua, &Util::newQueue, "new_queue");
    registerMethod(lua, &Util::newTimer, "new_timer");
}

int Lua::Util::newStack(lua_State* lua)
{
    new(lua) Stack(lua);
    
    return 1;
}

int Lua::Util::newQueue(lua_State* lua)
{
    return luaL_error(lua, "not implemented: queue");
}

int Lua::Util::newTimer(lua_State* lua)
{
    new(lua) Timer(lua);
    
    return 1;
}


////////// Lua::Stack //////////

Lua::Stack::Stack(lua_State* lua)
    : LuaObject(lua)
{
    // Register methods.
    registerMethod(lua, &Stack::empty, "empty", true);
    registerMethod(lua, &Stack::size, "size", true);
    registerMethod(lua, &Stack::push, "push", true);
    registerMethod(lua, &Stack::pop, "pop", true);
    registerMethod(lua, &Stack::top, "top", true);
    
    // Initialise stack.
    lua_newtable(lua);
    luaSetMember(lua, -2, "stack");
    lua_pushnumber(lua, 0);
    luaSetMember(lua, -2, "count");
}

int Lua::Stack::empty(lua_State* lua)
{int newTimer(lua_State* lua);
    if (lua_gettop(lua) != 0)
        luaL_error(lua, "empty expects 0 arguments");
    
    size(lua);
    
    lua_pushboolean(lua, lua_tonumber(lua, -1) == 0);
    lua_insert(lua, -2);
    lua_pop(lua, 1);
    
    return 1;
}

int Lua::Stack::size(lua_State* lua)
{
    if (lua_gettop(lua) != 0)
        luaL_error(lua, "size expects 0 arguments");
    
    // Fetch count.
    luaGetUserdataByThis(lua, this);
    luaGetMember(lua, -1, "count");
    
    // Remove userdata.
    lua_insert(lua, -2);
    lua_pop(lua, 1);
    
    return 1;
}

int Lua::Stack::push(lua_State* lua)
{
    luaL_argcheck(lua, !lua_isnil(lua, 1), 1, 0);
    if (lua_gettop(lua) != 1)
        luaL_error(lua, "push expects 1 argument");
    
    // Get stack table and count.
    luaGetUserdataByThis(lua, this);
    assert(lua_isuserdata(lua, -1));
    luaGetMember(lua, -1, "stack");
    luaGetMember(lua, -2, "count");
    
    // Increment count.
    lua_pushnumber(lua, lua_tonumber(lua, -1) + 1);
    lua_pushvalue(lua, -1);
    luaSetMember(lua, -5, "count");
    
    // Push value.
    lua_pushvalue(lua, 1);
    lua_settable(lua, -4);
    
    // Remove stack table.
    lua_pop(lua, 3);
    
    return 0;
}

int Lua::Stack::pop(lua_State* lua)
{
    if (lua_gettop(lua) != 0)
        luaL_error(lua, "pop expects 0 arguments");
    
    // Get stack table and count.
    luaGetUserdataByThis(lua, this);
    luaGetMember(lua, 1, "stack");
    luaGetMember(lua, 1, "count");
    
    // Check stack not empty.
    if (lua_tonumber(lua, -1) == 0)
        luaL_error(lua, "cannot pop from an empty stack");
    
    // Decrement count.
    lua_pushnumber(lua, lua_tonumber(lua, -1) - 1);
    luaSetMember(lua, -4, "count");
    
    // Get top value.
    lua_pushvalue(lua, -1);
    lua_gettable(lua, -3);
    
    // Pop from stack.
    lua_insert(lua, -2);
    lua_pushnil(lua);
    lua_settable(lua, -4);
    
    // Remove stack table.
    lua_insert(lua, -2);
    lua_pop(lua, 1);
    
    return 1;
}

int Lua::Stack::top(lua_State* lua)
{
    if (lua_gettop(lua) != 0)
        luaL_error(lua, "top expects 0 arguments");
    
    // Get stack table and count.
    luaGetUserdataByThis(lua, this);
    luaGetMember(lua, 1, "stack");
    luaGetMember(lua, 1, "count");
    
    // Check stack not empty.
    if (lua_tonumber(lua, -1) == 0)
        luaL_error(lua, "cannot take top of an empty stack");
    
    // Get top value.
    lua_gettable(lua, -2);
    
    // Remove stack table.
    lua_insert(lua, -3);
    lua_pop(lua, 2);
    
    return 1;
}


////////// Lua::Timer //////////

Lua::Timer::Timer(lua_State* lua)
    : LuaObject(lua)
{
    registerMethod(lua, &Timer::reset, "reset");
    registerMethod(lua, &Timer::elapsed, "elapsed");
    
    reset(lua);
}

int Lua::Timer::reset(lua_State*)
{
    gettimeofday(&_start, 0);
    
    return 0;
}

int Lua::Timer::elapsed(lua_State* lua)
{
    timeval now;
    gettimeofday(&now, 0);
    
    int deltaSecond = now.tv_sec - _start.tv_sec;
    int deltaMicrosecond = now.tv_usec - _start.tv_usec;
     
    lua_pushnumber(lua, 1000000 * deltaSecond + deltaMicrosecond);
    
    return 1;
}


