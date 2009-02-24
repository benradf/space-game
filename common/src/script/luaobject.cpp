#include "luaobject.hpp"


const char* memoryTable = "b9e9db08-77b6-4a21-b0fb-dc76738078c5";
const char* thisTable = "d59ce2b6-94ea-453b-a1c9-846fcb6ce2a7";


////////// LuaObject //////////

/// \brief Constructor.
///
/// The constructor is passed a Lua virtual machine state so that it can
/// construct the Lua side of the object as well as the C++ side. It saves the
/// object's \em this pointer in the userdata metatable so that it can be 
/// accessed by the LuaObject::finalise method later. It also creates the 
/// userdata's meta-index table ready for registering methods with Lua later. \n
/// [-0, +0, e]
LuaObject::LuaObject(lua_State* lua)
{
    // Map this pointer to userdata.
    luaSetThisToUserdata(lua, this);
    
    // Store this pointer.
    lua_pushlightuserdata(lua, this);
    luaSetMember(lua, -2, "__this");
    
    // Make object methods accessable.
    luaGetMetatable(lua);
    luaGetMetaIndex(lua);
    lua_newtable(lua);
    lua_insert(lua, -2);
    lua_newtable(lua);
    lua_insert(lua, -2);
    lua_setfield(lua, -2, "__index");
    lua_setmetatable(lua, -2);
    lua_setfenv(lua, -3);
    
    lua_pop(lua, 1);
}

/// \brief %LuaObject placement new.
///
/// Since the normal new operator is declared private and the destructor
/// protected (preventing stack creation) this is the only way to create a 
/// LuaObject. It takes a Lua VM state and allocates memory as userdata in that
/// VM. It also adds a mapping between the pointer to the allocated memory and
/// the actual Lua userdata. This is required so that if an exception should be 
/// thrown during construction, placement delete may reset to nil the garbage 
/// collection method that this operator sets initially. \n
/// [-0, +1, e]
/// \param size Number of bytes of memory to allocate.
/// \param lua Lua virtual machine state.
/// \return Pointer to allocated memory.
void* LuaObject::operator new(size_t size, lua_State* lua)
{
    void* p = lua_newuserdata(lua, size);
    
    luaSetMemoryToUserdata(lua, p);
    luaGetMetatable(lua);
    luaSetGCMethod(lua);
    lua_pop(lua, 1);
    
    return p;
}

/// \brief %LuaObject placement delete.
///
/// This operator is automatically invoked if an exception is thrown during
/// construction of a LuaObject or derived type. It does not need to free
/// any memory since its counterpart new operator allocates memory using 
/// lua_newuserdata. Memory allocated in this fashion is garbage collected by
/// Lua. However it does set the __gc metamethod to nil to ensure that the 
/// destructor is not called when Lua collects the memory. This is important 
/// because the object was never fully constructed. \n
/// [-0, +0, e]
/// \param p Address of memory to delete.
/// \param lua Lua virtual machine state.
void LuaObject::operator delete(void* p, lua_State* lua)
{
    luaGetUserdataByMemory(lua, p);
    if (!lua_isnil(lua, -1)) {
        luaGetMetatable(lua);
        lua_pushnil(lua);
        lua_setfield(lua, -2, "__gc");
        lua_pop(lua, 1);
    }
    
    lua_pop(lua, 1);
}

/// \brief Destructor.
LuaObject::~LuaObject()
{
    
}

/// \brief Normal delete operator.
///
/// This should never be called.
void LuaObject::operator delete(void*)
{
    assert(false);
}

/// \brief Gets a member value.
///
/// %LuaObjects should store their member values in the __members table of the
/// userdata metatable. \n
/// [-0, +1, e]
/// \param lua Lua virtual machine state.
/// \param index Userdata stack position.
/// \param name The name of the member to get.
void LuaObject::luaGetMember(lua_State* lua, int index, const char* name)
{
    // Get members table.
    luaGetMetatable(lua, index);
    lua_getfield(lua, -1, "__members");
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        lua_setfield(lua, -3, "__members");
    }
    
    // Get specified field.
    lua_getfield(lua, -1, name);
    
    // Remove members table.
    lua_insert(lua, -3);
    lua_pop(lua, 2);
}

