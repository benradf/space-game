/// \file luarpc.hpp
/// \brief A remote procedure call system for the Lua API.
/// \author Ben Radford 
/// \date 17th May 2008
///
/// Copyright (c) 2008 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):


#ifndef LUARPC_HPP
#define LUARPC_HPP

#include <memory.h>
#include <assert.h>
#include <luautil.hpp>


// cout << "vptr = " << (*(void**)this) << "    " << this << "::" << __FUNCTION__ << ":" << __LINE__ << endl;


class luaR_base {
    public:
        luaR_base();
        luaR_base(const luaR_base& that);
        virtual luaR_base& operator=(const luaR_base& that);
        virtual ~luaR_base();

        void operator()(lua_State* lua);

    protected:
        const char* copyString(const char* s) const;
        void deleteString(const char* s) const;

        template<typename T>
        void set(const T& data);
        template<typename T>
        const T& get() const;
        template<typename T>
        T& get();

    private:
        virtual void invoke(lua_State* lua);
        virtual void copy(const luaR_base& that);

        char _data[16];
};

struct luaR_atpanic : public luaR_base {
    luaR_atpanic(lua_CFunction panicf);
    virtual void invoke(lua_State* lua);
};

struct luaR_call : public luaR_base {
    luaR_call(int nargs, int nresults);
    virtual void invoke(lua_State* lua);
};

struct luaR_checkstack : public luaR_base {
    luaR_checkstack(int extra);
    virtual void invoke(lua_State* lua);
};

struct luaR_close : public luaR_base {
    luaR_close();
    virtual void invoke(lua_State* lua);
};

