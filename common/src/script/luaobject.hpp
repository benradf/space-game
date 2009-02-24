/// \file luaobject.hpp
/// \brief Interface between lua and C++.
/// \author Ben Radford 
/// \date 27th July 2007
///
/// Copyright (c) 2007 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):

// TODO: Compile Lua to use exceptions instead of long jumps.

// NOTE: It might be necessary to catch all exceptions at the lua->c call gate.
// If non lua exceptions get propagated back to lua it may leave lua in an
// inconsistent state.

// TODO: Include the userdata in the closure of its C functions. This prevents
// it from being garbage collected while references to its methods still exist,
// even if there are no references to the userdata itself.

// NOTE: Implemented the above todo. Still need to check that it works though.

// TODO: Handle exceptions properly. Currently the gateway from Lua to C catches
// all exceptions to prevent them propagating back into Lua and causing trouble.
// However it asserts false whenever it catches an exception. The best solution
// is probably to raise a lua_error with the exception description instead.

// TODO: Consider using raw set and get functions instead of table set and get.
// This should be more efficient when there is no intention to invoke 
// metamethods anyway. Also use rawseti when setting an array.

// Ben Radford - 1st December 2006
// All rights reserved


#ifndef LUAOBJECT_HPP
#define LUAOBJECT_HPP


extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}


#include <sstream>
#include <typeinfo>
#include <assert.h>
#include <string.h>

/// \brief Handles interface between Lua and C++.
///
/// This class defines a base object which manages the interaction between Lua
/// and C++. Its placement new operator allocates its memory as Lua userdata
/// and registers a garbage collection metamethod so that its destructor is
/// called by Lua when collecting. As a result, this object must be heap rather
/// than stack allocated and must never be manually deleted. C++ users of 
/// %LuaObject's should also take care what references they keep to them since 
/// the references will become invalid if Lua garbage collects the %LuaObject's.
/// Classes deriving from LuaObject may use LuaObject::registerMethod to expose
/// a member function to Lua.
class LuaObject {
    public:
        LuaObject(lua_State* lua);
        
        static void* operator new(size_t size, lua_State* lua);
        static void operator delete(void* p, lua_State* lua);
        
    protected:
        virtual ~LuaObject();
        static void operator delete(void* p);
        
        template<typename T>
        void registerMethod(lua_State* lua, int(T::*method)(lua_State*), 
                            const char* name, bool selfChecked = false);
        
        template<typename T, int N>
        void registerMethod(lua_State* lua, int(T::*method)(lua_State*), 
                            int (&types)[N], const char* name);
        
        template<typename T>
        static T* checkUserdata(lua_State* lua);
        
        static void luaGetMember(lua_State* lua, int index, const char* name);
        static void luaSetMember(lua_State* lua, int index, const char* name);
        
        static void* luaGetThisByUserdata(lua_State* lua);
        static void luaGetUserdataByThis(lua_State* lua, void* p);
        static void luaGetUserdataByMemory(lua_State* lua, void* p);
        
        static void luaGetMetatable(lua_State* lua, int index = -1);
        
        static const char* typeStr(int type);
        
    private:
        LuaObject(const LuaObject&);
        LuaObject& operator=(const LuaObject&);
        static void* operator new(size_t size);
        
        /// \brief Information required to invoke method.
        ///
        /// A instance of this struct is included in the closure of every method
        /// registered with Lua. This makes it possible to dispatch to the 
        /// correct object and method when Lua calls LuaObject::dispatch.
        template<typename T>
        struct MethodInfo {
            LuaObject* objptr;            ///< LuaObject to call method on.
            char methodName[32];          ///< Lua name of method as string.
            int(T::*method)(lua_State*);  ///< C++ member function to call.
            bool selfChecked;             ///< Method checks its own parameters.
            int paramCount;               ///< Number of parameters.
            int paramTypes[1];            ///< Parameter type codes.
        };
        
        template<typename T>
        void registerMethod(lua_State* lua, int(T::*method)(lua_State*),
                        int types[], int count, const char* name, bool checked);
        
        static void luaSetThisToUserdata(lua_State* lua, void* p);
        static void luaSetMemoryToUserdata(lua_State* lua, void* p);
        
        static void luaSetGCMethod(lua_State* lua);
        static void luaGetMetaIndex(lua_State* lua);
        