/// \brief Sets a member value.
///
/// This method sets the member field specified by name to the value at the top
/// of the stack. \n
/// [-1, +0, e]
/// \param lua Lua virtual machine state.
/// \param index Userdata stack position.
/// \param name The name of the member to set.
/// \pre The top of the stack contains the value to set.
void LuaObject::luaSetMember(lua_State* lua, int index, const char* name)
{
    // Get members table.
    luaGetMetatable(lua, index);
    lua_getfield(lua, -1, "__members");
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        lua_setfield(lua, -3, "__members");
    }
    
    // Set specified field.
    lua_pushvalue(lua, -3);
    lua_setfield(lua, -2, name);
    
    // Remove members table.
    lua_pop(lua, 3);
}

/// \brief Maps userdata to a \em this pointer
///
/// Returns nil if the userdata is unknown or the \em this pointer table doesn't
/// exist. The userdata should be placed on the top of the stack before calling
/// this function. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \param p Userdata pointer.
void* LuaObject::luaGetThisByUserdata(lua_State* lua)
{
    void* p = 0;
    
    lua_getfield(lua, LUA_REGISTRYINDEX, thisTable);
    if (lua_istable(lua, -1)) {
        lua_pushvalue(lua, -2);
        lua_gettable(lua, -2);
        p = lua_touserdata(lua, -1);
        lua_pop(lua, 1);
    }
    
    lua_pop(lua, 1);
    
    return p;
}

/// \brief Maps a \em this pointer to userdata.
///
/// Returns nil if the \em this pointer table doesn't exist or if the \em this 
/// pointer isn't found. \n
/// [-0, +1, e]
/// \param lua Lua virtual machine state.
/// \param p This pointer to map.
void LuaObject::luaGetUserdataByThis(lua_State* lua, void* p)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, thisTable);
    if (lua_istable(lua, -1)) {
        lua_pushlightuserdata(lua, p);
        lua_gettable(lua, -2);
        lua_insert(lua, -2);
        lua_pop(lua, 1);
    } else {
        lua_pop(lua, 1);
        lua_pushnil(lua);
    }
    
    assert(lua_isuserdata(lua, -1));
}

/// \brief Maps memory addresses to userdata.
///
/// Returns nil if the memory table doesn't exist or if the address isn't 
/// found. \n
/// [-0, +1, e]
/// \param lua Lua virtual machine state.
/// \param p Memory address.
void LuaObject::luaGetUserdataByMemory(lua_State* lua, void* p)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, memoryTable);
    if (lua_istable(lua, -1)) {
        lua_pushlightuserdata(lua, p);
        lua_gettable(lua, -2);
        lua_insert(lua, -2);
        lua_pop(lua, 1);
    } else {
        lua_pop(lua, 1);
        lua_pushnil(lua);
    }
    
    assert(lua_isuserdata(lua, -1));
}

/// \brief Pushes metatable onto the stack.
///
/// This method gets the metatable for the table or userdata at the specified
/// index. If the value does not yet have a metatable it creates an empty one
/// for it and returns that. \n
/// [-0, +1, e]
/// \param lua Lua virtual machine state.
/// \param index Index of table whose metatable should be returned.
/// \pre The value at the index must be a table or userdata.
void LuaObject::luaGetMetatable(lua_State* lua, int index)
{
    assert(lua_istable(lua, index) || lua_isuserdata(lua, index));
    
    if (!lua_getmetatable(lua, index)) {
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        lua_setmetatable(lua, -3);
    }
}

/// \brief Lua type code to string lookup.
///
/// Returns a string representation of a Lua type given the integer type code.
/// \param type Integer type code.
/// \return String representation of type.
const char* LuaObject::typeStr(int type)
{
    switch (type) {
        case LUA_TNONE:          return "NONE";
        case LUA_TNIL:           return "NIL";
        case LUA_TBOOLEAN:       return "BOOLEAN";
        case LUA_TLIGHTUSERDATA: return "LIGHTUSERDATA";
        case LUA_TNUMBER:        return "NUMBER";
        case LUA_TSTRING:        return "STRING";
        case LUA_TTABLE:         return "TABLE";
        case LUA_TFUNCTION:      return "FUNCTION";
        case LUA_TUSERDATA:      return "USERDATA";
        case LUA_TTHREAD:        return "THREAD";
        default:                 return "UNKNOWN";
    }
}

