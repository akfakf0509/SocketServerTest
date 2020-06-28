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
#include "Room.h"

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
void Match(Client*);
void Match(int, Client*);

std::vector<Client*> clients;
std::vector<std::thread*> threads;
std::vector<Room*> rooms;

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

	printTime(); textcolor(GREEN, BLACK); std::cout << "Successfully opended" << std::endl; textcolor(WHITE, BLACK);
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);

	threads.push_back(new std::thread(CommandFun));

	while (wait_client) {
		SOCKET sock = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);

		Client* client = new Client((void*)sock, inet_ntoa(tClntAddr.sin_addr), STATUS::WAIT);

		clients.push_back(client);

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
		//-----------클라이언트로부터 수신------------
		int recvsize = RecvFun(client.getSocket(), (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 읽고
		recvsize = RecvFun(client.getSocket(), buf, size, 0);								// 그 사이즈만큼의 데이터 읽기
		if (recvsize <= 0)		break;
		//------------------------------------------------
		buf[recvsize] = '\0';
		printTime(); textcolor(WHITE, BLACK); std::cout << client.getIP(censor_ip) << " : " << buf << std::endl;

		std::queue<std::string> megs;
		std::string tmp;

		for (auto iter : std::string(buf)) {
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

			if (tmp == "TO_SERVER") {
				if (megs.size() > 0) {
					tmp = megs.front();
					megs.pop();

					if (tmp == "JOIN") {
						if (megs.size() > 0) {
							tmp = megs.front();
							megs.pop();

							if (tmp == "AUTO") {
								client.setStatus(STATUS::MATCH);
								printTime(); textcolor(GREEN, BLACK); std::cout << "Match auto" << std::endl;
								//connect room in auto
							}
							else if (tmp == "ROOM_ID") {
								if (megs.size() > 0) {
									tmp = megs.front();
									megs.pop();

									client.setStatus(STATUS::MATCH);
									//coonect room in room_id
								}
								else {
									printTime(); textcolor(RED, BLACK); std::cout << "Can not find room_id" << std::endl;
								}
							}
							else {
								printTime(); textcolor(RED, BLACK); std::cout << "Can not find command : TO_SERVER JOIN >>" << tmp << "<< here" << std::endl;
							}
						}
						else {
							printTime(); textcolor(RED, BLACK); std::cout << "Need more command" << std::endl;
						}
					}
					else {
						printTime(); textcolor(RED, BLACK); std::cout << "Can not find command : TO_SERVER >>" << tmp << "<< here" << std::endl;
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Need more command" << std::endl;
				}
			}
			else if (tmp == "TO_CLIENTS") {
				//----------클라이언트에게 전송------------------
				for (auto iter : clients)
				{
					if (iter->getSocket() != client.getSocket())
					{
						int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 보내고
						sendsize = send(iter->getSocket(), buf, (int)strlen(buf), 0);					// 그 사이즈만큼 데이터 보냄..
					}
				}
				//-----------------------------------------------
			}
			else {
				printTime(); textcolor(RED, BLACK); std::cout << "Can not find command >>" << tmp << "<< here" << std::endl;
			}
		}
		else {
			printTime(); textcolor(RED, BLACK); std::cout << "Can not find command" << std::endl;
		}
	}

	printTime(); textcolor(GREEN, BLACK); std::cout << "IP : " << client.getIP(censor_ip) << " DIsconnected" << std::endl;

	for (auto iter = clients.begin(); iter != clients.end(); iter++) {
		if ((**iter) == client) {
			clients.erase(iter);
			break;
		}
	}

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

							printTime(); textcolor(GREEN, BLACK); std::cout << "Setting do_censor is " << censor_ip << std::endl;
						}
						else {
							printTime(); textcolor(RED, BLACK); std::cout << "Cant not find value" << std::endl;
						}
					}
					else {
						printTime(); textcolor(RED, BLACK); std::cout << "Cant not find setting name : set do_censor >>" << tmp << "<< here" << std::endl;
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Cant not find setting name" << std::endl;
				}
			}
			else if (tmp == "stop") {
				printTime(); textcolor(GREEN, BLACK); std::cout << "stopping.." << std::endl;

				for (auto iter : clients) {
					shutdown(iter->getSocket(), SD_BOTH);
					closesocket(iter->getSocket());

					delete iter;
				}

				wait_client = false;
				shutdown(hListen, SD_BOTH);
				closesocket(hListen);

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

					for (auto iter : clients)
					{
						int size = (int)tmp.size();
						int sendsize = send(iter->getSocket(), (char*)&size, sizeof(int), 0);		// 사이즈를 먼저 보내고
						sendsize = send(iter->getSocket(), tmp.c_str(), size, 0);					// 그 사이즈만큼 데이터 보냄..
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Can not find msg" << std::endl;
				}
			}
			else if (tmp == "list") {
				if (clients.empty()) {
					printTime(); textcolor(GREEN, BLACK); std::cout << "No clients" << std::endl;
				}
				int index = 0;
				for (auto iter : clients) {
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
						bool kicked = false;
						for (auto iter = clients.begin(); iter != clients.end(); iter++) {
							if ((*iter)->getIP() == tmp) {
								closesocket((*iter)->getSocket());

								delete (*iter);

								clients.erase(iter);

								kicked = true;
								break;
							}
						}
						if(!kicked)
							printTime(); textcolor(RED, BLACK); std::cout << "Can not find target : kick >>" << tmp << "<< here" << std::endl;
					}
					else if(0 <= std::stol(tmp) && std::stoi(tmp) < (int)clients.size()){
						int index = std::stoi(tmp);
						
						closesocket(clients[index]->getSocket());

						delete (clients[index]);

						clients.erase(clients.begin() + index);
					}
					else {
						printTime(); textcolor(RED, BLACK); std::cout << "Can not find target : kick >>" << tmp << "<< here" << std::endl;
					}
				}
				else {
					printTime(); textcolor(RED, BLACK); std::cout << "Can not find target" << std::endl;
				}
			}
			else {
				printTime(); textcolor(RED, BLACK); std::cout << "Can not find function : >>" << tmp << "<< here" << std::endl;
			}

		}
		else {
			printTime(); textcolor(RED, BLACK); std::cout << "Can not find command" << std::endl;
		}
	}
}

void Match(Client* client) {
	if (rooms.size() == 0) {
		rooms.push_back(new Room(client));
		rooms[0]->joinRoom(client);
	}
	else {
		bool joined = false;

		for (auto iter : rooms) {
			if (!iter->full()) { //full 아직 정의 안됨
				iter->joinRoom(client);
				joined = true;
			}
		}

		if (!joined) {
			rooms.push_back(new Room(client));
			rooms[rooms.size() - 1]->joinRoom(client);
		}
	}
}

void Match(int room_id, Client* client) {
	for (auto iter : rooms) {
		if (iter->getRoomId() == room_id && !iter->full()) {
			iter->joinRoom(client);
		}
		else if (iter->getRoomId() == room_id && iter->full()) {
			//방이 꽉참
		}
		else {
			//방을 찾을 수 없음
		}
	}
}