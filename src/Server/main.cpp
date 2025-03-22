#include "pch.h"
#include "main.h"

#include "Server.h"

int main()
{
    const unsigned int port = 54000;

    std::cout << "Demarrage du serveur sur le port " << port << "..." << std::endl;

    Server server;

    server.Start(port);

    std::cout << "Serveur en cours d'execution. Appuyez sur Entree pour arrêter le serveur." << std::endl;

    std::cin.get();

    return 0;
}