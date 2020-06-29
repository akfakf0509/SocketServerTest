#include "Client.h"

int Client::current_id = 0;

Client::Client(void* socket, std::string ip, STATUS status) : socket((SOCKET)socket), ip(ip), room(nullptr), status(status) {
	client_id = Client::current_id++;
}

Client::~Client() {
	closesocket(socket);
}

void Client::setSocket(void* socket_) {
	socket = (SOCKET)socket_;
}

void Client::setIP(std::string ip_) {
	ip = ip_;
}

void Client::setRoom(Room* room) {
	this->room = room;
}

void Client::setStatus(STATUS status_) {
	status = status_;
}

SOCKET Client::getSocket() {
	return socket;
}

std::string Client::getIP() {
	return ip;
}

std::string Client::getIP(bool do_censor) {
	std::string new_ip(ip);

	if (do_censor) {
		int censor_count = 0;

		for (auto iter = --new_ip.end(); (*iter) != '.'; iter--) {
			(*iter) = '*';
			censor_count++;
		}

		for (censor_count; censor_count < 3; censor_count++) {
			new_ip.push_back('*');
		}
	}

	return new_ip;
}

int Client::getClientId() {
	return client_id;
}

Room* Client::getRoom() {
	return room;
}

STATUS Client::getStatus() {
	return status;
}

bool Client::operator==(Client client) {
	return this->getClientId() == client.getClientId();
}