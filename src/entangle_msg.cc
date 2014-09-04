#include <sstream>
#include <string>
#include <vector>

#include "src/entangle_msg.h"

entangle::EntangleMessage::EntangleMessage(std::string string) {
}

entangle::EntangleMessage::EntangleMessage(bool ack, size_t msg_id, std::string client_id, std::string auth, std::string cmd, size_t err, std::vector<std::string> args, std::string tail) {
	this->ack = ack;
	this->msg_id = msg_id;
	this->client_id = client_id;
	this->auth = auth;
	this->cmd = cmd;
	this->err = err;
	this->args = args;
	this->tail = tail;
}

bool entangle::EntangleMessage::get_ack() { return(this->ack); }
size_t entangle::EntangleMessage::get_msg_id() { return(this->msg_id); }
std::string entangle::EntangleMessage::get_client_id() { return(this->client_id); }
std::string entangle::EntangleMessage::get_auth() { return(this->auth); }
std::string entangle::EntangleMessage::get_cmd() { return(this->cmd); }
size_t entangle::EntangleMessage::get_err() { return(this->err); }
std::vector<std::string> entangle::EntangleMessage::get_args() { return(this->args); }
std::string entangle::EntangleMessage::get_tail() { return(this->tail); }

std::string entangle::EntangleMessage::to_string() {
	std::stringstream buf;
	buf << this->get_ack() << ":" << this->get_msg_id() << ":" << this->get_client_id() << ":" << this->get_auth() << ":" << this->get_cmd() << ":" << this->get_err() << ":";
	for(size_t i = 0; i < this->get_args().size(); ++i) {
		buf << this->get_args().at(i) << ":";
	}
	buf << this->get_tail();
	return(buf.str());
}
