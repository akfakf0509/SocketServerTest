#include "Room.h"

Room::Room(Client* owner) : owner(owner), max_player(2) {
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

void Room::setPrivate(bool is_private) {
	this->is_private = is_private;
}

bool Room::full() {
	return players.size() >= max_player;
}

bool Room::getPrivate() {
	return is_private;
}

int Room::playerCount() {
	return players.size();
}

int Room::getRoomId() {
	return room_id;
}