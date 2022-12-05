/// \file login.hpp
/// \brief Handles logging into game.
/// \author Ben Radford 
/// \date 16th May 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef LOGIN_HPP
#define LOGIN_HPP


#include <string>


namespace CEGUI {
    class System;
    class EventArgs;
    class Window;
    class Editbox;
}


class Login {
    public:
        Login(CEGUI::System& cegui);

        void promptForLoginDetails();
        bool loginDetailsAvailable();

        const std::string& getUsername() const;
        const std::string& getPassword() const;
        const std::string& getHostname() const;

        bool connectClicked(const CEGUI::EventArgs& args);

    private:
        bool _prompting;
        bool _detailsAvailable;
        std::string _username;
        std::string _password;
        std::string _hostname;

        CEGUI::System& _cegui;
        CEGUI::Window* _loginRoot;
        CEGUI::Window* _previousRoot;
        CEGUI::Editbox* _usernameBox;
        CEGUI::Editbox* _passwordBox;
        CEGUI::Editbox* _hostnameBox;
};


#endif  // LOGIN_HPP

