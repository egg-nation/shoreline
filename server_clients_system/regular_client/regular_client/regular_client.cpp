#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MSG_BUF_SIZE 1024

int main()
{
	std::string ipAddress = "127.0.0.1";			// server IP address
	int port = 4500;								// listening port # on the server

	// initialize winSock
	WORD ver = MAKEWORD(2, 2);
	WSAData wsData;

	if (WSAStartup(ver, &wsData) != 0)
	{
		std::cerr << "Can't Initialize winsock! Error: " << ::WSAGetLastError() << std::endl;
		return 0;
	}

	// create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	// hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server! Error: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}

	// send and receive data
	char buffer[MSG_BUF_SIZE];
	ZeroMemory(buffer, MSG_BUF_SIZE);

	// info about the general commands from server
	int bytesReceived = recv(sock, buffer, MSG_BUF_SIZE, 0);
	if (bytesReceived > 0)
	{
		std::cout << "SERVER> " << std::string(buffer, 0, bytesReceived) << std::endl;
	}

	std::string userInput;
	do
	{
		// user input follows
		std::cout << "> ";
		getline(std::cin, userInput);
		userInput = "get";
		if (userInput.size() > 0)
		{
			// send command to server
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				ZeroMemory(buffer, MSG_BUF_SIZE);

				// get response from server
				bytesReceived = recv(sock, buffer, MSG_BUF_SIZE, 0);

				if (bytesReceived == SOCKET_ERROR)
				{
					std::cerr << "recv() failed! Error: " << ::GetLastError() << std::endl;
					break;
				}

				if (bytesReceived == 0)
				{
					std::cout << "SERVER> " << "Server didn't send anything." << std::endl;
					break;
				}

				if (bytesReceived > 0)
				{
					std::cout << "SERVER> " << std::string(buffer, 0, bytesReceived) << std::endl;
				}
			}
		}

	} while (userInput.size() > 0);

	closesocket(sock);
	WSACleanup();
}