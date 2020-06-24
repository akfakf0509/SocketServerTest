#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <string>
#include <thread>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024
#define SERVER_IP "192.168.43.147" //"58.228.116.60"

void printTime();
void RecvThread(void*);
int RecvFun(SOCKET, char*, int, int);

SOCKET hSocket;

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	printTime(); printf("Connecting..\n");

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

	printTime(); printf("Connected!\n");

	std::thread thread1(RecvThread, (void*)hSocket);

	while (true) {
		std::string str;

		while (str.size() == 0) {
			std::getline(std::cin, str);
		}

		int size = str.size();
		int sendsize = send(hSocket, (char*)&size, sizeof(int), 0);
		sendsize = send(hSocket, str.c_str(), size, 0);

		if (sendsize <= 0) {
			printTime(); std::cout << "Falled send" << std::endl;
			break;
		}
	}

	thread1.join();

	closesocket(hSocket);

	WSACleanup();

	system("pause");
}

void printTime() {
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);

	std::cout << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] ";
}

void __cdecl RecvThread(void* p)
{
	SOCKET sock = (SOCKET)p;
	char buf[256];
	int size;
	while (1)
	{
		//-----------�����κ��� ����------------
		int recvsize = recv(sock, (char*)&size, sizeof(int), 0);
		recvsize = recv(sock, buf, size, 0);
		if (recvsize <= 0)
		{
			printTime(); printf("���� ��������\n");
			break;
		}
		//------------------------------------------------
		buf[recvsize] = '\0';
		printTime(); printf("%s\n", buf);
	}
}

int RecvFun(SOCKET s, char* buf, int len, int flags)		// �� ������ ���� ���� ���ϴ� �Լ�...
{
	int received;		// �ѹ��� ���� ��..
	char* ptr = buf;
	int left = len;		//������..

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)		// ���������� ���� ����..
			return SOCKET_ERROR;
		else if (received == 0)				// �� �о ������ ������ �׳� break;
			break;
		left -= received;					// �ѹ� ������ŭ�� ���� �翡�� ��..
		ptr += received;					// �ѹ� ������ŭ ������ ���۸� �ű�..
											//    ==> �Ȱ��� ptr���� ������ �����ϴ�...
											//          ==> ptr�� �ּҿ��� ����.. ������ ������ �����ּ�
	}

	return (len - left);					// ���̿��� �������� ������ ����... �ᱹ ������� ���̸� �����ϴ°̴ϴ� ����..
}