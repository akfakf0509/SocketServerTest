#pragma once
#include <iostream>
#include <string>
#include <WinSock2.h>

enum STATUS { WAIT, MATCH, PLAY };

class Client
{
private:
	SOCKET socket;
	std::string ip;
	STATUS status;
public:
	Client(void*, std::string);
	~Client();

	void setSocket(void*);
	void setIP(std::string);

	SOCKET getSocket();
	std::string getIP();
	std::string getIP(bool);

	bool operator==(Client);
};

