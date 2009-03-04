#include <core.hpp>
#include "scriptmodule.hpp"
#include "settings.hpp"
#include "network.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH


// NOTE: Epic bug with the following string literal. It was originally named 
// bind. Unfortunately this confused the linker, which directed calls to bind 
// sockets in the statically linked enet library to the address of this string.
// I noticed fairly early on in debugging that the instructions for the supposed
// bind function didn't make much sense, especially after checking glibc for the
// source code of bind. However it wasn't until I checked the executable with 
// the nm tool and noticed that the bind symbol was in the data section rather
// than an unresolved external symbol that it occured to me what must have 
// happened. Sure enough, a quick grep of the source code for bind turned up 
// this string.

// TODO: Implement removal of modules from global module list when they finish.

// TODO: Lock metatable for module call tables.


////////// Instance //////////

Instance::Instance(lua_State* lua, ID id, VirtualMachine& vm)
    : LuaObject(lua), _id(id), _vm(vm)
{

}

Instance::~Instance()
{
    _vm.removeInstance(_id);
}


////////// VirtualMachine //////////

VirtualMachine::VirtualMachine(VMSet& vmSet, Instance::ID baseID)
    : _lua(lua_open()), _instanceToVMLock(_instanceToVM),
      _vmSet(vmSet), _nextID(baseID)
{
    loadLibraries();
    storeFunctions();

    if (luaL_loadfile(_lua, "script/main.lua") != 0)
        throw ModuleLoadException(_lua, "main.lua");

    Lua::protectedCall<ScriptException>(_lua);

    addToSet();
}

VirtualMachine::~VirtualMachine()
{
    lua_close(_lua);
}

Job::RetType VirtualMachine::run()
{
    lua_getglobal(_lua, "main");
    Lua::protectedCall<ScriptException>(_lua);

    swapRPC();

    foreach (luaR_base& cmd, _rpcLocal)
        cmd(_lua);

    _rpcLocal.clear();

    return YIELD;
}

void VirtualMachine::removeInstance(Instance::ID id)
{
    AutoWriteLock<InstanceToVM>(_instanceToVMLock)->erase(id);
}

VirtualMachine* VirtualMachine::localLookup(Instance::ID id)
{
    AutoReadLock<InstanceToVM> instances(_instanceToVMLock);
    InstanceToVM::iterator pos = instances->find(id);

    if (pos != instances->end()) 
        return pos->second;

    return 0;
}

VirtualMachine* VirtualMachine::globalLookup(Instance::ID id)
{
    VirtualMachine* vm = localLookup(id);

    if (vm != 0) 
        return vm;

    foreach (VirtualMachine* search, *VMSet::LockForRead(_vmSet)) {
        if ((vm = search->localLookup(id)) != 0) 
            return vm;
    }

    return 0;
}

void VirtualMachine::addToSet()
{
    VMSet::LockForWrite(_vmSet)->insert(this);
}

void VirtualMachine::loadLibraries()
{
    new(_lua) ScriptLog(_lua);
    lua_setglobal(_lua, "log");
    
    new(_lua) Lua::Util(_lua);
    lua_setglobal(_lua, "util");
    
    luaL_openlibs(_lua);
}

void VirtualMachine::storeFunctions()
{
    lua_pushlightuserdata(_lua, this);
    lua_pushcclosure(_lua, luaNewInstance, 1);
    lua_setglobal(_lua, "__vm_new_instance");

    lua_pushlightuserdata(_lua, this);
    lua_pushcclosure(_lua, luaDelInstance, 1);
    lua_setglobal(_lua, "__vm_del_instance");

    lua_pushlightuserdata(_lua, this);
    lua_pushcclosure(_lua, luaSendMessage, 1);
    lua_setglobal(_lua, "__vm_send_message");
}

Instance::ID VirtualMachine::nextID()
{
    return _nextID++;
}

int VirtualMachine::luaNewInstance(lua_State* lua)
{
    VirtualMachine* vm = 
        reinterpret_cast<VirtualMachine*>(
        lua_touserdata(lua, lua_upvalueindex(1)));

    Instance::ID id = vm->nextID();
    (*AutoWriteLock<InstanceToVM>(vm->_instanceToVMLock))[id] = vm;

    lua_pushnumber(lua, id);

    return 1;
}

