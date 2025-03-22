#include "pch.h"
#include "Network.h"

struct Data
{
    SOCKET socket;
    sockaddr_in addr;
    int x;
    int y;
};

static DWORD WINAPI threadFunc(void* lPtr)
{
    Data* data = static_cast<Data*>(lPtr);

	// Envoi de données
    char message[8];
    memcpy(message, &data->x, 4);
    memcpy(message + 4, &data->y, 4);

    int ret = sendto(data->socket, message, 15, 0, reinterpret_cast<const sockaddr*>(&data->addr), sizeof(data->addr));
    if (ret <= 0)
    {
        std::cout << "Erreur envoi de données : " << WSAGetLastError() << ". Fermeture du programme.";
        return 2;
    }

    return 1;
}

int Connexion(const char* IP, unsigned int portDst)
{
    // Initialisation de Winsock
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        std::cerr << "Erreur initialisation Winsock : " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Configuration de l'adresse du serveur
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portDst);
    if (inet_pton(AF_INET, "IP", &addr.sin_addr) <= 0)
    {
        std::cerr << "Adresse IP invalide." << std::endl;
        WSACleanup();
        return 1;
    }

    // Création du socket
    SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET)
    {
        std::cerr << "Erreur creation socket : " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Connexion au serveur
    if (connect(_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "Erreur connection : " << WSAGetLastError() << std::endl;
        closesocket(_socket);
        WSACleanup();
        return 1;
    }
    std::cout << "Socket connecte !" << std::endl;

    // Nettoyage
    closesocket(_socket);
    WSACleanup();
}