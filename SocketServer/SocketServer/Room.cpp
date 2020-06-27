#include "Room.h"

Room::Room(Client* owner) : owner(owner) {
}

Room::Room(int room_id, Client* owner) : room_id(room_id), owner(owner) {
}

void Room::joinRoom(Client* client) {
	players.push_back(client);
}

void Room::exitRoom(Client* client) {

}