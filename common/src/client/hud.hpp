/// \file hud.hpp
/// \brief Heads up display.
/// \author Ben Radford 
/// \date 14th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef HUD_HPP
#define HUD_HPP


#include "chat.hpp"
#include <OIS/OIS.h>
#include <CEGUI.h>
#include <Ogre.h>
#include <memory>


class LocalController;
class NetworkInterface;


namespace gfx {


class ObjectOverlay;


class HUD : public OIS::KeyListener {
    public:
        HUD(CEGUI::System& cegui, NetworkInterface& network, LocalController& localController);
        virtual ~HUD();

        void update();

    private:
        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

        void toggleChatMessageEditbox();

        bool haveChatSystem() const;
        ChatSystem& getChatSystem();

        void updateConsoleText();
        void sendChatMessage();

        enum State {
            STATE_NORMAL,
            STATE_TYPEMSG,
        };

        State _state;
        LocalController& _ctrls;
        NetworkInterface& _network;

        CEGUI::System& _cegui;
        CEGUI::Editbox* _message;
        CEGUI::Listbox* _console;
};


}  // namespace gfx


#endif  // HUD_HPP

