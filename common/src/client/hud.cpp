/// \file hud.cpp
/// \brief Heads up display.
/// \author Ben Radford 
/// \date 14th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "hud.hpp"
#include "network.hpp"
#include "graphics.hpp"

using namespace CEGUI;


////////// HUD //////////

gfx::HUD::HUD(CEGUI::System& cegui, NetworkInterface& network, LocalController& localController) :
    _state(STATE_NORMAL), _ctrls(localController), _network(network), _cegui(cegui)
{
    WindowManager& wmgr = WindowManager::getSingleton();

    _hudRoot = wmgr.loadLayoutFromFile("HUD.layout");
    _cegui.getDefaultGUIContext().setRootWindow(_hudRoot);
    _hudRoot->setVisible(true);

    _message = static_cast<Editbox*>(_hudRoot->getChild("Root/Message"));
    _console = static_cast<Listbox*>(_hudRoot->getChild("Root/Console"));
    _radar = _hudRoot->getChild("Root/Radar");

    _console->setVisible(true);
    _radar->setVisible(true);
}

gfx::HUD::~HUD()
{
    // unset gui sheet
}

void gfx::HUD::activate()
{
    _cegui.getDefaultGUIContext().setRootWindow(_hudRoot);
}

void gfx::HUD::update()
{
    updateConsoleText();
}

bool gfx::HUD::keyPressed(const OIS::KeyEvent& arg)
{
    if (arg.key == OIS::KC_RETURN) 
        toggleChatMessageEditbox();

    return true;
}

bool gfx::HUD::keyReleased(const OIS::KeyEvent& arg)
{
    return true;
}

void gfx::HUD::toggleChatMessageEditbox()
{
    if (_state == STATE_NORMAL) {
        _message->setVisible(true);
        _ctrls.setEnabled(false);
        _state = STATE_TYPEMSG;
        _message->activate();

    } else if (_state == STATE_TYPEMSG) {
        _message->setVisible(false);
        _ctrls.setEnabled(true);
        _state = STATE_NORMAL;
        sendChatMessage();
    }
}

ChatSystem& gfx::HUD::getChatSystem()
{
    return _network.getServer();
}

bool gfx::HUD::haveChatSystem() const
{
    return _network.hasServer();
}

void gfx::HUD::updateConsoleText()
{
    if (!haveChatSystem()) 
        return;

    ChatSystem& chatSystem = getChatSystem();

    while (chatSystem.haveConsoleText()) {
        ListboxTextItem* line = new ListboxTextItem(chatSystem.getConsoleText());
        line->setSelectionColours(CEGUI::Colour(1.00f, 1.00f, 1.00f));
        _console->addItem(static_cast<ListboxItem*>(line));
        chatSystem.moveToNextConsoleText();
    }

    while (_console->getItemCount() > ChatSystem::MESSAGE_HISTORY)
        _console->removeItem(_console->getListboxItemFromIndex(0));

    Scrollbar* scrollV = _console->getVertScrollbar();
    scrollV->setScrollPosition(scrollV->getDocumentSize());
    scrollV->setVisible(false);

    Scrollbar* scrollH = _console->getHorzScrollbar();
    scrollH->setScrollPosition(0);
    scrollH->setVisible(false);
}

void gfx::HUD::sendChatMessage()
{
    if (haveChatSystem()) 
        getChatSystem().sendMsgPubChat(_message->getText().c_str());

    _message->setText("");
}

