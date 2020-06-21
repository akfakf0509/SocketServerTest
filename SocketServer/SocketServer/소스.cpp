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
		//-----------클라이언트로부터 수신------------
		int recvsize = RecvFun(sock, (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 읽고
		recvsize = RecvFun(sock, buf, size, 0);								// 그 사이즈만큼의 데이터 읽기
		if (recvsize <= 0)		break;
		//------------------------------------------------
		buf[recvsize] = '\0';
		printTime(); printf("%s : %s\n", ip, buf);
		//----------클라이언트에게 전송------------------
		for (int i = 0; i < hClient.size(); i++)
		{
			if (hClient[i] != sock)
			{
				int sendsize = send(hClient[i], (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 보내고
				sendsize = send(hClient[i], buf, strlen(buf), 0);					// 그 사이즈만큼 데이터 보냄..
			}
		}
		//-----------------------------------------------
	}
	printTime(); printf("IP : %s Disconnected\n", ip);
	//------------vector에 있는 데이터 지우기-----------
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

	//------------소켓 해제---------------------
	closesocket(sock);
	//----------------------------------------
}

int RecvFun(SOCKET s, char* buf, int len, int flags)		// 다 받을때 까지 리턴 안하는 함수
{
	int received;		// 한번에 읽은 양
	char* ptr = buf;
	int left = len;		//남은양

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)		// 에러났으면 에러 리턴
			return SOCKET_ERROR;
		else if (received == 0)				// 다 읽어서 읽은거 없으면 그냥 break;
			break;
		left -= received;					// 한번 읽은만큼을 남은 양에서 뺌
		ptr += received;					// 한번 읽은만큼 저장할 버퍼를 옮김
											//    ==> 똑같이 ptr에서 돌리면 덮어씌웁니다
											//          ==> ptr은 주소에여 ㅇㅇ.. 저장할 공간의 시작주소
	}

	return (len - left);					// 길이에서 남은양을 뺀값을 리턴 결국 읽으라는 길이를 리턴하는겁니다 ㅇㅇ
}