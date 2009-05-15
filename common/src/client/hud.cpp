/// \file hud.cpp
/// \brief Heads up display.
/// \author Ben Radford 
/// \date 14th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "hud.hpp"


using namespace CEGUI;


gfx::HUD::HUD(CEGUI::System& cegui) :
    _cegui(cegui)
{
    WindowManager& wmgr = WindowManager::getSingleton();

#if 0
    Window* root = wmgr.loadWindowLayout("client.layout");
    _cegui.setGUISheet(root);
    fWnd->setVisible(true);
#endif

    Window* myRoot = wmgr.createWindow("DefaultWindow", "root");
    _cegui.setGUISheet(myRoot);
    FrameWindow* fWnd = (FrameWindow*)wmgr.createWindow("TaharezLook/FrameWindow", "testWindow");
    fWnd->setPosition(UVector2(UDim( 0.25f, 0), UDim( 0.25f, 0)));
    fWnd->setSize(UVector2(UDim(0.5f, 0), UDim( 0.5f, 0)));
    fWnd->setText("Hello World!");
    fWnd->setVisible(true);
    myRoot->addChildWindow(fWnd);
}

