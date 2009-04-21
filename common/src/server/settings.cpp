#include <assert.h>
#include <argtable2.h>
#include <core/core.hpp>
#include "settings.hpp"


Settings* settings = 0;

void setSettings(Settings& s)
{
    settings = &s;
}

Settings& getSettings()
{
    assert(settings != 0);
    return *settings;
}


////////// Settings //////////

Settings::Settings(int argc, char* argv[])
{
    arg_int* argThreadMax = arg_int0("t", "thread-max", "NUM", "use up to NUM worker threads");
    arg_int* argGamePort = arg_int0("p", "game-port", "PORT", "game clients connect on PORT");
    arg_int* argClients = arg_int0("c", "clients", "NUM", "allow NUM clients to connect");
    arg_int* argDownstream = arg_int0("d", "downstream", "BYTES", "incoming bandwidth in BYTES per second");
    arg_int* argUpstream = arg_int0("u", "upstream", "BYTES", "outgoing bandwidth in BYTES per second");
    arg_str* argDirectory = arg_str0("w", "working-dir", "DIR", "make DIR the working directory");
    
    void* argtable[] = {argThreadMax, argGamePort, argClients, argUpstream, 
                        argDownstream, argDirectory, arg_end(20)};
    
    if (arg_nullcheck(argtable) != 0)
        throw InputException("failed to read arguments");
    
    if (arg_parse(argc, argv, argtable) > 0)
        throw InputException("error while parsing arguments");
    
    _threadMax = (argThreadMax->count > 0 ? argThreadMax->ival[0] : 2);
    _gamePort = (argGamePort->count > 0 ? argGamePort->ival[0] : 18572);
    _clients = (argClients->count > 0 ? argClients->ival[0] : 10);
    _downstream = (argDownstream->count > 0 ? argDownstream->ival[0] : 2048);
    _upstream = (argUpstream->count > 0 ? argUpstream->ival[0] : 2048);
    _directory = (argDirectory->count > 0 ? argDirectory->sval[0] : ".");
    
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
}

int Settings::threadMax() const
{
    return _threadMax;
}

int Settings::gamePort() const
{
    return _gamePort;
}

int Settings::clients() const
{
    return _clients;
}

int Settings::downstream() const
{
    return _downstream;
}

int Settings::upstream() const
{
    return _upstream;
}

const std::string& Settings::directory() const
{
    return _directory;
}
