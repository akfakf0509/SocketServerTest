#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024

void RecvFun();
void SendFun();

SOCKET hClient;

bool connected = false;

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
	hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);

	printf("Connected!\n");
	connected = true;

	std::thread thread1(RecvFun);
	std::thread thread2(SendFun);

	thread1.join();
	thread2.join();

	printf("Disconnected\n");

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();
}

void RecvFun() {
	while (true) {
		char cBuffer[PACKET_SIZE] = {};
		if (recv(hClient, cBuffer, PACKET_SIZE, 0) <= 0)
			break;

		printf("Recv Msg : %s\n", cBuffer);
	}
	connected = false;
}

void SendFun() {
	while (connected) {
		std::string cMsg;
		std::getline(std::cin, cMsg);
		send(hClient, cMsg.c_str(), cMsg.size(), 0);
	}
}