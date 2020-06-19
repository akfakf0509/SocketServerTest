#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024
#define SERVER_IP "58.228.116.60"

void RecvFun();
void SendFun();

SOCKET hSocket;

bool connected = false;

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	printf("Connecting..\n");

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

	printf("Connected!\n");
	connected = true;

	std::thread thread1(RecvFun);
	std::thread thread2(SendFun);

	thread1.join();
	thread2.join();

	closesocket(hSocket);

	WSACleanup();

	system("pause");
}

void RecvFun() {
	while (true) {
		char cBuffer[PACKET_SIZE] = {};

		if (recv(hSocket, cBuffer, PACKET_SIZE, 0) <= 0)
			break;

		printf("Recv Msg : %s\n", cBuffer);
	}
	connected = false;
}

void SendFun() {
	while (connected) {
		std::string cMsg;
		std::getline(std::cin, cMsg);
		send(hSocket, cMsg.c_str(), cMsg.size(), 0);
	}
}