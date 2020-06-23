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

SOCKET hListen;

bool censor_ip = true;
bool wait_client = true;

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

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

	threads.push_back(new std::thread(CommandFun));

	while (wait_client) {
		SOCKET sock = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);
		Client* client = new Client((void*)sock, inet_ntoa(tClntAddr.sin_addr));

		hClient.push_back(client);

		printTime(); std::cout << "IP : " << client->getIP(censor_ip) << " Connected" << std::endl;

		std::thread* thread = new std::thread(RecvThread, client);
		threads.push_back(thread);
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
		//-----------클라이언트로부터 수신------------
		int recvsize = RecvFun(client.getSocket(), (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 읽고
		recvsize = RecvFun(client.getSocket(), buf, size, 0);								// 그 사이즈만큼의 데이터 읽기
		if (recvsize <= 0)		break;
		//------------------------------------------------
		buf[recvsize] = '\0';
		printTime(); std::cout << client.getIP(censor_ip) << " : " << buf << std::endl;
		//----------클라이언트에게 전송------------------
		for (auto iter : hClient)
		{
			if (iter->getSocket() != client.getSocket())
			{
				int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 보내고
				sendsize = send(iter->getSocket(), buf, strlen(buf), 0);					// 그 사이즈만큼 데이터 보냄..
			}
		}
		//-----------------------------------------------
	}
	printTime(); std::cout << "IP : " << client.getIP(censor_ip) << " DIsconnected" << std::endl;

	//------------소켓 해제---------------------
	closesocket(client.getSocket());
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

void CommandFun() {
	while (true) {
		std::string str;

		std::getline(std::cin, str);

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
		megs.push(tmp);
		tmp.clear();

		if (megs.size() > 0) {
			tmp = megs.front();
			megs.pop();
			if (tmp == "set" && megs.size() > 0) {
				tmp = megs.front();
				megs.pop();
				if (tmp == "do_censor" && megs.size() > 0) {
					tmp = megs.front();
					megs.pop();
					if (tmp == "true" || tmp == "1") {
						censor_ip = true;
					}
					else if (tmp == "false" || tmp == "0") {
						censor_ip = false;
					}
					printTime(); std::cout << "now do_censor is " << censor_ip << std::endl;
				}
				else {
					printTime(); std::cout << "Can not find value" << std::endl;
				}

			}
			else if (tmp == "stop") {
				printTime(); std::cout << "stopping.." << std::endl;
				break;
			}
			else if (tmp == "say" && megs.size() > 0) {
				tmp = megs.front();
				while (megs.size() > 1) {
					megs.pop();
					tmp += " " + megs.front();
				}

				for (auto iter : hClient)
				{
					int size = tmp.size();
					int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 보내고
					sendsize = send(iter->getSocket(), tmp.c_str(), size, 0);					// 그 사이즈만큼 데이터 보냄..
				}
			}
			else {
				printTime(); std::cout << "Can not find function" << std::endl;
			}

		}
		else {
			printTime(); std::cout << "Can not find command" << std::endl;
		}
	}
}