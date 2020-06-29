#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <vector>
#include "Client.h"

class Client;

class Room
{
private:
	int room_id;
	static int current_id;
	bool is_private;
	std::vector<Client*> players;
	int max_player;
public:
	Room();
	Room(bool);
	
	void joinRoom(Client*);
	void exitRoom(Client*);
	void StartGame();
	void setPrivate(bool);
	void sendCommand(std::string, Client*);
	
	bool full();
	bool getPrivate();
	int playerCount();
	int getRoomId();
};
