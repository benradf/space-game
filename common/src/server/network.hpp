#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <memory>
#include <tr1/unordered_map>
#include "concurrency.hpp"
#include "settings.hpp"
#include <script.hpp>
#include <net.hpp>
#include "scriptmodule.hpp"


class Network : public LuaObject {
    public:
        Network(lua_State* lua);

    protected:
        virtual ~Network();
        
    private:
        int luaHandleTasks(lua_State* lua);

        //ENetHost* _server;
        //std::auto_ptr<Net::Controller> _controller;
};



#endif  // NETWORK_HPP
