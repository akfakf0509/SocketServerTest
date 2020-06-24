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

#define BLACK 0 
#define BLUE 1 
#define GREEN 2 
#define CYAN 3 
#define RED 4 
#define MAGENTA 5 
#define BROWN 6 
#define LIGHTGRAY 7 
#define DARKGRAY 8 
#define LIGHTBLUE 9 
#define LIGHTGREEN 10 
#define LIGHTCYAN 11 
#define LIGHTRED 12 
#define LIGHTMAGENTA 13 
#define YELLOW 14 
#define WHITE 15 

void printTime();
void RecvThread(Client*);
int RecvFun(SOCKET, char*, int, int);
void CommandFun();
void textcolor(int, int);

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

	printTime(); textcolor(GREEN, BLACK); std::cout << "Successfully opended" << std::endl;
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);

	threads.push_back(new std::thread(CommandFun));

	while (wait_client) {
		SOCKET sock = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);
		Client* client = new Client((void*)sock, inet_ntoa(tClntAddr.sin_addr));

		hClient.push_back(client);

		printTime(); textcolor(GREEN, BLACK); std::cout << "IP : " << client->getIP(censor_ip) << " Connected" << std::endl;

		std::thread* thread = new std::thread(RecvThread, client);
		threads.push_back(thread);
	}

	for (auto iter : threads) {
		iter->join();
	}

	closesocket(hListen);

	WSACleanup();
}

void textcolor(int foreground, int background)
{
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printTime() {
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);

	textcolor(WHITE, BLACK); std::cout << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] ";
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
		printTime(); textcolor(WHITE, BLACK); std::cout << client.getIP(censor_ip) << " : " << buf << std::endl;
		//----------Ŭ���̾�Ʈ���� ����------------------
		for (auto iter : hClient)
		{
			if (iter->getSocket() != client.getSocket())
			{
				int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// ����� ���� ������
				sendsize = send(iter->getSocket(), buf, (int)strlen(buf), 0);					// �� �����ŭ ������ ����..
			}
		}
		//-----------------------------------------------
	}
	printTime(); textcolor(GREEN, BLACK); std::cout << "IP : " << client.getIP(censor_ip) << " DIsconnected" << std::endl;

	for (auto iter = hClient.begin(); iter != hClient.end(); iter++) {
		if ((**iter) == client) {
			hClient.erase(iter);
			break;
		}
	}

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
	while (true) {
		std::string str;

		textcolor(WHITE, BLACK);

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
			if (tmp == "set"){
				if (megs.size() > 0) {
					tmp = megs.front();
					megs.pop();
					if (tmp == "do_censor") {
						if (megs.size() > 0) {
							tmp = megs.front();
							megs.pop();
							if (tmp == "true" || tmp == "1") {
								censor_ip = true;
							}
							else if (tmp == "false" || tmp == "0") {
								censor_ip = false;
							}
							printTime(); textcolor(GREEN, BLACK); std::cout << "now do_censor is " << censor_ip << std::endl;
						}
						else {
							printTime(); textcolor(RED, BLACK); std::cout << "Can not find value" << std::endl;
						}
					}
					else {
						printTime(); textcolor(RED, BLACK); std::cout << "Can not find setting name" << std::endl;
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Can not find setting name" << std::endl;
				}
			}
			else if (tmp == "stop") {
				printTime(); textcolor(GREEN, BLACK); std::cout << "stopping.." << std::endl;

				for (auto iter : hClient) {
					shutdown(iter->getSocket(), SD_SEND);
				}

				shutdown(hListen, SD_SEND);

				textcolor(WHITE, BLACK);
				break;
			}
			else if (tmp == "say") {
				if (megs.size() > 0) {
					tmp = megs.front();
					while (megs.size() > 1) {
						megs.pop();
						tmp += " " + megs.front();
					}

					for (auto iter : hClient)
					{
						int size = (int)tmp.size();
						int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// ����� ���� ������
						sendsize = send(iter->getSocket(), tmp.c_str(), size, 0);					// �� �����ŭ ������ ����..
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Can not find msg" << std::endl;
				}
			}
			else if (tmp == "list") {
				if (hClient.empty()) {
					printTime(); textcolor(GREEN, BLACK); std::cout << "No clients" << std::endl;
				}
				int index = 0;
				for (auto iter : hClient) {
					printTime(); textcolor(GREEN, BLACK); std::cout << "[" << index++ << "]" << " IP : " << iter->getIP(censor_ip) << std::endl;
				}
			}
			else if (tmp == "kick") {
				if (megs.size() > 0) {
					tmp = megs.front();
					bool is_ip = false;

					for (auto iter : tmp) {
						if (iter == '.')
							is_ip = true;
					}

					if (is_ip) {
						for (auto iter = hClient.begin(); iter != hClient.end(); iter++) {
							if ((*iter)->getIP() == tmp) {
								shutdown((*iter)->getSocket(), SD_SEND);
								closesocket((*iter)->getSocket());

								delete (*iter);

								hClient.erase(iter);
								break;
							}
						}
					}
					else {
						int index = std::stoi(tmp);
						
						shutdown(hClient[index]->getSocket(), SD_SEND);
						closesocket(hClient[index]->getSocket());

						delete (hClient[index]);

						hClient.erase(hClient.begin() + index);
						break;
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Can not find target" << std::endl;
				}
			}
			else {
				printTime(); textcolor(RED, BLACK); std::cout << "Can not find function" << std::endl;
			}

		}
		else {
			printTime(); textcolor(RED, BLACK); std::cout << "Can not find command" << std::endl;
		}
	}
}