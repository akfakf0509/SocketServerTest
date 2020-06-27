#pragma once
#include <vector>
#include "Client.h"
class Room
{
private:
	int room_id;
	Client* owner;
	std::vector<Client*> players;
public:
	Room(Client*);
	Room(int, Client*);
	
	void joinRoom(Client*);
	void exitRoom(Client*);
	void StartGame();
	
	bool full();
};

