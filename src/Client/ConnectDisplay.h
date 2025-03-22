#ifndef CONNECTDISPLAY_H
#define CONNECTDISPLAY_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <string>
#include <functional>

class ConnectDisplay
{
public:
    ConnectDisplay();
    void run(); // Lance la fenêtre

    std::function<void(const std::string&, int, const std::string&)> onConnect;
    
    const char* GetIP();
    unsigned int GetPort();
    const char* GetName();
private:
    sf::RenderWindow window;
    sf::Font font;

    sf::Text title;
    sf::Text ipLabel;
    sf::Text portLabel;
    sf::Text nameLabel;
    sf::Text connectButton;

    sf::RectangleShape buttonBox;
    sf::RectangleShape ipBox;   // Ajout de la boîte IP
    sf::RectangleShape portBox; // Ajout de la boîte Port
    sf::RectangleShape nameBox; // Ajout de la boîte Nom

    sf::RectangleShape cursor; // Curseur clignotant
    sf::Clock cursorClock; // Chronomètre pour le clignotement
    bool cursorVisible; // État du curseur

    std::string ipInput;
    std::string portInput;
    std::string nameInput;

    sf::Text ipText;
    sf::Text portText;
    sf::Text nameText;

    bool isTypingIP;
    bool isTypingPort;
    bool isTypingName;

    void handleEvents();
    void render();
    void handleTextInput(sf::Event& event);
    void handleMouseClick(sf::Event& event);
    void handleTabKey();
};

#endif
