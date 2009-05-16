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


class Camera;


class ObjectOverlay {
    public:
        ObjectOverlay(Ogre::Overlay* overlay, const char* name);
        ~ObjectOverlay();

        void attachCamera(Camera& camera);
        void update(Ogre::MovableObject* object);

        void setColour(const Ogre::ColourValue& colour);
        void setText(const char* text);
        void setVisible(bool visible);

    private:
        Ogre::OverlayContainer* _container;
        Ogre::OverlayElement* _text;
        Ogre::Overlay* _overlay;
        Camera* _camera;
};


class HUD : public OIS::KeyListener {
    public:
        HUD(CEGUI::System& cegui, NetworkInterface& network, LocalController& localController);
        virtual ~HUD();

        void update();

        std::auto_ptr<ObjectOverlay> createObjectOverlay(const char* name);

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

        Ogre::Overlay* _objectOverlay;
};


}  // namespace gfx


#endif  // HUD_HPP