int VirtualMachine::luaDelInstance(lua_State* lua)
{
    VirtualMachine* vm = 
        reinterpret_cast<VirtualMachine*>(
        lua_touserdata(lua, lua_upvalueindex(1)));

    Instance::ID id = luaL_checkint(lua, 1);
    AutoWriteLock<InstanceToVM>(vm->_instanceToVMLock)->erase(id);

    return 0;
}

int VirtualMachine::luaSendMessage(lua_State* lua)
{
    VirtualMachine* vm = 
        reinterpret_cast<VirtualMachine*>(
        lua_touserdata(lua, lua_upvalueindex(1)));
    
    Instance::ID id = luaL_checkint(lua, 1);
    VirtualMachine* host = vm->globalLookup(id);

    if (host == 0) {
        lua_pushboolean(lua, false);
        return 1;
    }

    RPCQueue::LockForWrite rpc(host->getRPC());

    rpc->push_back(luaR_getglobal("debug"));
    rpc->push_back(luaR_getfield(-1, "traceback"));
    rpc->push_back(luaR_getglobal(lua_tostring(lua, 2)));

    size_t top = lua_gettop(lua);
    for (int i = 3; i <= top; i++)
        pushLuaValue(rpc, lua, i - top - 1);

    rpc->push_back(luaR_pcall(top - 2, 0, -top));
    rpc->push_back(luaR_pop(2));

    lua_pushboolean(lua, true);

    return 1;
}

int VirtualMachine::luaInitNetwork(lua_State* lua)
{
    //new(lua) Network(lua);
    return 0;
}

void VirtualMachine::pushLuaValue(RPCQueue::LockForWrite& rpc, lua_State* lua, int index)
{
    switch (lua_type(lua, index)) {
        case LUA_TNUMBER:
            rpc->push_back(luaR_pushnumber(lua_tonumber(lua, index)));
            break;
        case LUA_TBOOLEAN:
            rpc->push_back(luaR_pushboolean(lua_toboolean(lua, index)));
            break;
        case LUA_TSTRING:
            rpc->push_back(luaR_pushstring(lua_tostring(lua, index)));
            break;
        case LUA_TTABLE:
            lua_pushstring(lua, "__is_instance");
            lua_rawget(lua, index - 1);
            if (lua_isnil(lua, -1)) {
                lua_pushnil(lua);
                rpc->push_back(luaR_newtable());
                while (lua_next(lua, index - 2) != 0) {
                    pushLuaValue(rpc, lua, -2);
                    pushLuaValue(rpc, lua, -1);
                    rpc->push_back(luaR_rawset(-3));
                    lua_pop(lua, 1);
                }
            } else {
                rpc->push_back(luaR_getglobal("__create_stub"));
                lua_pushstring(lua, "__id");
                lua_rawget(lua, index - 2);
                rpc->push_back(luaR_pushnumber(lua_tonumber(lua, -1)));
                rpc->push_back(luaR_pushnumber(luaL_checkint(lua, 1)));
                rpc->push_back(luaR_pcall(2, 1, 0));
                lua_pop(lua, 1);
            }
            lua_pop(lua, 1);
            break;
        case LUA_TNIL:
        case LUA_TTHREAD:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
            rpc->push_back(luaR_pushnil());
            break;
    }
}



void protectedLuaCall(lua_State* lua, int args, int rets)
{
    if (lua_pcall(lua, args, rets, 0) != 0)
        throw ScriptCallException(lua);
}


////////// ScriptCallException //////////

ScriptCallException::ScriptCallException(lua_State* lua)
{
    std::string description;
    if (lua_isstring(lua, -1))
        description += lua_tostring(lua, -1);
    description += "\nscript call failed in module `";
    lua_getglobal(lua, "module_name");
    description += (lua_isstring(lua, -1) ? lua_tostring(lua, -1) : "???");
    description += "'";
    lua_pop(lua, 1);
    annotate(description);
}


////////// ModuleLoadException //////////

ModuleLoadException::ModuleLoadException(lua_State* lua, const std::string& module)
{
    std::string description;
    if (lua_isstring(lua, -1))
        description += lua_tostring(lua, -1);
    description += "\nerror loading module `";
    description += module;
    description += "'";
    annotate(description);    
}


////////// UnknownModuleException //////////

