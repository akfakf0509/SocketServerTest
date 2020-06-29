#include "Room.h"

int Room::current_id = 0;

Room::Room() : is_private(false), max_player(2) {
	room_id = Room::current_id++;
}

Room::Room(bool is_private) : is_private(is_private), max_player(2) {
	room_id = Room::current_id++;
}

void Room::joinRoom(Client* client) {
	if (client->getRoom() != nullptr)
		client->getRoom()->exitRoom(client);
	client->setRoom(this);
	players.push_back(client);
}

void Room::exitRoom(Client* client) {
	client->setRoom(nullptr);
	for (auto iter = players.begin(); iter != players.end(); iter++) {
		if ((*iter) == client) {
			players.erase(iter);
		}
	}
}

void Room::StartGame() {

}

void Room::setPrivate(bool is_private) {
	this->is_private = is_private;
}

bool Room::full() {
	return (int)players.size() >= max_player;
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