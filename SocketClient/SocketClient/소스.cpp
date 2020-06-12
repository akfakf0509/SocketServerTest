#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024
#define SERVER_IP "39.124.9.196"

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	printf("Connecting..\n");

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

	printf("Connected!\n");

	char cBuffer[PACKET_SIZE] = {};
	std::string cMsg = "Client Send";
	while (true) {
		std::getline(std::cin, cMsg);
		send(hSocket, cMsg.c_str(), cMsg.size(), 0);

		recv(hSocket, cBuffer, PACKET_SIZE, 0);
		printf("Recv Msg : %s\n", cBuffer);
	}

	closesocket(hSocket);

	WSACleanup();

	system("pause");
}