UnknownModuleException::UnknownModuleException(const char* module, const char* method)
{
    std::string description("attempt to call `");
    description += method;
    description += "' in unknown module `";
    description += module;
    description += "'";
    annotate(description);
}


////////// ScriptLog //////////

ScriptLog::ScriptLog(lua_State* lua)
    : LuaObject(lua)
{
    registerMethod(lua, &ScriptLog::debug, "debug", true);
    registerMethod(lua, &ScriptLog::info, "info", true);
    registerMethod(lua, &ScriptLog::warn, "warn", true);
    registerMethod(lua, &ScriptLog::error, "error", true);
}

ScriptLog::~ScriptLog()
{
    
}

int ScriptLog::debug(lua_State* lua)
{
    Log::log->debug(luaL_checkstring(lua, 1));
    
    return 0;
}

int ScriptLog::info(lua_State* lua)
{
    Log::log->info(luaL_checkstring(lua, 1));
    
    return 0;
}

int ScriptLog::warn(lua_State* lua)
{
    Log::log->warn(luaL_checkstring(lua, 1));
    
    return 0;
}

int ScriptLog::error(lua_State* lua)
{
    Log::log->error(luaL_checkstring(lua, 1));
    
    return 0;
}


////////// ScriptLoader //////////

ScriptLoader::ScriptLoader(JobPool& pool, const ModuleVector& preloaded)
{
    foreach (ScriptModule* module, preloaded)
        _modules.insert(std::make_pair(module->name(), module));
    
    loadModules(pool);
    linkModules();
}

ScriptLoader::~ScriptLoader()
{
    
}

const ModuleMap& ScriptLoader::modules()
{
    return _modules;
}

void ScriptLoader::loadModules(JobPool& pool)
{
    std::ifstream moduleList("modules.list");
    
    if (!moduleList.is_open())
        throw FileException("unable to open modules.list");
    
    std::string moduleName;
    
    while (std::getline(moduleList, moduleName)) {
        std::auto_ptr<Job> module(new ScriptModule(moduleName));
        ScriptModule* refined = static_cast<ScriptModule*>(module.get());
        _modules.insert(std::make_pair(moduleName, refined));
        pool.add(module);
    }
    
    moduleList.close();
}

void ScriptLoader::linkModules()
{
    foreach (ModulePair& module, _modules) {
        lua_State* lua = module.second->lua();
        new(lua) ModuleCaller(lua, _modules);
    }
}


////////// ModuleCaller //////////

ModuleCaller::ModuleCaller(lua_State* lua, const ModuleMap& modules)
    : LuaObject(lua), _modules(modules)
{
    registerMethod(lua, &ModuleCaller::call, "call", true);
    
    const char* bindCall = 
        "local f, m = ...;"
        "return function (t, k)"
        "    return function (...)"
        "        f(m, k, ...)"
        "    end;"
        "end";
    
    luaL_loadstring(lua, bindCall);
    lua_getfield(lua, -2, "call");
    
    foreach (const ModulePair& module, _modules) {
        const char* name = module.second->name().c_str();
        lua_newtable(lua);
        lua_newtable(lua);
        lua_pushvalue(lua, -4);
        lua_pushvalue(lua, -4);
        lua_pushstring(lua, name);
        lua_call(lua, 2, 1);
        lua_setfield(lua, -2, "__index");
        lua_setmetatable(lua, -2);
        lua_setglobal(lua, name);
    }
    
    lua_pop(lua, 3);
}

ModuleCaller::~ModuleCaller()
{
    
}

// NOTE: It is possible for the following function to leak memory if the queue->push
// methods throw an exception. This occurs when the queue needs to allocate more
// memory for the push but its call to new generates a std::bad_alloc exception.
// The code is pretty without slapping shared_ptrs around everything though and
// if std::bad_alloc gets thrown we will kill the program anyway. Still, think
// about implementing an exception safe version when refactoring the module call
// framework to a generalised remote lua api framework.

