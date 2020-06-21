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
void RecvThread(void*, char*);
int RecvFun(SOCKET, char*, int, int);

std::vector<SOCKET> hClient;
std::vector<std::thread*> threads;

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

		std::thread *thread1 = new std::thread(RecvThread, (void*)sock, inet_ntoa(tClntAddr.sin_addr));
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

void __cdecl RecvThread(void* p, char* ip)
{
	SOCKET sock = (SOCKET)p;
	char buf[256];
	int size;
	while (1)
	{
		//-----------Ŭ���̾�Ʈ�κ��� ����------------
		int recvsize = RecvFun(sock, (char*)&size, sizeof(int), 0);		// ����� ���� �а�
		recvsize = RecvFun(sock, buf, size, 0);								// �� �����ŭ�� ������ �б�
		if (recvsize <= 0)		break;
		//------------------------------------------------
		buf[recvsize] = '\0';
		printTime(); printf("%s : %s\n", ip, buf);
		//----------Ŭ���̾�Ʈ���� ����------------------
		for (int i = 0; i < hClient.size(); i++)
		{
			if (hClient[i] != sock)
			{
				int sendsize = send(hClient[i], (char*)&size, sizeof(int), 0);		// ����� ���� ������
				sendsize = send(hClient[i], buf, strlen(buf), 0);					// �� �����ŭ ������ ����..
			}
		}
		//-----------------------------------------------
	}
	printTime(); printf("IP : %s Disconnected\n", ip);
	//------------vector�� �ִ� ������ �����-----------
	std::vector<SOCKET>::iterator iter = hClient.begin();
	for (int i = 0; i < hClient.size(); i++)
	{
		if (hClient[i] == sock)
		{
			hClient.erase(iter);
			break;
		}
		iter++;
	}
	//---------------------------------------------------

	//------------���� ����---------------------
	closesocket(sock);
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