#ifndef SCRIPTMODULE_HPP
#define SCRIPTMODULE_HPP


extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include "concurrency.hpp"
#include "autolock.hpp"
#include "luarpc.hpp"
#include <core.hpp>
#include <script.hpp>
#include <string>
#include <assert.h>
#include <tr1/unordered_map>
#include <boost/shared_ptr.hpp>
#include <tr1/unordered_set>



// script core creates other luavms to run modules such as: net, battle, trade.
// the core can be asked for a handle to the other vms by the name of the module
// that they are running.


// TODO: Replace call objects with "remote lua api" objects.

class VirtualMachine;

class Instance : public LuaObject {
    public:
        typedef uint32_t ID;

        Instance(lua_State* lua, ID id, VirtualMachine& vm);

    protected:
        virtual ~Instance();

    private:
        ID _id;

        class VirtualMachine& _vm;
};


typedef Lockable<luaR_base>::Vector RPCQueue;
typedef Lockable<VirtualMachine*>::UnorderedSet VMSet;
typedef std::tr1::unordered_map<Instance::ID, VirtualMachine*> InstanceToVM;


class VirtualMachine : public Job {
    public:
        VirtualMachine(VMSet& virtualMachines, Instance::ID baseID);
        virtual ~VirtualMachine();

        virtual RetType run();

        virtual void removeInstance(Instance::ID id);
        virtual VirtualMachine* localLookup(Instance::ID id);
        virtual VirtualMachine* globalLookup(Instance::ID id);

        RPCQueue& getRPC();
        void swapRPC();

    private:
        void addToSet();
        void loadLibraries();
        void storeFunctions();
        Instance::ID nextID();

        static int luaNewInstance(lua_State* lua);
        static int luaDelInstance(lua_State* lua);
        static int luaSendMessage(lua_State* lua);
        static int luaInitNetwork(lua_State* lua);

        static void pushLuaValue(RPCQueue::LockForWrite& rpc,
                                 lua_State* lua, int index);

        lua_State* _lua;
        Instance::ID _nextID;

        VMSet& _vmSet;
        RPCQueue _rpc;
        std::vector<luaR_base> _rpcLocal;

        InstanceToVM _instanceToVM;
        Lock<InstanceToVM> _instanceToVMLock;
};



typedef std::queue<class Call*> CallQueue;
typedef std::pair<const std::string, class ScriptModule*> ModulePair;
typedef std::tr1::unordered_map<std::string, class ScriptModule*> ModuleMap;
typedef std::vector<ScriptModule*> ModuleVector;


void protectedLuaCall(lua_State* lua, int args, int rets);


class ScriptCallException : public ScriptException {
    public:
        ScriptCallException(lua_State* lua);
};


class ModuleLoadException : public ScriptException {
    public:
        ModuleLoadException(lua_State* lua, const std::string& module);
};


class UnknownModuleException : public ScriptException {
    public:
        UnknownModuleException(const char* module, const char* method);
};


class ScriptLog : public LuaObject {
    public:
        ScriptLog(lua_State* lua);
        
    protected:
        virtual ~ScriptLog();
        
    private:
        int debug(lua_State* lua);
        int info(lua_State* lua);
        int warn(lua_State* lua);
        int error(lua_State* lua);
};


class ScriptLoader {
    public:
        ScriptLoader(JobPool& pool, const ModuleVector& preloaded);
        ~ScriptLoader();
        
        const ModuleMap& modules();
        
    private:
        void loadModules(JobPool& pool);
        void linkModules();
        
        ModuleMap _modules;
};


class ModuleCaller : public LuaObject {
    public:
        ModuleCaller(lua_State* lua, const ModuleMap& modules);
        
    protected:
        virtual ~ModuleCaller();
        
    private:
        int call(lua_State* lua);
        
        const ModuleMap& _modules;
};


class Call {
    public:
        virtual ~Call();
        virtual void process(lua_State* lua) = 0;
        
    private:
        
};


class CallMethod : public Call {
    public:
        virtual ~CallMethod();
        CallMethod(const std::string& method);
        virtual void process(lua_State* lua);
        
    private:
        std::string _method;
};


class CallParam : public Call {
    public:
        virtual ~CallParam();
};


class ParamNumber : public CallParam {
    public:
        virtual ~ParamNumber();
        ParamNumber(lua_Number number);
        virtual void process(lua_State* lua);
        
    private:
        lua_Number _number;
};


class ParamString : public CallParam {
    public:
        virtual ~ParamString();
        ParamString(const std::string& string);
        virtual void process(lua_State* lua);
        
    private:
        std::string _string;
};


class ParamBoolean : public CallParam {
    public:
        virtual ~ParamBoolean();
        ParamBoolean(bool boolean);
        virtual void process(lua_State* lua);
        
    private:
        bool _boolean;
};


class ParamNil : public CallParam {
    public:
        virtual ~ParamNil();
        virtual void process(lua_State* lua);
};


class CallInvoke : public Call {
    public:
        virtual ~CallInvoke();
        CallInvoke(int paramCount);
        virtual void process(lua_State* lua);
    
    private:
        int _paramCount;
};


class ScriptModule : public Job {
    public:
        ScriptModule(const std::string& name);
        virtual ~ScriptModule();
        
        const std::string& name() const;
        lua_State* lua();
        
        virtual RetType run();
        Lock<CallQueue>& callQueue();
        
    private:
        void scriptMain();
        void handleMessages();
        void annotateWithModule(Exception& e);
        void loadLibraries();
        
        lua_State* _lua;
        std::string _name;
        
        std::auto_ptr<Call> nextCall();
        CallQueue _callQueue;
        Lock<CallQueue> _lock;
        
        void suspendModule(std::exception& e);
        bool suspended() const;
        bool _suspended;
};


////////// VirtualMachine //////////

inline RPCQueue& VirtualMachine::getRPC()
{
    return _rpc;
}

inline void VirtualMachine::swapRPC()
{
    RPCQueue::LockForWrite(_rpc)->swap(_rpcLocal);
}


////////// Call //////////

inline Call::~Call()
{
    
}


////////// CallMethod //////////

inline CallMethod::~CallMethod()
{
    
}

inline CallMethod::CallMethod(const std::string& method)
    : _method(method)
{
    
}


////////// CallParam //////////

inline CallParam::~CallParam()
{
    
}


////////// ParamNumber //////////

inline ParamNumber::~ParamNumber()
{
    
}

inline ParamNumber::ParamNumber(lua_Number number)
    : _number(number)
{
    
}


////////// ParamString //////////

inline ParamString::~ParamString()
{
    
}

inline ParamString::ParamString(const std::string& string)
    : _string(string)
{
    
}


////////// ParamBoolean //////////

inline ParamBoolean::~ParamBoolean()
{
    
}

inline ParamBoolean::ParamBoolean(bool boolean)
    : _boolean(boolean)
{
    
}


////////// ParamNil //////////

inline ParamNil::~ParamNil()
{
    
}


////////// CallInvoke //////////

inline CallInvoke::~CallInvoke()
{
    
}

inline CallInvoke::CallInvoke(int paramCount)
    : _paramCount(paramCount)
{
    
}


////////// ScriptModule //////////

inline const std::string& ScriptModule::name() const
{
    return _name;
}

inline lua_State* ScriptModule::lua()
{
    return _lua;
}

inline bool ScriptModule::suspended() const
{
    return _suspended;
}


#endif  // SCRIPTMODULE_HPP
