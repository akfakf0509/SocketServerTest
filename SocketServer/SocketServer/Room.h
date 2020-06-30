#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <vector>
#include "Client.h"

enum class ROOM_STATUS { WAIT, START };

class Client;

class Room
{
private:
	int room_id;
	static int current_id;
	bool is_private;
	std::vector<Client*> players;
	ROOM_STATUS status;
	int max_player;
public:
	Room();
	Room(bool);
	
	void joinRoom(Client*);
	void exitRoom(Client*);
	void StartGame();
	void FinishGame();
	void setPrivate(bool);
	void setStatus(ROOM_STATUS);
	void sendCommand(std::string, Client*);
	
	bool full();
	bool getPrivate();
	bool empty();
	ROOM_STATUS getStatus();
	int playerCount();
	int getRoomId();
};
