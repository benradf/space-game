#include "luaextlib.hpp"


LuaExtLib::LuaExtLib(lua_State* lua)
         : LuaObject(lua)
{
    int oneTable[] = {LUA_TTABLE};
    registerMethod(lua, &LuaExtLib::luaE_copy, oneTable, "copy");
    registerMethod(lua, &LuaExtLib::luaE_mcopy, oneTable, "mcopy");
    
    int twoTables[] = {LUA_TTABLE, LUA_TTABLE};
    registerMethod(lua, &LuaExtLib::luaE_isect, twoTables, "isect");
    registerMethod(lua, &LuaExtLib::luaE_union, twoTables, "union");
    registerMethod(lua, &LuaExtLib::luaE_diff, twoTables, "diff");
    registerMethod(lua, &LuaExtLib::luaE_symdiff, twoTables, "symdiff");
}

LuaExtLib::~LuaExtLib()
{
    
}

int LuaExtLib::luaE_copy(lua_State* lua)
{
    // Create new table.
    lua_newtable(lua);
    
    // Iterate over table.
    lua_pushnil(lua);
    while (lua_next(lua, 1) != 0) {
        // Recurse if entry is a table.
        if (lua_istable(lua, -1)) {
            lua_getfield(lua, LUA_ENVIRONINDEX, "copy");
            lua_insert(lua, -2);
            lua_call(lua, 1, 1);
        }
        
        // Copy entry into new table.
        lua_pushvalue(lua, -2);
        lua_insert(lua, -3);
        lua_settable(lua, -4);
    }
    
    // Set metatable.
    if (lua_getmetatable(lua, 1))
        lua_setmetatable(lua, -2);
    
    return 1;
}

int LuaExtLib::luaE_mcopy(lua_State* lua)
{
    // Copy table entries.
    lua_getfield(lua, LUA_ENVIRONINDEX, "copy");
    lua_pushvalue(lua, 1);
    lua_call(lua, 1, 1);
    
    // Make a seperate copy of metatable.
    if (lua_getmetatable(lua, -1)) {
        lua_getfield(lua, LUA_ENVIRONINDEX, "copy");
        lua_insert(lua, -2);
        lua_call(lua, 1, 1);
        lua_setmetatable(lua, -2);
    }
    
    return 1;
}

int LuaExtLib::luaE_isect(lua_State* lua)
{
    // Create new table.
    lua_newtable(lua);
    
    // Iterate over left table.
    lua_pushnil(lua);
    while (lua_next(lua, 1) != 0) {
        // Does key exist in right table?
        lua_pushvalue(lua, -2);
        lua_gettable(lua, 2);
        
        // Insert entries that share a key.
        if (!lua_isnil(lua, -1)) {
            lua_pop(lua, 1);
            lua_pushvalue(lua, -2);
            lua_insert(lua, -2);
            lua_settable(lua, -4);
        } else {
            lua_pop(lua, 2);
        }
    }
    
    return 1;
}

int LuaExtLib::luaE_union(lua_State* lua)
{
    // Create new table.
    lua_newtable(lua);
    
    // Iterate over right table.
    lua_pushnil(lua);
    while (lua_next(lua, 2) != 0) {
        lua_pushvalue(lua, -2);
        lua_insert(lua, -2);
        lua_settable(lua, -4);
    }
    
    // Iterate over left table.
    lua_pushnil(lua);
    while (lua_next(lua, 1) != 0) {
        lua_pushvalue(lua, -2);
        lua_insert(lua, -2);
        lua_settable(lua, -4);
    }
    
    return 1;
}

int LuaExtLib::luaE_diff(lua_State* lua)
{
    // Create new table.
    lua_newtable(lua);
    
    // Iterate over left table.
    lua_pushnil(lua);
    while (lua_next(lua, 1) != 0) {
        // Does key exist in right table?
        lua_pushvalue(lua, -2);
        lua_gettable(lua, 2);
        
        // Insert entries that aren't in right table.
        if (lua_isnil(lua, -1)) {
            lua_pop(lua, 1);
            lua_pushvalue(lua, -2);
            lua_insert(lua, -2);
            lua_settable(lua, -4);
        } else {
            lua_pop(lua, 2);
        }
    }
    
    return 1;
}

int LuaExtLib::luaE_symdiff(lua_State* lua)
{
    // Get union function.
    lua_getfield(lua, LUA_ENVIRONINDEX, "union");
    
    // Get diff function twice.
    lua_getfield(lua, LUA_ENVIRONINDEX, "diff");
    lua_pushvalue(lua, -1);
    
    // Do 'left diff right'.
    lua_pushvalue(lua, 1);
    lua_pushvalue(lua, 2);
    lua_call(lua, 2, 1);
    lua_insert(lua, -2);
    
    // Do 'right diff left'.
    lua_pushvalue(lua, 2);
    lua_pushvalue(lua, 1);
    lua_call(lua, 2, 1);
    
    // Apply union to the results.
    lua_call(lua, 2, 1);
    
    return 1;
}
