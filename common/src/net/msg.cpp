#include "msg.hpp"


bool MessageLayer::handleMessage(void* buffer, size_t size) const
{
    switch (type) {
        case MSG_KEYEXCHANGE: return handleKeyExchange(buffer, size);
        case MSG_PLAYERLOGIN: return handlePlayerLogin(buffer, size);
    }

    return false;
}

