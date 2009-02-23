extern "C" {
    #include <enet/enet.h>
}

#include <memory>
#include <fstream>
#include <algorithm>
#include <foreach.hpp>
#include <core.hpp>
#include "network.hpp"


#define foreach BOOST_FOREACH


// TODO: Once networking is done on the client run valgrind on the server to
// check memory is properly freed. The memory usage is quite complicated here.


////////// Network //////////

Network::Network(lua_State* lua) :
    LuaObject(lua)
{
    registerMethod(lua, &Network::luaHandleTasks, "handle_tasks", true);

    if (enet_initialize() != 0)
        throw NetworkException("failed to initialise enet");
    
    Settings& settings = getSettings();

    // Create address object.
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = settings.gamePort();
    
    // Calculate bandwidth per client.
    int ds = settings.downstream() / settings.clients();
    int us = settings.upstream() / settings.clients();
    
    // Create server host with address and bandwidth settings.
    //if ((_server = enet_host_create(&address, settings.clients(), ds, us)) == 0)
    //    throw NetworkException("failed to create enet host");
    
    // Create network controller.
    try {
        //_controller.reset(new Net::Controller(lua, _server));
    } catch (...) {
        //enet_host_destroy(_server);
        throw;
    }
}

Network::~Network()
{
    //enet_host_destroy(_server);
}

int Network::luaHandleTasks(lua_State* lua)
{
    //_controller->handleTasks();

    return 0;
}
