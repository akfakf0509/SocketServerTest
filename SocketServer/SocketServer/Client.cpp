#include "Client.h"

Client::Client(void* socket, std::string ip, STATUS status) : socket((SOCKET)socket), ip(ip), status(status) {
	static int current_id;
	client_id = current_id++;
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

STATUS Client::getStatus() {
	return status;
}

bool Client::operator==(Client client) {
	return this->getClientId() == client.getClientId();
}