#include "pch.h"
#include "ConnectDisplay.h"

ConnectDisplay::ConnectDisplay()
    : window(sf::VideoMode(400, 300), "Connexion au Serveur"), ipInput(""), portInput(""), nameInput("")
{
    if (!font.loadFromFile("../../../res/Client/Arial.ttf"))
    {
        throw std::runtime_error("Impossible de charger la police arial.ttf");
    }

    isTypingIP = false;
    isTypingPort = false;
    isTypingName = false;

    title.setFont(font);
    title.setString("Connectez-vous");
    title.setCharacterSize(24);
    title.setPosition(120, 20);

    ipLabel.setFont(font);
    ipLabel.setString("IP :");
    ipLabel.setCharacterSize(18);
    ipLabel.setPosition(50, 80);

    portLabel.setFont(font);
    portLabel.setString("Port :");
    portLabel.setCharacterSize(18);
    portLabel.setPosition(50, 130);

    nameLabel.setFont(font);
    nameLabel.setString("Nom :");
    nameLabel.setCharacterSize(18);
    nameLabel.setPosition(50, 180);

    connectButton.setFont(font);
    connectButton.setString("Se connecter");
    connectButton.setCharacterSize(18);
	connectButton.setFillColor(sf::Color::Black);
    connectButton.setPosition(148, 248);

    buttonBox.setSize(sf::Vector2f(160, 40));
    buttonBox.setFillColor(sf::Color(100, 200, 100));
    buttonBox.setPosition(120, 240);

    ipText.setFont(font);
    ipText.setCharacterSize(18);
    ipText.setPosition(110, 80);

    portText.setFont(font);
    portText.setCharacterSize(18);
    portText.setPosition(110, 130);

    nameText.setFont(font);
    nameText.setCharacterSize(18);
    nameText.setPosition(110, 180);

    ipBox.setSize(sf::Vector2f(200, 30));
    ipBox.setPosition(105, 75);
    ipBox.setFillColor(sf::Color::Black);
    ipBox.setOutlineColor(sf::Color::White);
    ipBox.setOutlineThickness(2);

    portBox.setSize(sf::Vector2f(200, 30));
    portBox.setPosition(105, 125);
    portBox.setFillColor(sf::Color::Black);
    portBox.setOutlineColor(sf::Color::White);
    portBox.setOutlineThickness(2);

    nameBox.setSize(sf::Vector2f(200, 30));
    nameBox.setPosition(105, 175);
    nameBox.setFillColor(sf::Color::Black);
    nameBox.setOutlineColor(sf::Color::White);
    nameBox.setOutlineThickness(2);

    cursor.setSize(sf::Vector2f(2, 20));
    cursor.setFillColor(sf::Color::Red);
}

void ConnectDisplay::run()
{
    while (window.isOpen())
    {
        handleEvents();
        render();
    }
}

void ConnectDisplay::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::TextEntered)
            handleTextInput(event);
        if (event.type == sf::Event::MouseButtonPressed)
            handleMouseClick(event);
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
            handleTabKey();
    }
}

void ConnectDisplay::render()
{
    window.clear(sf::Color::Black);

    window.draw(title);
    window.draw(ipLabel);
    window.draw(portLabel);
    window.draw(nameLabel);

    window.draw(ipBox);
    window.draw(portBox);
    window.draw(nameBox);

    window.draw(ipText);
    window.draw(portText);
    window.draw(nameText);
    window.draw(buttonBox);
    window.draw(connectButton);

    if (cursorClock.getElapsedTime().asMilliseconds() > 500)
    {
        cursorVisible = !cursorVisible;
        cursorClock.restart();
    }

    if (cursorVisible)
    {
        if (isTypingIP)
            cursor.setPosition(ipText.getPosition().x + ipText.getGlobalBounds().width + 5, ipText.getPosition().y);
        else if (isTypingPort)
            cursor.setPosition(portText.getPosition().x + portText.getGlobalBounds().width + 5, portText.getPosition().y);
        else if (isTypingName)
            cursor.setPosition(nameText.getPosition().x + nameText.getGlobalBounds().width + 5, nameText.getPosition().y);

        window.draw(cursor);
    }

    window.display();
}

void ConnectDisplay::handleTabKey()
{
    if (isTypingIP)
    {
        isTypingIP = false;
        isTypingPort = true;
    }
    else if (isTypingPort)
    {
        isTypingPort = false;
        isTypingName = true;
    }
    else if (isTypingName)
    {
        isTypingName = false;
        isTypingIP = true;
    }
}

void ConnectDisplay::handleTextInput(sf::Event& event)
{
    if (event.text.unicode < 128)
    {
        char entered = static_cast<char>(event.text.unicode);
        if (entered == '')
        {
            if (isTypingIP && !ipInput.empty())
                ipInput.pop_back();
            else if (isTypingPort && !portInput.empty())
                portInput.pop_back();
            else if (isTypingName && !nameInput.empty())
                nameInput.pop_back();
        }
        else if (std::isprint(entered))
        {
            if (isTypingIP)
                ipInput += entered;
            else if (isTypingPort && std::isdigit(entered))
                portInput += entered;
            else if (isTypingName)
                nameInput += entered;
        }

        ipText.setString(ipInput);
        portText.setString(portInput);
        nameText.setString(nameInput);
    }
}

void ConnectDisplay::handleMouseClick(sf::Event& event)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect ipBounds = ipBox.getGlobalBounds();
    sf::FloatRect portBounds = portBox.getGlobalBounds();
    sf::FloatRect nameBounds = nameBox.getGlobalBounds();
    sf::FloatRect buttonBounds = buttonBox.getGlobalBounds();

    if (ipBounds.contains(static_cast<sf::Vector2f>(mousePos)))
    {
        isTypingIP = true;
        isTypingPort = false;
        isTypingName = false;
    }
    else if (portBounds.contains(static_cast<sf::Vector2f>(mousePos)))
    {
        isTypingIP = false;
        isTypingPort = true;
        isTypingName = false;
    }
    else if (nameBounds.contains(static_cast<sf::Vector2f>(mousePos)))
    {
        isTypingIP = false;
        isTypingPort = false;
        isTypingName = true;
    }
    else if (buttonBounds.contains(static_cast<sf::Vector2f>(mousePos)))
    {
        window.close();
    }
}

const char* ConnectDisplay::GetIP()
{
	const char* ip = ipInput.c_str();
	return ip;
}

unsigned int ConnectDisplay::GetPort()
{
	unsigned int port = std::stoi(portInput);
	return port;
}

const char* ConnectDisplay::GetName()
{
	const char* name = nameInput.c_str();
	return name;
}