        template<typename T>
        static int dispatch(lua_State* lua);
        static int finalise(lua_State* lua);
};


////////// LuaObject //////////

/// \brief Adapter for registerMethod.
///
/// Registers a method that takes no parameters. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \param method Member function pointer to method to register.
/// \param name Name to give method in Lua.
template<typename T>
void LuaObject::registerMethod(lua_State* lua, int(T::*method)(lua_State*), 
                               const char* name, bool selfChecked)
{
    registerMethod(lua, method, 0, 0, name, selfChecked);
}


/// \brief Adapter for registerMethod.
///
/// Registers a method that takes N parameters. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \param method Member function pointer to method to register.
/// \param types Array of method parameter type codes.
/// \param name Name to give method in Lua.
template<typename T, int N>
void LuaObject::registerMethod(lua_State* lua, int(T::*method)(lua_State*), 
                               int (&types)[N], const char* name)
{
    registerMethod(lua, method, types, N, name, false);
}


/// \brief Inform Lua of a new method this object is exposing.
///
/// Adds the specified method to the userdata in Lua. It does this by 
/// registering the static LuaObject::dispatch method with a closure containing
/// a MethodInfo object as userdata. This allows dispatch to call the correct
/// method on the correct object. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \param method Member function pointer to method to register.
/// \param types Array of method parameter type codes.
/// \param count Number of method parameters.
/// \param name Name to give method in Lua.
template<typename T>
void LuaObject::registerMethod(lua_State* lua, int(T::*method)(lua_State*),
                         int types[], int count, const char* name, bool checked)
{
    // Get meta-index table.
    luaGetMetatable(lua);
    luaGetMetaIndex(lua);
    
    // Create new method info userdata.
    void* p = lua_newuserdata(lua, sizeof(MethodInfo<T>) + sizeof(int) * count);
    MethodInfo<T>* info = reinterpret_cast<MethodInfo<T>*>(p);
    
    // Set method info.
    info->objptr = this;
    info->method = method;
    info->paramCount = count;
    info->selfChecked = checked;
    for (int i = 0; i < count; i++)
        info->paramTypes[i] = types[i];
    strncpy(info->methodName, name, sizeof(info->methodName));
    
    // Get userdata.
    luaGetUserdataByThis(lua, this);
    
    // Create closure with method info.
    lua_pushcclosure(lua, dispatch<T>, 2);
    
    // Insert closure into meta-index table.
    lua_getfenv(lua, -4);
    lua_setfenv(lua, -2);
    lua_setfield(lua, -2, name);
    
    lua_pop(lua, 2);
}


/// \brief Checks a userdata is of a certain type.
///
/// This method converts the userdata at the top of the lua stack to a C++
/// object pointer. It then checks whether the object is of the type specified
/// in the template parameter. If so it returns the pointer, otherwise zero.
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
template<typename T>
T* LuaObject::checkUserdata(lua_State* lua)
{
    void* p = luaGetThisByUserdata(lua);
    struct Obj { virtual ~Obj() {} }* obj = reinterpret_cast<Obj*>(p);
    
    if (typeid(*obj) != typeid(T))
        return 0;
    
    return reinterpret_cast<T*>(p);
}


/// \brief Determines and invokes method on object.
///
/// Retrieves MethodInfo userdata and dispatches to the correct object and 
/// method. \n
/// [-0, +?, e]
/// \param lua Lua virtual machine state.
/// \return Number of Lua return values pushed onto stack.
template<typename T>
int LuaObject::dispatch(lua_State* lua)
{
    // Retrieve userdata from closure.
    void* userdata = lua_touserdata(lua, lua_upvalueindex(1));
    MethodInfo<T>* info = reinterpret_cast<MethodInfo<T>*>(userdata);
    
    // Check parameters.
    if (!info->selfChecked) {
        for (int i = 0; i < info->paramCount; i++) {
            luaL_argcheck(lua, lua_type(lua, i + 1) 
                == info->paramTypes[i], i + 1, 0);
        }
    }
    
    try {
        return (static_cast<T*>(info->objptr)->*(info->method))(lua);
    } catch (std::exception& e) {
        lua_pushstring(lua, e.what());
        return lua_error(lua);
    }
}


#endif  // LUAOBJECT_HPP