// function call(module, function, ...)
int ModuleCaller::call(lua_State* lua)
{
    const char* module = luaL_checkstring(lua, 1);
    const char* method = luaL_checkstring(lua, 2);
    
    ModuleMap::const_iterator iter = _modules.find(module);
    
    if (iter == _modules.end())
        throw UnknownModuleException(module, method);
    
    AutoWriteLock<CallQueue> queue(iter->second->callQueue());
    
    queue->push(new CallMethod(method));
    
    for (int i = 3; i <= lua_gettop(lua); i++) {
        switch (lua_type(lua, i)) {
            case LUA_TBOOLEAN:
                queue->push(new ParamBoolean(lua_toboolean(lua, i))); 
                break;
            case LUA_TNUMBER:
                queue->push(new ParamNumber(lua_tonumber(lua, i)));
                break;
            case LUA_TSTRING:
                queue->push(new ParamString(lua_tostring(lua, i)));
                break;
            case LUA_TNIL:
            default:
                queue->push(new ParamNil);
                break;
        }
    }
    
    queue->push(new CallInvoke(lua_gettop(lua) - 2));
    
    return 0;
}


////////// CallMethod //////////

void CallMethod::process(lua_State* lua)
{
    lua_getglobal(lua, _method.c_str());
}


////////// ParamNumber //////////

void ParamNumber::process(lua_State* lua)
{
    lua_pushnumber(lua, _number);
}


////////// ParamString //////////

void ParamString::process(lua_State* lua)
{
    lua_pushstring(lua, _string.c_str());
}


////////// ParamBoolean //////////

void ParamBoolean::process(lua_State* lua)
{
    lua_pushboolean(lua, _boolean);
}


////////// ParamNil //////////

void ParamNil::process(lua_State* lua)
{
    lua_pushnil(lua);
}


////////// CallInvoke //////////

void CallInvoke::process(lua_State* lua)
{
    Lua::protectedCall<ScriptException>(lua, _paramCount, 0);
}


////////// RunScript //////////

ScriptModule::ScriptModule(const std::string& name)
    : _lua(lua_open()), _name(name), _lock(_callQueue), _suspended(false)
{
    loadLibraries();
    
    std::string filename("script/");
    filename += name + "/" + name + ".lua";
    
    if (luaL_loadfile(_lua, filename.c_str()) != 0)
        throw ModuleLoadException(_lua, name);
    
    lua_pushstring(_lua, name.c_str());
    lua_setglobal(_lua, "module_name");
    
    Lua::protectedCall<ScriptException>(_lua, 0, 0);
}

// NOTE: No locking used while emptying queue. If the destructor has been called
// all bets are off on accessing the object anyway.
ScriptModule::~ScriptModule()
{
    lua_close(_lua);
    
    while (!_callQueue.empty()) {
        std::auto_ptr<Call> temp(_callQueue.front());
        _callQueue.pop();
    }
}

Job::RetType ScriptModule::run()
{
    try {
        if (!suspended()) {
            scriptMain();
            handleMessages();
        }
    } catch (Exception& e) {
        annotateWithModule(e);
        suspendModule(e);
    } catch (std::exception& e) {
        suspendModule(e);
    }
    
    return YIELD;
}

Lock<CallQueue>& ScriptModule::callQueue()
{
    return _lock;
}

void ScriptModule::scriptMain()
{
    lua_getglobal(_lua, "main");
    if (lua_isfunction(_lua, -1)) {
        Lua::protectedCall<ScriptException>(_lua, 0, 0);
    } else {
        lua_pop(_lua, 1);
    }
}

void ScriptModule::handleMessages()
{
    std::auto_ptr<Call> next;
    
    while ((next = nextCall()).get() != 0)
        next->process(_lua);
}

void ScriptModule::annotateWithModule(Exception& e)
{
    lua_getglobal(_lua, "module_name");
    std::string description("failure in lua script module `");
    description += (lua_isstring(_lua, -1) ? lua_tostring(_lua, -1) : "???");
    description += "'";
    lua_pop(_lua, 1);
    e.annotate(description);
}

void ScriptModule::loadLibraries()
{
    new(lua()) ScriptLog(lua());
    lua_setglobal(lua(), "log");
    
    new(lua()) Lua::Util(lua());
    lua_setglobal(lua(), "util");
    
    luaL_openlibs(lua());
}

std::auto_ptr<Call> ScriptModule::nextCall()
{
    AutoWriteLock<CallQueue> queue(callQueue());
    
    if (queue->empty())
        return std::auto_ptr<Call>();
    
    std::auto_ptr<Call> next(queue->front());
    queue->pop();
    
    return next;
}

void ScriptModule::suspendModule(std::exception& e)
{
    Log::log->error(e.what());
    _suspended = true;
}
