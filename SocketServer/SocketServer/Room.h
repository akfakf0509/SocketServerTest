#pragma once
#include <vector>
#include "Client.h"
class Room
{
private:
	int room_id;
	Client* owner;
	bool is_private;
	std::vector<Client*> players;
	int max_player;
public:
	Room(Client*);
	
	void joinRoom(Client*);
	void exitRoom(Client*, std::vector<Client*>*);
	void StartGame();
	void setPrivate(bool);
	
	bool full();
	bool getPrivate();
	int playerCount();
	int getRoomId();
};
