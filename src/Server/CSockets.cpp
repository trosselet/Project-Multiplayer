#include "pch.h"
#include "CSockets.h"


CSockets::CSockets() : mSocket(INVALID_SOCKET), mAddress()
{
}

CSockets::~CSockets()
{
	if (mSocket != INVALID_SOCKET)
	{
		closesocket(mSocket);
	}
}

bool CSockets::Init(unsigned int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }

    mAddress.sin_family = AF_INET;
    mAddress.sin_port = htons(port);
    mAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(mSocket, (sockaddr*)&mAddress, sizeof(mAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(mSocket);
        WSACleanup();
        return false;
    }

    return true;
}

bool CSockets::SendTo(const char* ip, unsigned int port, const void* data, int dataSize)
{
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, ip, &dest.sin_addr);

    int sent = sendto(mSocket, static_cast<const char*>(data), dataSize, 0, (sockaddr*)&dest, sizeof(dest));
    return sent == dataSize;
}

bool CSockets::ReceiveFrom(char* buffer, int bufferSize, sockaddr_in& from)
{
    int fromSize = sizeof(from);
    int received = recvfrom(mSocket, buffer, bufferSize, 0, (sockaddr*)&from, &fromSize);
    if (received <= 0)
    {
        int Error_Code = WSAGetLastError();
    }
    return received > 0;
}
