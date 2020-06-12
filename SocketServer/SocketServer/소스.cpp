#include <iostream>
#include <string>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hListen;
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tListenAddr = {};
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(PORT);
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Waiting for client..\n");
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);

	printf("Connected!\n");
	char cBuffer[PACKET_SIZE] = {};
	std::string cMsg = "정해성 바보";
	while (true) {
		recv(hClient, cBuffer, PACKET_SIZE, 0);
		printf("Recv Msg : %s\n", cBuffer);

		std::getline(std::cin, cMsg);
		send(hClient, cMsg.c_str(), cMsg.size(), 0);
	}

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();
}