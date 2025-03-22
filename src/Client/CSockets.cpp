#include "pch.h"
#include "CSockets.h"
#include "CVector2.h"


CSockets::CSockets() : mSocket(INVALID_SOCKET), mAddress()
{
}

CSockets::~CSockets()
{
	/*if (mSocket != INVALID_SOCKET)
	{
		closesocket(mSocket);
	}*/
}

bool CSockets::Init(unsigned int port)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    mAddress = {};
    mAddress.sin_family = AF_INET;
    mAddress.sin_port = htons(port);
    mAddress.sin_addr.s_addr = INADDR_ANY;

    bind(mSocket, (sockaddr*)&mAddress, sizeof(mAddress));

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

bool CSockets::ReceiveFrom(void* buffer, int bufferSize, sockaddr_in& from)
{
    int fromSize = sizeof(from);
    int received = recvfrom(mSocket, static_cast<char*>(buffer), bufferSize, 0, (sockaddr*)&from, &fromSize);
    return received > 0;
}
