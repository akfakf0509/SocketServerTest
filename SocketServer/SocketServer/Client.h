#pragma once
#include <iostream>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
class Client
{
private:
	SOCKET socket;
	std::string ip;
public:
	Client(void*, std::string);
	~Client();

	void setSocket(void*);
	void setIP(std::string);

	SOCKET getSocket();
	std::string getIP();
	std::string getIP(bool);
};