struct luaR_concat : public luaR_base {
    luaR_concat(int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_cpcall : public luaR_base {
    luaR_cpcall(lua_CFunction func, void* ud);
    virtual void invoke(lua_State* lua);
};

struct luaR_createtable : public luaR_base {
    luaR_createtable(int narr, int nrec);
    virtual void invoke(lua_State* lua);
};

struct luaR_dump : public luaR_base {
    luaR_dump(lua_Writer writer, void* data);
    virtual void invoke(lua_State* lua);
};

struct luaR_error : public luaR_base {
    luaR_error();
    virtual void invoke(lua_State* lua);
};

struct luaR_gc : public luaR_base {
    luaR_gc(int what, int data);
    virtual void invoke(lua_State* lua);
};

struct luaR_getfenv : public luaR_base {
    luaR_getfenv(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_getfield : public luaR_base {
    luaR_getfield(int index, const char* k);
    virtual ~luaR_getfield();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_getglobal : public luaR_base {
    luaR_getglobal(const char* name);
    virtual ~luaR_getglobal();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_getinfo : public luaR_base {
    luaR_getinfo(const char* what, lua_Debug* ar);
    virtual ~luaR_getinfo();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_getlocal : public luaR_base {
    luaR_getlocal(lua_Debug* ar, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_getmetatable : public luaR_base {
    luaR_getmetatable(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_getstack : public luaR_base {
    luaR_getstack(int level, lua_Debug* ar);
    virtual void invoke(lua_State* lua);
};

struct luaR_gettable : public luaR_base {
    luaR_gettable(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_getupvalue : public luaR_base {
    luaR_getupvalue(int funcindex, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_insert : public luaR_base {
    luaR_insert(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_load : public luaR_base {
    luaR_load(lua_Reader reader, void* data, const char* chunkname);
    virtual ~luaR_load();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_newtable : public luaR_base {
    luaR_newtable();
    virtual void invoke(lua_State* lua);
};

struct luaR_newthread : public luaR_base {
    luaR_newthread();
    virtual void invoke(lua_State* lua);
};

struct luaR_newuserdata : public luaR_base {
    luaR_newuserdata(size_t size);
    virtual void invoke(lua_State* lua);
};

struct luaR_next : public luaR_base {
    luaR_next(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_objlen : public luaR_base {
    luaR_objlen(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_pcall : public luaR_base {
    luaR_pcall(int nargs, int nresults, int errfunc);
    virtual void invoke(lua_State* lua);
};

struct luaR_pop : public luaR_base {
    luaR_pop(int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushboolean : public luaR_base {
    luaR_pushboolean(int b);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushcclosure : public luaR_base {
    luaR_pushcclosure(lua_CFunction fn, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushcfunction : public luaR_base {
    luaR_pushcfunction(lua_CFunction f);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushinteger : public luaR_base {
    luaR_pushinteger(lua_Integer n);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushlightuserdata : public luaR_base {
    luaR_pushlightuserdata(void* p);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushlstring : public luaR_base {
    luaR_pushlstring(const char* s, size_t len);
    virtual ~luaR_pushlstring();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_pushnil : public luaR_base {
    luaR_pushnil();
    virtual void invoke(lua_State* lua);
};

struct luaR_pushnumber : public luaR_base {
    luaR_pushnumber(lua_Number n);
    virtual void invoke(lua_State* lua);
};

struct luaR_pushstring : public luaR_base {
    luaR_pushstring(const char* s);
    virtual ~luaR_pushstring();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_pushthread : public luaR_base {
    luaR_pushthread();
    virtual void invoke(lua_State* lua);
};

struct luaR_pushvalue : public luaR_base {
    luaR_pushvalue(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_rawget : public luaR_base {
    luaR_rawget(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_rawgeti : public luaR_base {
    luaR_rawgeti(int index, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_rawset : public luaR_base {
    luaR_rawset(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_rawseti : public luaR_base {
    luaR_rawseti(int index, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_register : public luaR_base {
    luaR_register(const char* name, lua_CFunction f);
    virtual ~luaR_register();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_remove : public luaR_base {
    luaR_remove(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_replace : public luaR_base {
    luaR_replace(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_resume : public luaR_base {
    luaR_resume(int narg);
    virtual void invoke(lua_State* lua);
};

struct luaR_setallocf : public luaR_base {
    luaR_setallocf(lua_Alloc f, void* ud);
    virtual void invoke(lua_State* lua);
};

struct luaR_setfenv : public luaR_base {
    luaR_setfenv(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_setfield : public luaR_base {
    luaR_setfield(int index, const char* k);
    virtual ~luaR_setfield();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_setglobal : public luaR_base {
    luaR_setglobal(const char* name);
    virtual ~luaR_setglobal();
    virtual void invoke(lua_State* lua);
    virtual void copy(const luaR_base& that);
};

struct luaR_sethook : public luaR_base {
    luaR_sethook(lua_Hook f, int mask, int count);
    virtual void invoke(lua_State* lua);
};

struct luaR_setlocal : public luaR_base {
    luaR_setlocal(lua_Debug* ar, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_setmetatable : public luaR_base {
    luaR_setmetatable(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_settable : public luaR_base {
    luaR_settable(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_settop : public luaR_base {
    luaR_settop(int index);
    virtual void invoke(lua_State* lua);
};

struct luaR_setupvalue : public luaR_base {
    luaR_setupvalue(int funcindex, int n);
    virtual void invoke(lua_State* lua);
};

struct luaR_yield : public luaR_base {
    luaR_yield(int nresults);
    virtual void invoke(lua_State* lua);
};


////////// luaR_base //////////

inline luaR_base::luaR_base()
{

}

inline luaR_base::luaR_base(const luaR_base& that)
{
    operator=(that);
}

inline luaR_base& luaR_base::operator=(const luaR_base& that)
{
    memcpy(this, &that, sizeof(*this));
    copy(that);
    return *this;
}

inline luaR_base::~luaR_base()
{

}

inline void luaR_base::operator()(lua_State* lua)
{
    invoke(lua);
}

inline const char* luaR_base::copyString(const char* s) const
{
    size_t len = strlen(s);
    char* str = new char[len+1];
    strcpy(str, s);

    return str;
}

inline void luaR_base::deleteString(const char* s) const
{
    delete[] s;
}

template<typename T>
inline void luaR_base::set(const T& data)
{
    assert(sizeof(T) <= sizeof(_data));
    *reinterpret_cast<T*>(_data) = data;
}

template<typename T>
inline const T& luaR_base::get() const
{
    assert(sizeof(T) <= sizeof(_data));
    return *reinterpret_cast<const T*>(_data);
}

template<typename T>
inline T& luaR_base::get()
{
    assert(sizeof(T) <= sizeof(_data));
    return *reinterpret_cast<T*>(_data);
}

inline void luaR_base::invoke(lua_State* lua)
{

}

inline void luaR_base::copy(const luaR_base& that)
{

}


////////// luaR_atpanic //////////

inline luaR_atpanic::luaR_atpanic(lua_CFunction panicf)
{
    set(panicf);
}

inline void luaR_atpanic::invoke(lua_State* lua)
{
    lua_atpanic(lua, get<lua_CFunction>());
}


////////// luaR_call //////////

struct data_call { int nargs; int nresults; };

inline luaR_call::luaR_call(int nargs, int nresults)
{
    get<data_call>().nargs = nargs;
    get<data_call>().nresults = nresults;
}

inline void luaR_call::invoke(lua_State* lua)
{
    lua_call(lua, get<data_call>().nargs, get<data_call>().nresults);
}


////////// luaR_checkstack //////////

inline luaR_checkstack::luaR_checkstack(int extra)
{
    set(extra);
}

inline void luaR_checkstack::invoke(lua_State* lua)
{
    if (lua_checkstack(lua, get<int>()) == 0)
        throw ScriptException("luaR_checkstack: checkstack failed");
}


////////// luaR_close //////////

inline luaR_close::luaR_close()
{

}

inline void luaR_close::invoke(lua_State* lua)
{
    lua_close(lua);
}


////////// luaR_concat //////////

inline luaR_concat::luaR_concat(int n)
{
    set(n);
}

inline void luaR_concat::invoke(lua_State* lua)
{
    lua_concat(lua, get<int>());
}


////////// luaR_cpcall //////////

struct data_cpcall { lua_CFunction func; void* ud; };

inline luaR_cpcall::luaR_cpcall(lua_CFunction func, void* ud)
{
    get<data_cpcall>().func = func;
    get<data_cpcall>().ud = ud;
}

inline void luaR_cpcall::invoke(lua_State* lua)
{
    if (lua_cpcall(lua, get<data_cpcall>().func, get<data_cpcall>().ud) != 0) {
        if (lua_isstring(lua, -1)) {
            throw ScriptException(lua_tostring(lua, -1));
        } else {
            throw ScriptException("luaR_cpcall: failed");
        }
    }
}


////////// luaR_createtable //////////

struct data_createtable { int narr; int nrec; };

inline luaR_createtable::luaR_createtable(int narr, int nrec)
{
    get<data_createtable>().narr = narr;
    get<data_createtable>().nrec = nrec;
}

inline void luaR_createtable::invoke(lua_State* lua)
{
    lua_createtable(lua, 
        get<data_createtable>().narr,
        get<data_createtable>().nrec);
}


////////// luaR_dump //////////

struct data_dump { lua_Writer writer; void* data; };

inline luaR_dump::luaR_dump(lua_Writer writer, void* data)
{
    get<data_dump>().writer = writer;
    get<data_dump>().data = data;
}

inline void luaR_dump::invoke(lua_State* lua)
{
    lua_dump(lua, get<data_dump>().writer, get<data_dump>().data);
}


////////// luaR_error //////////

inline luaR_error::luaR_error()
{

}

inline void luaR_error::invoke(lua_State* lua)
{
    lua_error(lua);
}


////////// luaR_gc //////////


struct data_gc { int what; int data; };

inline luaR_gc::luaR_gc(int what, int data)
{
    get<data_gc>().what = what;
    get<data_gc>().data = data;
}

inline void luaR_gc::invoke(lua_State* lua)
{
    lua_gc(lua, get<data_gc>().what, get<data_gc>().data);
}


////////// luaR_getfenv //////////

inline luaR_getfenv::luaR_getfenv(int index)
{
    set(index);
}

inline void luaR_getfenv::invoke(lua_State* lua)
{
    lua_getfenv(lua, get<int>());
}


////////// luaR_getfield //////////

struct data_getfield { int index; const char* k; };

inline luaR_getfield::luaR_getfield(int index, const char* k)
{
    get<data_getfield>().index = index;
    get<data_getfield>().k = copyString(k);
}

inline luaR_getfield::~luaR_getfield()
{
    deleteString(get<data_getfield>().k);
}

inline void luaR_getfield::invoke(lua_State* lua)
{
    lua_getfield(lua, get<data_getfield>().index, get<data_getfield>().k);
}

inline void luaR_getfield::copy(const luaR_base& that)
{
    get<data_getfield>().index = that.get<data_getfield>().index;
    get<data_getfield>().k = copyString(that.get<data_getfield>().k);
}


////////// luaR_getglobal //////////

inline luaR_getglobal::luaR_getglobal(const char* name)
{
    set(copyString(name));
}

inline luaR_getglobal::~luaR_getglobal()
{
    deleteString(get<const char*>());
}

inline void luaR_getglobal::invoke(lua_State* lua)
{
    lua_getglobal(lua, get<const char*>());
}

inline void luaR_getglobal::copy(const luaR_base& that)
{
    set(copyString(that.get<const char*>()));
}


////////// luaR_getinfo //////////

struct data_getinfo { const char* what; lua_Debug* ar; };

inline luaR_getinfo::luaR_getinfo(const char* what, lua_Debug* ar)
{
    get<data_getinfo>().what = copyString(what);
    get<data_getinfo>().ar = ar;
}

inline luaR_getinfo::~luaR_getinfo()
{
    deleteString(get<data_getinfo>().what);
}

inline void luaR_getinfo::invoke(lua_State* lua)
{
    lua_getinfo(lua, get<data_getinfo>().what, get<data_getinfo>().ar);
}

inline void luaR_getinfo::copy(const luaR_base& that)
{
    get<data_getinfo>().what = copyString(that.get<data_getinfo>().what);
    get<data_getinfo>().ar = get<data_getinfo>().ar;
}


////////// luaR_getlocal //////////

struct data_getlocal { lua_Debug* ar; int n; };

inline luaR_getlocal::luaR_getlocal(lua_Debug* ar, int n)
{
    get<data_getlocal>().ar = ar;
    get<data_getlocal>().n = n;
}

inline void luaR_getlocal::invoke(lua_State* lua)
{
    lua_getlocal(lua, get<data_getlocal>().ar, get<data_getlocal>().n);
}


////////// luaR_getmetatable //////////

inline luaR_getmetatable::luaR_getmetatable(int index)
{
    set(index);
}

inline void luaR_getmetatable::invoke(lua_State* lua)
{
    lua_getmetatable(lua, get<int>());
}

////////// luaR_getstack //////////

struct data_getstack { int level; lua_Debug* ar; };

inline luaR_getstack::luaR_getstack(int level, lua_Debug* ar)
{
    get<data_getstack>().level = level;
    get<data_getstack>().ar = ar;
}

inline void luaR_getstack::invoke(lua_State* lua)
{
    lua_getstack(lua, get<data_getstack>().level, get<data_getstack>().ar);
}



////////// luaR_gettable //////////

inline luaR_gettable::luaR_gettable(int index)
{
    set(index);
}

inline void luaR_gettable::invoke(lua_State* lua)
{
    lua_gettable(lua, get<int>());
}


////////// luaR_getupvalue //////////

struct data_getupvalue { int funcindex; int n; };

inline luaR_getupvalue::luaR_getupvalue(int funcindex, int n)
{
    get<data_getupvalue>().funcindex = funcindex;
    get<data_getupvalue>().n = n;
}

inline void luaR_getupvalue::invoke(lua_State* lua)
{
    lua_getupvalue(lua, get<data_getupvalue>().funcindex, get<data_getupvalue>().n);
}


////////// luaR_insert //////////

inline luaR_insert::luaR_insert(int index)
{
    set(index);
}

inline void luaR_insert::invoke(lua_State* lua)
{
    lua_insert(lua, get<int>());
}


////////// luaR_load //////////

struct data_load { lua_Reader reader; void* data; const char* chunkname; };

inline luaR_load::luaR_load(lua_Reader reader, void* data, const char* chunkname)
{
    get<data_load>().reader = reader;
    get<data_load>().data = data;
    get<data_load>().chunkname = copyString(chunkname);
}

inline luaR_load::~luaR_load()
{
    deleteString(get<data_load>().chunkname);
}

inline void luaR_load::invoke(lua_State* lua)
{
    lua_load(lua, 
        get<data_load>().reader,
        get<data_load>().data,
        get<data_load>().chunkname);
}

inline void luaR_load::copy(const luaR_base& that)
{
    get<data_load>().reader = get<data_load>().reader;
    get<data_load>().data = get<data_load>().data;
    get<data_load>().chunkname = copyString(get<data_load>().chunkname);
}


////////// luaR_newtable //////////

inline luaR_newtable::luaR_newtable()
{

}

inline void luaR_newtable::invoke(lua_State* lua)
{
    lua_newtable(lua);
}


////////// luaR_newthread //////////

inline luaR_newthread::luaR_newthread()
{

}

inline void luaR_newthread::invoke(lua_State* lua)
{
    lua_newthread(lua);
}


////////// luaR_newuserdata //////////

inline luaR_newuserdata::luaR_newuserdata(size_t size)
{
    set(size);
}

inline void luaR_newuserdata::invoke(lua_State* lua)
{
    lua_newuserdata(lua, get<size_t>());
}


////////// luaR_next //////////

inline luaR_next::luaR_next(int index)
{
    set(index);
}

inline void luaR_next::invoke(lua_State* lua)
{
    lua_next(lua, get<int>());
}


////////// luaR_objlen //////////

inline luaR_objlen::luaR_objlen(int index)
{
    set(index);
}

inline void luaR_objlen::invoke(lua_State* lua)
{
    lua_objlen(lua, get<int>());
}


////////// luaR_pcall //////////

struct data_pcall { int nargs; int nresults; int errfunc; };

inline luaR_pcall::luaR_pcall(int nargs, int nresults, int errfunc)
{
    get<data_pcall>().nargs = nargs;
    get<data_pcall>().nresults = nresults;
    get<data_pcall>().errfunc = errfunc;
}

inline void luaR_pcall::invoke(lua_State* lua)
{
    Lua::protectedCall<ScriptException>(lua,
        get<data_pcall>().nargs,
        get<data_pcall>().nresults,
        get<data_pcall>().errfunc);
}


////////// luaR_pop //////////

inline luaR_pop::luaR_pop(int n)
{
    set(n);
}

inline void luaR_pop::invoke(lua_State* lua)
{
    lua_pop(lua, get<int>());
}


////////// luaR_pushboolean //////////

inline luaR_pushboolean::luaR_pushboolean(int b)
{
    set(b);
}

inline void luaR_pushboolean::invoke(lua_State* lua)
{
    lua_pushboolean(lua, get<int>());
}


////////// luaR_pushcclosure //////////

struct data_pushcclosure { lua_CFunction fn; int n; };

inline luaR_pushcclosure::luaR_pushcclosure(lua_CFunction fn, int n)
{
    get<data_pushcclosure>().fn = fn;
    get<data_pushcclosure>().n = n;
}

inline void luaR_pushcclosure::invoke(lua_State* lua)
{
    lua_pushcclosure(lua,
        get<data_pushcclosure>().fn,
        get<data_pushcclosure>().n);
}


////////// luaR_pushcfunction //////////

inline luaR_pushcfunction::luaR_pushcfunction(lua_CFunction f)
{
    set(f);
}

inline void luaR_pushcfunction::invoke(lua_State* lua)
{
    lua_pushcfunction(lua, get<lua_CFunction>());
}


////////// luaR_pushinteger //////////

inline luaR_pushinteger::luaR_pushinteger(lua_Integer n)
{
    set(n);
}

inline void luaR_pushinteger::invoke(lua_State* lua)
{
    lua_pushinteger(lua, get<lua_Integer>());
}


////////// luaR_pushlightuserdata //////////

inline luaR_pushlightuserdata::luaR_pushlightuserdata(void* p)
{
    set(p);
}

inline void luaR_pushlightuserdata::invoke(lua_State* lua)
{
    lua_pushlightuserdata(lua, get<void*>());
}


////////// luaR_pushlstring //////////

struct data_pushlstring { const char* s; size_t len; };

inline luaR_pushlstring::luaR_pushlstring(const char* s, size_t len)
{
    get<data_pushlstring>().s = copyString(s);
    get<data_pushlstring>().len = len;
}

inline luaR_pushlstring::~luaR_pushlstring()
{
    deleteString(get<const char*>());
}

inline void luaR_pushlstring::invoke(lua_State* lua)
{
    lua_pushlstring(lua, get<data_pushlstring>().s, get<data_pushlstring>().len);
}

inline void luaR_pushlstring::copy(const luaR_base& that)
{
    get<data_pushlstring>().s = copyString(that.get<data_pushlstring>().s);
    get<data_pushlstring>().len = that.get<data_pushlstring>().len;
}


////////// luaR_pushnil //////////

inline luaR_pushnil::luaR_pushnil()
{

}

inline void luaR_pushnil::invoke(lua_State* lua)
{
    lua_pushnil(lua);
}


////////// luaR_pushnumber //////////

inline luaR_pushnumber::luaR_pushnumber(lua_Number n)
{
    set(n);
}

inline void luaR_pushnumber::invoke(lua_State* lua)
{
    lua_pushnumber(lua, get<lua_Number>());
}


////////// luaR_pushstring //////////

inline luaR_pushstring::luaR_pushstring(const char* s)
{
    set(copyString(s));
}

inline void luaR_pushstring::copy(const luaR_base& that)
{
    set(copyString(that.get<const char*>()));
}

inline luaR_pushstring::~luaR_pushstring()
{
    deleteString(get<const char*>());
}

inline void luaR_pushstring::invoke(lua_State* lua)
{
    lua_pushstring(lua, get<const char*>());
}


////////// luaR_pushthread //////////

inline luaR_pushthread::luaR_pushthread()
{

}

inline void luaR_pushthread::invoke(lua_State* lua)
{
    lua_pushthread(lua);
}


////////// luaR_pushvalue //////////

inline luaR_pushvalue::luaR_pushvalue(int index)
{
    set(index);
}

inline void luaR_pushvalue::invoke(lua_State* lua)
{
    lua_pushvalue(lua, get<int>());
}


////////// luaR_rawget //////////

inline luaR_rawget::luaR_rawget(int index)
{
    set(index);
}

inline void luaR_rawget::invoke(lua_State* lua)
{
    lua_rawget(lua, get<int>());
}


////////// luaR_rawgeti //////////

struct data_rawgeti { int index; int n; };

inline luaR_rawgeti::luaR_rawgeti(int index, int n)
{
    get<data_rawgeti>().index = index;
    get<data_rawgeti>().n = n;
}

inline void luaR_rawgeti::invoke(lua_State* lua)
{
    lua_rawgeti(lua, get<data_rawgeti>().index, get<data_rawgeti>().n);
}


////////// luaR_rawset //////////

inline luaR_rawset::luaR_rawset(int index)
{
    set(index);
}

inline void luaR_rawset::invoke(lua_State* lua)
{
    lua_rawset(lua, get<int>());
}


////////// luaR_rawseti //////////

struct data_rawseti { int index; int n; };

inline luaR_rawseti::luaR_rawseti(int index, int n)
{
    get<data_rawseti>().index = index;
    get<data_rawseti>().n = n;
}

inline void luaR_rawseti::invoke(lua_State* lua)
{
    lua_rawseti(lua, get<data_rawseti>().index, get<data_rawseti>().n);
}


////////// luaR_register //////////

struct data_register { const char* name; lua_CFunction f; };

inline luaR_register::luaR_register(const char* name, lua_CFunction f)
{
    get<data_register>().name = copyString(name);
    get<data_register>().f = f;
}

inline luaR_register::~luaR_register()
{
    deleteString(get<data_register>().name);
}

inline void luaR_register::invoke(lua_State* lua)
{
    lua_register(lua, get<data_register>().name, get<data_register>().f);
}

inline void luaR_register::copy(const luaR_base& that)
{
    get<data_register>().name = copyString(that.get<data_register>().name);
    get<data_register>().f = that.get<data_register>().f;
}


////////// luaR_remove //////////

inline luaR_remove::luaR_remove(int index)
{
    set(index);
}

inline void luaR_remove::invoke(lua_State* lua)
{
    lua_remove(lua, get<int>());
}


////////// luaR_replace //////////

inline luaR_replace::luaR_replace(int index)
{
    set(index);
}

inline void luaR_replace::invoke(lua_State* lua)
{
    lua_replace(lua, get<int>());
}


////////// luaR_resume //////////

inline luaR_resume::luaR_resume(int narg)
{
    set(narg);
}

inline void luaR_resume::invoke(lua_State* lua)
{
    lua_resume(lua, get<int>());
}


////////// luaR_setallocf //////////

struct data_setallocf { lua_Alloc f; void* ud; };

inline luaR_setallocf::luaR_setallocf(lua_Alloc f, void* ud)
{
    get<data_setallocf>().f = f;
    get<data_setallocf>().ud = ud;
}

inline void luaR_setallocf::invoke(lua_State* lua)
{
    lua_setallocf(lua, get<data_setallocf>().f, get<data_setallocf>().ud);
}


////////// luaR_setfenv //////////

inline luaR_setfenv::luaR_setfenv(int index)
{
    set(index);
}

inline void luaR_setfenv::invoke(lua_State* lua)
{
    lua_setfenv(lua, get<int>());
}


////////// luaR_setfield //////////

struct data_setfield { int index; const char* k; };

inline luaR_setfield::luaR_setfield(int index, const char* k)
{
    get<data_setfield>().index = index;
    get<data_setfield>().k = copyString(k);
}

inline luaR_setfield::~luaR_setfield()
{
    deleteString(get<data_setfield>().k);
}

inline void luaR_setfield::invoke(lua_State* lua)
{
    lua_setfield(lua, get<data_setfield>().index, get<data_setfield>().k);
}

inline void luaR_setfield::copy(const luaR_base& that)
{
    get<data_setfield>().index = that.get<data_setfield>().index;
    get<data_setfield>().k = copyString(that.get<data_setfield>().k);
}


////////// luaR_setglobal //////////

inline luaR_setglobal::luaR_setglobal(const char* name)
{
    set(name);
}

inline luaR_setglobal::~luaR_setglobal()
{
    deleteString(get<const char*>());
}

inline void luaR_setglobal::invoke(lua_State* lua)
{
    lua_setglobal(lua, get<const char*>());
}

inline void luaR_setglobal::copy(const luaR_base& that)
{
    set(copyString(that.get<const char*>()));
}


////////// luaR_sethook //////////

struct data_sethook { lua_Hook f; int mask; int count; };

inline luaR_sethook::luaR_sethook(lua_Hook f, int mask, int count)
{
    get<data_sethook>().f = f;
    get<data_sethook>().mask = mask;
    get<data_sethook>().count = count;
}

inline void luaR_sethook::invoke(lua_State* lua)
{
    lua_sethook(lua,
        get<data_sethook>().f,
        get<data_sethook>().mask,
        get<data_sethook>().count);
}


////////// luaR_setlocal //////////

struct data_setlocal { lua_Debug* ar; int n; };

inline luaR_setlocal::luaR_setlocal(lua_Debug* ar, int n)
{
    get<data_setlocal>().ar = ar;
    get<data_setlocal>().n = n;
}

inline void luaR_setlocal::invoke(lua_State* lua)
{
    lua_setlocal(lua, get<data_setlocal>().ar, get<data_setlocal>().n);
}


////////// luaR_setmetatable //////////

inline luaR_setmetatable::luaR_setmetatable(int index)
{
    set(index);
}

inline void luaR_setmetatable::invoke(lua_State* lua)
{
    lua_setmetatable(lua, get<int>());
}


////////// luaR_settable //////////

inline luaR_settable::luaR_settable(int index)
{
    set(index);
}

inline void luaR_settable::invoke(lua_State* lua)
{
    lua_settable(lua, get<int>());
}


////////// luaR_settop //////////

inline luaR_settop::luaR_settop(int index)
{
    set(index);
}

inline void luaR_settop::invoke(lua_State* lua)
{
    lua_settop(lua, get<int>());
}


////////// luaR_setupvalue //////////

struct data_setupvalue { int funcindex; int n; };

inline luaR_setupvalue::luaR_setupvalue(int funcindex, int n)
{
    get<data_setupvalue>().funcindex = funcindex;
    get<data_setupvalue>().n = n;
}

inline void luaR_setupvalue::invoke(lua_State* lua)
{
    lua_setupvalue(lua, get<data_setupvalue>().funcindex, get<data_setupvalue>().n);
}


////////// luaR_yield //////////

inline luaR_yield::luaR_yield(int nresults)
{
    set(nresults);
}

inline void luaR_yield::invoke(lua_State* lua)
{
    lua_yield(lua, get<int>());
}


#endif  // LUARPC_HPP

