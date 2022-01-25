#ifndef SERVER_H
#define SERVER_H

#include <Winsock2.h>
#include <windows.h>
#include "generator.h"
#include "load_queue.h"

#pragma comment (lib, "ws2_32.lib")

class Server
{
private:
    WORD ver;
    WSADATA wsData;
    SOCKET serverSocket;
    SOCKET clientSocket;
    HANDLE hThread;
    SOCKADDR_IN hint;
    SOCKADDR_IN client;
    int addressLen;
    char host[NI_MAXHOST];
public:
    Server();
    ~Server();
    void WaitForClient();
};

#endif