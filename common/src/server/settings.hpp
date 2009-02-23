#ifndef SETTINGS_HPP
#define SETTINGS_HPP


class Settings {
    public:
        Settings(int argc, char* argv[]);
        
        int threadMax() const;
        int gamePort() const;
        int clients() const;
        int downstream() const;
        int upstream() const;
        const std::string& directory() const;
        
    private:
        int _threadMax;
        int _gamePort;
        int _clients;
        int _downstream;
        int _upstream;
        std::string _directory;
};


void setSettings(Settings& s);
Settings& getSettings();


#endif  // SETTINGS_HPP