/// \brief Sets the mapping from the \em this pointer to userdata
///
/// The \em this pointer table maps \em this pointers to the corresponding Lua 
/// userdata. Being able to map a \em this pointer to userdata is useful in many
/// situations  and provides added safety because a Lua C function can directly 
/// access the  userdata it is a member of rather than relying on the script to 
/// pass the  correct userdata. If the \em this pointer table does not yet exist
/// it is created by this method. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \param p This pointer to map.
/// \pre The top of the stack contains the userdata to map to.
void LuaObject::luaSetThisToUserdata(lua_State* lua, void* p)
{
    // Try and get table from registry.
    lua_getfield(lua, LUA_REGISTRYINDEX, thisTable);
    
    if (lua_isnil(lua, -1)) {
        // Create new table.
        lua_pop(lua, 1);
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        
        // Make table weak.
        lua_newtable(lua);
        lua_pushstring(lua, "v");
        lua_setfield(lua, -2, "__mode");
        lua_setmetatable(lua, -2);
        
        // Save table in registry.
        lua_setfield(lua, LUA_REGISTRYINDEX, thisTable);
    }
    
    // Insert forward mapping.
    lua_pushlightuserdata(lua, p);
    lua_pushvalue(lua, -3);
    lua_settable(lua, -3);
    
    // Insert reverse mapping.
    lua_pushvalue(lua, -2);
    lua_pushlightuserdata(lua, p);
    lua_settable(lua, -3);
    
    lua_pop(lua, 1);
}

/// \brief Sets the mapping from a memory address to userdata
///
/// The memory table maps memory addresses used by the new and delete operators 
/// to the corresponding Lua userdata. A distinction is drawn between these 
/// memory addresses and an object's \em this pointer because the C++ 
/// specification doesn't guarantee that they are the same (although they are in 
/// most implementations). It is necessary to be able to map memory addresses to 
/// userdata so that the placement delete operator can set the garbage 
/// collection metamethod of a userdatum to nil. If the memory table does not 
/// yet exist it is created by this method. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \param p Memory address.
void LuaObject::luaSetMemoryToUserdata(lua_State* lua, void* p)
{
    // Try and get table from registry.
    lua_getfield(lua, LUA_REGISTRYINDEX, memoryTable);
    
    if (lua_isnil(lua, -1)) {
        // Create new table.
        lua_pop(lua, 1);
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        
        // Make table weak.
        lua_newtable(lua);
        lua_pushstring(lua, "v");
        lua_setfield(lua, -2, "__mode");
        lua_setmetatable(lua, -2);
        
        // Save table in registry.
        lua_setfield(lua, LUA_REGISTRYINDEX, memoryTable);
    }
    
    // Insert into table.
    lua_pushlightuserdata(lua, p);
    lua_pushvalue(lua, -3);
    lua_settable(lua, -3);
    lua_pop(lua, 1);
}


/// \brief Sets the garbage collection metamethod.
///
/// This method sets the GC metamethod to LuaObject::finalise. This object will
/// then be notified by Lua just before it is about to be collected. The 
/// metatable is also protected so that the __gc method cannot be changed from
/// within a Lua script and cause a memory leak. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \pre This metatable for the userdata corresponding to this object must be at
///      the top of the stack.
void LuaObject::luaSetGCMethod(lua_State* lua)
{
    // Set garbage collection method.
    lua_pushcfunction(lua, finalise);
    lua_setfield(lua, -2, "__gc");
    
    // Protect metatable.
    lua_pushboolean(lua, false);
    lua_setfield(lua, -2, "__metatable");
}

/// \brief Gets meta-index table.
///
/// The methods this object exposes to Lua are stored in the meta-index table.
/// This is the table referenced by the metatable's __index entry.
/// [-0, +1, e]
/// \param lua Lua virtual machine state.
/// \pre The userdata's metatable must be the value at the top of the stack.
void LuaObject::luaGetMetaIndex(lua_State* lua)
{
    lua_getfield(lua, -1, "__index");
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        lua_newtable(lua);
        lua_pushvalue(lua, -1);
        lua_setfield(lua, -3, "__index");
    }
}

/// \brief Lua garbage collection method.
///
/// This method is called by Lua just before it garbage collects the userdata.
/// It retrieves the \em this pointer from the userdata's metatable and calls
/// the destructor on that. The userdata address and the \em this pointer are
/// very likely to be the same on most platforms, however it would be a hack to
/// assume this. \n
/// [-0, +0, e]
/// \param lua Lua virtual machine state.
/// \return Number of Lua return values pushed onto stack.
int LuaObject::finalise(lua_State* lua)
{
    luaL_argcheck(lua, lua_isuserdata(lua, 1), 1, 0);
    
    // Destroy object.
    luaGetMember(lua, -1, "__this");
    reinterpret_cast<LuaObject*>(lua_touserdata(lua, 1))->~LuaObject();
    
    return 0;
}
