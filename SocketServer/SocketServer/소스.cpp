#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <WinSock2.h>
#include "Client.h"

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024

void printTime();
void RecvThread(Client*);
int RecvFun(SOCKET, char*, int, int);
void CommandFun();

std::vector<Client*> hClient;
std::vector<std::thread*> threads;

const bool censor_ip = true;


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
		Client *client = new Client((void*)sock, inet_ntoa(tClntAddr.sin_addr));

		hClient.push_back(client);

		printTime(); std::cout << "IP : " << client->getIP(censor_ip) << " Connected" << std::endl;

		std::thread *thread1 = new std::thread(RecvThread, client);
		threads.push_back(thread1);
	}

	for (auto iter : threads) {
		iter->join();
	}

	closesocket(hListen);

	WSACleanup();
}

void printTime() {
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);

	std::cout << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] ";
}

void __cdecl RecvThread(Client* client_)
{
	Client client = *client_;

	char buf[256];
	int size;
	while (1)
	{
		//-----------Ŭ���̾�Ʈ�κ��� ����------------
		int recvsize = RecvFun(client.getSocket(), (char*)&size, sizeof(int), 0);		// ����� ���� �а�
		recvsize = RecvFun(client.getSocket(), buf, size, 0);								// �� �����ŭ�� ������ �б�
		if (recvsize <= 0)		break;
		//------------------------------------------------
		buf[recvsize] = '\0';
		printTime(); std::cout << client.getIP(censor_ip) << " : " << buf << std::endl;
		//----------Ŭ���̾�Ʈ���� ����------------------
		for (auto iter : hClient)
		{
			if (iter->getSocket() != client.getSocket())
			{
				int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// ����� ���� ������
				sendsize = send(iter->getSocket(), buf, strlen(buf), 0);					// �� �����ŭ ������ ����..
			}
		}
		//-----------------------------------------------
	}
	printTime(); std::cout << "IP : " << client.getIP(censor_ip) << " DIsconnected" << std::endl;

	//------------���� ����---------------------
	closesocket(client.getSocket());
	//----------------------------------------
}

int RecvFun(SOCKET s, char* buf, int len, int flags)		// �� ������ ���� ���� ���ϴ� �Լ�
{
	int received;		// �ѹ��� ���� ��
	char* ptr = buf;
	int left = len;		//������

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)		// ���������� ���� ����
			return SOCKET_ERROR;
		else if (received == 0)				// �� �о ������ ������ �׳� break;
			break;
		left -= received;					// �ѹ� ������ŭ�� ���� �翡�� ��
		ptr += received;					// �ѹ� ������ŭ ������ ���۸� �ű�
											//    ==> �Ȱ��� ptr���� ������ �����ϴ�
											//          ==> ptr�� �ּҿ��� ����.. ������ ������ �����ּ�
	}

	return (len - left);					// ���̿��� �������� ������ ���� �ᱹ ������� ���̸� �����ϴ°̴ϴ� ����
}

void CommandFun() {
	std::string str;

	std::getline(std::cin, str);

	if (str[0] == '/') {
		std::queue<std::string> megs;

		std::string tmp;

		for (auto iter : str) {
			if (iter == ' ') {
				megs.push(tmp);
				tmp.clear();
			}
			else {
				tmp.push_back(iter);
			}
		}

		//add here
	}
}