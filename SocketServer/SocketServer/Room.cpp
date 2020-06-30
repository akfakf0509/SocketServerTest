#include "Room.h"

int Room::current_id = 0;

Room::Room() : is_private(false), status(ROOM_STATUS::WAIT), max_player(2) {
	room_id = Room::current_id++;
}

Room::Room(bool is_private) : is_private(is_private), status(ROOM_STATUS::WAIT), max_player(2) {
	room_id = Room::current_id++;
}

void Room::joinRoom(Client* client) {
	if (client->getRoom() != nullptr)
		client->getRoom()->exitRoom(client);
	client->setRoom(this);
	client->setStatus(STATUS::MATCH);
	players.push_back(client);

	char tmp_buf[256];
	sprintf(tmp_buf, "TO_CLIENT ROOM UPDATE %d", playerCount());
	std::string buf(tmp_buf);
	sendCommand(buf, client);
}

void Room::exitRoom(Client* client) {
	client->setRoom(nullptr);
	client->setStatus(STATUS::WAIT);
	for (auto iter = players.begin(); iter != players.end(); iter++) {
		if ((*iter) == client) {
			players.erase(iter);
		}
	}

	char tmp_buf[256];
	sprintf(tmp_buf, "TO_CLIENT ROOM UPDATE %d", playerCount());
	std::string buf(tmp_buf);
	sendCommand(buf, client);
}

void Room::StartGame() {
	if (status == ROOM_STATUS::WAIT) {
		status = ROOM_STATUS::START;
		for (int i = 0; i < players.size(); i++) {
			players[i]->setStatus(STATUS::PLAY);
			char tmp_buf[256];
			sprintf(tmp_buf, "TO_CLIENT GAME NUMBER %d", i + 1);
			sendCommand(std::string(tmp_buf), players[i]);
		}
	}
}

void Room::FinishGame() {
	if (status == ROOM_STATUS::START) {
		status = ROOM_STATUS::WAIT;
		for (auto iter : players) {
			iter->setStatus(STATUS::MATCH);
		}
	}
}

void Room::setPrivate(bool is_private) {
	this->is_private = is_private;
}

void Room::setStatus(ROOM_STATUS status) {
	this->status = status;
}

void Room::sendCommand(std::string buf, Client* sender) {
	for (auto iter : players)
	{
		if (iter->getSocket() != sender->getSocket())
		{
			unsigned char size_buf[256] = "";
			sprintf((char*)size_buf, "%d$", buf.size());
			int sendsize = send(iter->getSocket(), (char*)size_buf, strlen((char*)size_buf), 0);
			unsigned char tmp[256] = "";
			strcpy((char*)tmp, buf.c_str());
			sendsize = send(iter->getSocket(), (char*)tmp, strlen((char*)tmp), 0);
		}
	}
}

bool Room::full() {
	return (int)players.size() >= max_player;
}

bool Room::getPrivate() {
	return is_private;
}

bool Room::empty() {
	return players.empty();
}

ROOM_STATUS Room::getStatus() {
	return status;
}

int Room::playerCount() {
	return players.size();
}

int Room::getRoomId() {
	return room_id;
}