/// \file login.cpp
/// \brief Handles logging into game.
/// \author Ben Radford 
/// \date 16th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "login.hpp"
#include <CEGUI.h>


using namespace CEGUI;


Login::Login(CEGUI::System& cegui) :
    _prompting(false), _detailsAvailable(false), _cegui(cegui), 
    _loginRoot(0), _usernameBox(0), _passwordBox(0), _hostnameBox(0)
{
    WindowManager& wmgr = WindowManager::getSingleton();

    _loginRoot = wmgr.loadWindowLayout("Login.layout");
    _cegui.setGUISheet(_loginRoot);
    _loginRoot->setVisible(true);

    _usernameBox = static_cast<Editbox*>(wmgr.getWindow("Root/Login/Username"));
    _passwordBox = static_cast<Editbox*>(wmgr.getWindow("Root/Login/Password"));
    _hostnameBox = static_cast<Editbox*>(wmgr.getWindow("Root/Login/Hostname"));

    Window* connectButton = wmgr.getWindow("Root/Login/Connect");
    connectButton->subscribeEvent(PushButton::EventClicked,
        Event::Subscriber(&Login::connectClicked, this));

    _hostname = "localhost";
}

void Login::promptForLoginDetails()
{
    if (_prompting) 
        return;

    _previousRoot = _cegui.getGUISheet();
    _cegui.setGUISheet(_loginRoot);

    _usernameBox->activate();
    _usernameBox->setText("");
    _passwordBox->setText("");
    _hostnameBox->setText(_hostname);

    _detailsAvailable = false;
    _prompting = true;
}

bool Login::loginDetailsAvailable()
{
    return _detailsAvailable;
}

const std::string& Login::getUsername() const
{
    return _username;
}

const std::string& Login::getPassword() const
{
    return _password;
}

const std::string& Login::getHostname() const
{
    return _hostname;
}

bool Login::connectClicked(const CEGUI::EventArgs& args)
{
    assert(_prompting);

    _cegui.setGUISheet(_previousRoot);

    _username = _usernameBox->getText().c_str();
    _password = _passwordBox->getText().c_str();
    _hostname = _hostnameBox->getText().c_str();

    _detailsAvailable = true;
    _prompting = false;

    return true;
}

