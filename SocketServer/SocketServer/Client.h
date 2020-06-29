#pragma once
#include <iostream>
#include <string>
#include <WinSock2.h>

enum class STATUS { WAIT, MATCH, PLAY };

class Client
{
private:
	SOCKET socket;
	std::string ip;
	int client_id;
	STATUS status;
public:
	Client(void*, std::string, STATUS);
	~Client();

	void setSocket(void*);
	void setIP(std::string);
	void setStatus(STATUS);

	SOCKET getSocket();
	std::string getIP();
	std::string getIP(bool);
	int getClientId();
	STATUS getStatus();

	bool operator==(Client);
};