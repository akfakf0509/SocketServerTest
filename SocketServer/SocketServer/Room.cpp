#include "Room.h"

Room::Room(Client* owner) : owner(owner) {
	static int current_id;
	room_id = current_id++;
}

void Room::joinRoom(Client* client) {
	players.push_back(client);
}

void Room::exitRoom(Client* client, std::vector<Client*>* clients) {
	for (auto iter = (*clients).begin(); iter != (*clients).end(); iter++) {
		if ((*iter) == client) {
			clients->erase(iter);
		}
	}
}

void Room::StartGame() {

}

bool Room::full() {
	
}

int Room::getRoomId() {
	return room_id;
}