// Ben Radford - 1st December 2006


#ifndef LUAEXTLIB_HPP
#define LUAEXTLIB_HPP


#include <assert.h>
#include "luaobject.hpp"


class LuaExtLib : public LuaObject {
    public:
        LuaExtLib(lua_State* lua);
        
    private:
        //friend class {};
        virtual ~LuaExtLib();
        
        int luaE_copy(lua_State* lua);
        int luaE_mcopy(lua_State* lua);
        int luaE_isect(lua_State* lua);
        int luaE_union(lua_State* lua);
        int luaE_diff(lua_State* lua);
        int luaE_symdiff(lua_State* lua);
};


#endif  // LUAEXTLIB_HPP
