#include "Client.h"

Client::Client(void* socket, std::string ip) : socket((SOCKET)socket), ip(ip) {
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