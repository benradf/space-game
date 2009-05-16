/// \file hud.cpp
/// \brief Heads up display.
/// \author Ben Radford 
/// \date 14th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "hud.hpp"
#include "network.hpp"

using namespace CEGUI;


////////// ObjectOverlay //////////

gfx::ObjectOverlay::ObjectOverlay(Ogre::Overlay* overlay, const char* name) :
    _container(0), _text(0), _overlay(overlay), _camera(0)
{

    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

    _container = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", name));

    _overlay->add2D(_container);

    char nameText[64];
    snprintf(nameText, sizeof(nameText), "%s_text", name);
    _text = overlayManager.createOverlayElement("TextArea", nameText);

    _container->addChild(_text);

    _text->setDimensions(1.0f, 1.0f);
    _text->setMetricsMode(Ogre::GMM_PIXELS);
    _text->setPosition(0.0f, 0.0f);
    _text->setParameter("font_name", "blue16");
    _text->setParameter("char_height", "16");
    _text->setParameter("horz_align", "center");
}

gfx::ObjectOverlay::~ObjectOverlay()
{
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

    _container->removeChild(_text->getName());
    overlayManager.destroyOverlayElement(_text);

    _overlay->remove2D(_container);
    overlayManager.destroyOverlayElement(_container);
}

void gfx::ObjectOverlay::attachCamera(Camera& camera)
{
    _camera = &camera;
}

struct MinMaxCorners {
    MinMaxCorners(const Ogre::Matrix4& matrix);
    void operator()(const Ogre::Vector3& corner);
    const Ogre::Matrix4& viewMatrix;
    Ogre::Vector3 min, max;
};

MinMaxCorners::MinMaxCorners(const Ogre::Matrix4& matrix) :
    viewMatrix(matrix), min(Ogre::Vector3::ZERO), max(Ogre::Vector3::ZERO)
    
{

}

void MinMaxCorners::operator()(const Ogre::Vector3& corner)
{
    Ogre::Vector3 pos = viewMatrix * corner;
    float x = pos.x / pos.z + 0.5f;
    float y = pos.y / pos.z + 0.5f;

    min.x = std::min(min.x, x);
    min.y = std::min(min.y, y);
    max.x = std::max(max.x, x);
    max.y = std::max(max.y, y);
}

void gfx::ObjectOverlay::update(Ogre::MovableObject* object)
{
    if (!_container->isVisible() || (_camera == 0))
        return;

    MinMaxCorners corners(_camera->getViewMatrix());
    const Ogre::AxisAlignedBox& aabb = object->getWorldBoundingBox(true);
    std::for_each(aabb.getAllCorners(), aabb.getAllCorners() + 8, corners);

    _container->setPosition(corners.min.x, corners.min.y);
    _container->setDimensions(corners.max.x - corners.min.x, 0.1);
}

void gfx::ObjectOverlay::setColour(const Ogre::ColourValue& colour)
{
    _text->setColour(colour);
}

void gfx::ObjectOverlay::setText(const char* text)
{
    _text->setCaption(text);
}

void gfx::ObjectOverlay::setVisible(bool visible)
{
    if (visible) {
        _container->show();
    } else {
        _container->hide();
    }
}


////////// HUD //////////

gfx::HUD::HUD(CEGUI::System& cegui, NetworkInterface& network, LocalController& localController) :
    _state(STATE_NORMAL), _ctrls(localController), _network(network), _cegui(cegui)
{
    WindowManager& wmgr = WindowManager::getSingleton();

    Window* root = wmgr.loadWindowLayout("HUD.layout");
    _cegui.setGUISheet(root);
    root->setVisible(true);
    //wmgr.getWindow("Root/Radar")->setVisible(false);

    _message = static_cast<Editbox*>(wmgr.getWindow("Root/Message"));
    _console = static_cast<Listbox*>(wmgr.getWindow("Root/Console"));
}

gfx::HUD::~HUD()
{

}

void gfx::HUD::update()
{
    updateConsoleText();
}

std::auto_ptr<gfx::ObjectOverlay> gfx::HUD::createObjectOverlay(const char* name)
{
    char overlayName[64];
    snprintf(overlayName, sizeof(overlayName), "%s_%04x", name, rand() % 0x10000);

    return std::auto_ptr<ObjectOverlay>(new ObjectOverlay(_objectOverlay, name));
}

bool gfx::HUD::keyPressed(const OIS::KeyEvent& arg)
{
    if (arg.key == OIS::KC_RETURN) 
        toggleChatMessageEditbox();
}

bool gfx::HUD::keyReleased(const OIS::KeyEvent& arg)
{

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
        line->setSelectionColours(colour(1.00f, 1.00f, 1.00f));
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

