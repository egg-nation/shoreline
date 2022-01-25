#include "server.h"
#include <iostream>
#include <sstream>
#include <WS2tcpip.h>

using std::string;

#define SERVER_PORT 4500
#define MSG_BUF_SIZE 1024

LoadQueue nodes;

Server::Server()
{
    ver = MAKEWORD(2, 2);

    // hint structure
    addressLen = sizeof(SOCKADDR_IN);
    hint.sin_family = AF_INET;
    hint.sin_port = ::htons(SERVER_PORT);
    hint.sin_addr.S_un.S_addr = ADDR_ANY;
    ZeroMemory(host, NI_MAXHOST);

    // initialize winsock
    if (::WSAStartup(ver, &wsData) != 0)
    {
        std::cerr << "Can't Initialize winsock! Error: " << ::WSAGetLastError() << std::endl;

        system("pause");
        exit(1);
    }

    // create server socket
    serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Can't create a socket! Error: " << ::WSAGetLastError() << std::endl;
        ::WSACleanup();

        system("pause");
        exit(1);
    }

    // bind the ip address and port to a socket
    if (::bind(serverSocket, (SOCKADDR*)&hint, sizeof(hint)) != 0)
    {
        std::cerr << "Can't bind socket! Error: " << ::WSAGetLastError() << std::endl;
        ::WSACleanup();

        system("pause");
        exit(1);
    }

    // tell winsock the socket is for listening
    if (::listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Server socket can't listen! Error: " << ::WSAGetLastError() << std::endl;
        ::WSACleanup();

        system("pause");
        exit(1);
    }

    std::cout << "Server ready..." << std::endl;
}

Server::~Server()
{
    ::closesocket(serverSocket);
    ::closesocket(clientSocket);
    ::WSACleanup();

    std::cout << "Socket closed..." << std::endl;
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter);

void Server::WaitForClient()
{
    while (true)
    {
        clientSocket = ::accept(serverSocket, (SOCKADDR*)&client, &addressLen);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Server socket can't accept client! Error: " << ::WSAGetLastError() << std::endl;
            ::WSACleanup();

            system("pause");
            exit(1);
        }

        ::inet_ntop(client.sin_family, &client, host, NI_MAXHOST);
        std::cout << "New client connection. IP address: " << host << ", PORT number: " << ::ntohs(client.sin_port) << std::endl;

        // create a new thread for the client
        hThread = ::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)clientSocket, 0, nullptr);
        if (hThread == NULL)
        {
            std::cerr << "CreateThread() failed! Error: " << ::WSAGetLastError() << std::endl;
            ::WSACleanup();

            system("pause");
            exit(1);
        }
        ::CloseHandle(hThread);
    }
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
    SOCKET clientSocket = (SOCKET)lpParameter;
    char buffer[MSG_BUF_SIZE];
    int bytesReceived = 0;

    // get free node from the queue
    uint16 nodeId = nodes.getFreeNode();

    ZeroMemory(buffer, MSG_BUF_SIZE);

    strcpy_s(buffer, "Type <get> to get an universally unique id or type <exit> to close socket\n");

    if (::send(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR)
    {
        std::cerr << "send() failed! Error: " << ::GetLastError() << std::endl;
        ::closesocket(clientSocket);

        system("pause");
        return 1;
    }

    // accept and echo message back to client 
    while (true)
    {
        ZeroMemory(buffer, MSG_BUF_SIZE);

        // wait for client to send data
        bytesReceived = ::recv(clientSocket, buffer, MSG_BUF_SIZE, 0);

        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "recv() failed! Error: " << ::GetLastError() << std::endl;
            break;
        }

        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        if (strcmp(buffer, "get") == 0)
        {
            // std::cout << "Command received: " << string(buffer, 0, bytesReceived) << " by client connected on thread with nodeID #" << nodeId << std::endl;

            Generator g(nodeId);
            uint64 generatedId = g.generate();
            sprintf_s(buffer, "%llu", generatedId);

            // send response to client
            if (::send(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR)
            {
                // the node is no longer in use
                nodes.addFreeNode(nodeId);

                std::cerr << "send() failed! Error: " << ::GetLastError() << std::endl;
                ::closesocket(clientSocket);

                system("pause");
                return 1;
            }
        }
        else if (strcmp(buffer, "exit") == 0)
        {
            std::cout << "Client requests to close the connection..." << std::endl;
            break;
        }
        else
        {
            strcpy_s(buffer, "Type a valid command\n");

            // send response to client
            if (::send(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR)
            {
                // the node is no longer in use
                nodes.addFreeNode(nodeId);

                std::cerr << "Failed to send message to client!Error code: " << ::GetLastError() << std::endl;
                ::closesocket(clientSocket);

                system("pause");
                return 1;
            }
        }
    }
    
    if (::shutdown(clientSocket, SD_SEND) == SOCKET_ERROR)
    {
        // the node is no longer in use
        nodes.addFreeNode(nodeId);

        std::cerr << "shutdown() failed! Error: " << ::GetLastError() << std::endl;
        ::closesocket(clientSocket);

        system("pause");
        return 1;
    }

    return 0;
}