#include "pch.h"

#include "main.h"
#include "Client.h"
#include "ConnectDisplay.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    Client* client = Client::Get();
	bool isConnected = false;


    /*const char* serverIp = "127.0.0.1";
    unsigned int port = 54000;
	const char* playerName = "Player";*/

	
	ConnectDisplay connectWindow;

	connectWindow.run();

	if (client->Connect(connectWindow.GetIP(), connectWindow.GetPort(), connectWindow.GetName()))
	{
		isConnected = true;

	}
	else
	{
		std::cerr << "Impossible de se connecter au serveur." << std::endl;
		return 1;
	}

	if (isConnected)
	{
		client->Run();
	}
    return 0;
}