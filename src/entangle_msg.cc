#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "libs/exceptionpp/exception.h"

#include "src/entangle_msg.h"

const std::string entangle::EntangleMessage::cmd_connect = "CONN";
const std::string entangle::EntangleMessage::cmd_drop = "DROP";
const std::string entangle::EntangleMessage::cmd_resize = "RESIZE";
const std::string entangle::EntangleMessage::cmd_sync = "SYNC";
const std::string entangle::EntangleMessage::cmd_syncpos = "SYNCPOS";
const std::string entangle::EntangleMessage::cmd_seek = "SEEK";
const std::string entangle::EntangleMessage::cmd_overwrite = "OVER";
const std::string entangle::EntangleMessage::cmd_insert = "INSERT";
const std::string entangle::EntangleMessage::cmd_erase = "ERASE";
const std::string entangle::EntangleMessage::cmd_backspace = "BACK";

entangle::EntangleMessage::EntangleMessage(std::string string, size_t n_args, bool silent_fail) : is_invalid(false) {
	std::vector<std::string> v;

	// cf. http://bit.ly/1o7a4Rq
	size_t curr;
	size_t next = -1;
	size_t count = 0;
	do {
		curr = next + 1;
		next = string.find_first_of(":", curr);
		v.push_back(string.substr(curr, next - curr));
		count++;
	}
	while (next != std::string::npos && v.size() <= 7 + n_args);
	if(v.size() < 7 + n_args) {
		if(!silent_fail) {
			throw(exceptionpp::InvalidOperation("entangle::EntangleMessage::EntangleMessage", "invalid input size"));
		}
		this->is_invalid = true;
		return;
	}

	try {
		this->ack = (v.at(0).compare("") == 0) ? 0 : (bool) stol(v.at(0));
		if((v.at(0).compare("") != 0) && stol(v.at(0)) > 1) {
			if(!silent_fail) {
				throw(exceptionpp::InvalidOperation("entangle::EntangleMessage::EntangleMessage", "invalid ACK value"));
			}
			this->is_invalid = true;
			return;
		}
		this->msg_id = (v.at(1).compare("") == 0) ? 0 : (size_t) stol(v.at(1));
		this->err = (v.at(5).compare("") == 0) ? 0 : (size_t) stol(v.at(5));
	} catch(const std::invalid_argument& e) {
		if(!silent_fail) {
			throw(exceptionpp::InvalidOperation("entangle::EntangleMessage::EntangleMessage", "could not convert argument to numeric value"));
		}
		this->is_invalid = true;
		return;
	}
	this->client_id = v.at(2);
	this->auth = v.at(3);
	this->cmd = v.at(4);
	this->args = std::vector<std::string> (v.begin() + 6, v.begin() + 6 + n_args);
	for(size_t i = 6 + n_args; i < v.size(); ++i) {
		this->tail.append(v.at(i));
		if(i != v.size() - 1) {
			this->tail.append(":");
		}
	}
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
bool entangle::EntangleMessage::get_is_invalid() { return(this->is_invalid); }

void entangle::EntangleMessage::set_err(size_t err) { this->err = err; }
void entangle::EntangleMessage::set_msg_id(size_t msg_id) { this->msg_id = msg_id; }
void entangle::EntangleMessage::set_args(std::vector<std::string> args) { this->args = args; }
void entangle::EntangleMessage::set_tail(std::string tail) { this->tail = tail; }

std::string entangle::EntangleMessage::to_string() {
	std::stringstream buf;
	buf << this->get_ack() << ":" << this->get_msg_id() << ":" << this->get_client_id() << ":" << this->get_auth() << ":" << this->get_cmd() << ":" << this->get_err() << ":";
	for(size_t i = 0; i < this->get_args().size(); ++i) {
		buf << this->get_args().at(i) << ":";
	}
	buf << this->get_tail();
	return(buf.str());
}
