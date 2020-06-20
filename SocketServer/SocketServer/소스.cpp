#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <thread>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024

void printTime();
void RecvFun(void*);

std::vector<SOCKET> hClient;

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

	printTime(); std::cout << "Successfully opended" << std::endl;
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);
	while (true) {
		SOCKET sock = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);
		hClient.push_back(sock);

		printTime(); std::cout << "IP : " << inet_ntoa(tClntAddr.sin_addr) << " Connected" << std::endl;
		connected = true;
		std::thread thread1(RecvFun, &sock);
		thread1.join();
	}

	//std::thread thread1(RecvFun);
	//std::thread thread2(SendFun);

	//thread1.join();
	//thread2.join();

	printf("Disconnected\n");

	closesocket(hClient[0]);
	closesocket(hListen);

	WSACleanup();
}

void printTime() {
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);

	std::cout << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] ";
}

void RecvFun(void* p) {
	SOCKET socket = (SOCKET)p;
	while (true) {
		char cBuffer[PACKET_SIZE] = {};
		if (recv(socket, cBuffer, PACKET_SIZE, 0) <= 0)
			break;

		for (auto iter : hClient) {
			send(iter, cBuffer, strlen(cBuffer), 0);
		}
	}
	connected = false;
}

void SendFun() {
	while (connected) {
		std::string cMsg;
		std::getline(std::cin, cMsg);
		send(hClient[0], cMsg.c_str(), cMsg.size(), 0);
	
	}
}