/// \file login.cpp
/// \brief Handles logging into game.
/// \author Ben Radford 
/// \date 16th May 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#include "login.hpp"
#include <CEGUI/CEGUI.h>
#include <unistd.h>


using namespace CEGUI;


extern std::string serverHostname;


Login::Login(CEGUI::System& cegui) :
    _prompting(false), _detailsAvailable(false), _cegui(cegui), 
    _loginRoot(0), _usernameBox(0), _passwordBox(0), _hostnameBox(0)
{
    WindowManager& wmgr = WindowManager::getSingleton();

    _loginRoot = wmgr.loadLayoutFromFile("Login.layout");
    _cegui.getDefaultGUIContext().setRootWindow(_loginRoot);
    _loginRoot->setVisible(true);

    _usernameBox = static_cast<Editbox*>(_loginRoot->getChildRecursive("Root/Login/Username"));
    _passwordBox = static_cast<Editbox*>(_loginRoot->getChildRecursive("Root/Login/Password"));
    _hostnameBox = static_cast<Editbox*>(_loginRoot->getChildRecursive("Root/Login/Hostname"));

    Window* connectButton = _loginRoot->getChildRecursive("Root/Login/Connect");
    connectButton->subscribeEvent(PushButton::EventClicked,
        Event::Subscriber(&Login::connectClicked, this));

    _hostname = serverHostname;
}

void Login::promptForLoginDetails()
{
    // 2022-08-30: autologin hack
    _username = "pilot-" + std::to_string(getpid());
    _password = "password";
    _detailsAvailable = true;
    _prompting = false;
    return;
    // 2022-08-30: autologin hack

    if (_prompting) 
        return;

    _previousRoot = _cegui.getDefaultGUIContext().getRootWindow();
    _cegui.getDefaultGUIContext().setRootWindow(_loginRoot);

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

    _cegui.getDefaultGUIContext().setRootWindow(_previousRoot);

    _username = _usernameBox->getText().c_str();
    _password = _passwordBox->getText().c_str();
    _hostname = _hostnameBox->getText().c_str();

    _detailsAvailable = true;
    _prompting = false;

    return true;
}

