#include <sys/file.h>
#include <memory>
#include <thread>
#include <unistd.h>

#include "libs/giga/client.h"
#include "libs/giga/file.h"

#include "src/entangle_server.h"

/**
 * entangle::ClientInfo
 */

entangle::ClientInfo::ClientInfo(std::string identifier, std::string hostname, size_t port, const std::shared_ptr<giga::File>& file) : id(identifier), hostname(hostname), port(port) {
	this->client = file->open();
}

std::string entangle::ClientInfo::get_identifier() { return(this->id); }
size_t entangle::ClientInfo::get_buf_begin() { return(this->buf_begin); }
size_t entangle::ClientInfo::get_buf_size() { return(this->buf_size); }
std::string entangle::ClientInfo::get_buffer() { return(this->buffer); }
size_t entangle::ClientInfo::get_client_pos() { return(this->get_client()->get_pos()); }
std::shared_ptr<giga::Client> entangle::ClientInfo::get_client() { return(this->client); }
size_t entangle::ClientInfo::get_sync_msg() { return(this->sync_msg); }
size_t entangle::ClientInfo::get_syncpos_msg() { return(this->syncpos_msg); }
size_t entangle::ClientInfo::get_last_client_msg() { return(this->last_client_msg); }
size_t entangle::ClientInfo::get_last_server_msg() { return(this->last_server_msg); }
bool entangle::ClientInfo::get_is_valid() { return(this->is_valid); }
size_t entangle::ClientInfo::get_port() { return(this->port); }
std::string entangle::ClientInfo::get_hostname() { return(this->hostname); }

void entangle::ClientInfo::set_buf_begin(size_t buf_begin) { this->buf_begin = buf_begin; }
void entangle::ClientInfo::set_buf_size(size_t buf_size) { this->buf_size = buf_size; }
void entangle::ClientInfo::set_buffer(std::string buffer) { this->buffer = buffer; }
void entangle::ClientInfo::set_sync_msg(size_t sync_msg) { this->sync_msg = sync_msg; }
void entangle::ClientInfo::set_syncpos_msg(size_t syncpos_msg) { this->syncpos_msg = syncpos_msg; }
void entangle::ClientInfo::set_last_client_msg(size_t last_msg) { this->last_client_msg = last_msg; }
void entangle::ClientInfo::set_last_server_msg(size_t last_msg) { this->last_server_msg = last_msg; }
void entangle::ClientInfo::set_is_valid(bool is_valid) { this->is_valid = is_valid; }

/**
 * entangle::EntangleServer
 */

// cf. http://bit.ly/1Ao5p36
std::map<std::string, entangle::disp_func> entangle::EntangleServer::dispatch_table;

entangle::EntangleServer::EntangleServer(std::string filename, size_t max_conn, size_t port) : count(0) {
	this->file = std::shared_ptr<giga::File> (new giga::File(filename, "rw+"));
	this->node = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(port, msgpp::MessageNode::ipv4, 5));
	this->flag = std::shared_ptr<std::atomic<bool>> (new std::atomic<bool> (0));
	entangle::EntangleServer::dispatch_table.clear();
	entangle::EntangleServer::dispatch_table[entangle::EntangleMessage::cmd_connect] = &entangle::EntangleServer::process_cmd_connect;
}

bool entangle::EntangleServer::get_status() { return(*(this->flag)); }
size_t entangle::EntangleServer::get_port() { return(this->node->get_port()); }
size_t entangle::EntangleServer::get_count() { return(this->count); }

void entangle::EntangleServer::up() {
	if(*(this->flag)) {
		return;
	}

	std::string f = this->file->get_filename();
	f.append(".lock");
	this->file_lock = fopen(f.c_str(), "w+");
	if(flock(fileno(this->file_lock), LOCK_NB | LOCK_EX) == -1) {
		fclose(this->file_lock);
		this->file_lock = NULL;
		throw(exceptionpp::RuntimeError("entangle::EntangleServer::EntangleServer", "file is locked for editing"));
	}

	this->node_t = std::thread(&msgpp::MessageNode::up, &*(this->node));
	while(!this->node->get_status());
	*(this->flag) = 1;
	while(this->node->get_status()) {
		while(this->node->query()) {
			this->process(this->node->pull());
		}
	}
	sleep(1);
	// process remaining items in queue
	while(this->node->query()) {
		this->process(this->node->pull());
	}
	this->dn();
}

void entangle::EntangleServer::dn() {
	if(!*(this->flag)) {
		return;
	}

	this->node_t.join();
	*(this->flag) = 0;

	std::string f = this->file->get_filename();
	f.append(".lock");
	remove(f.c_str());
	fclose(this->file_lock);
	this->file_lock = NULL;
}

void entangle::EntangleServer::process_cmd_connect(std::string buf) {}
/*
void entangle::EntangleServer::process_cmd_drop(std::string buf) {}
void entangle::EntangleServer::process_cmd_resize(std::string buf) {}
void entangle::EntangleServer::process_cmd_sync(std::string buf) {}
void entangle::EntangleServer::process_cmd_syncpos(std::string buf) {}
void entangle::EntangleServer::process_cmd_seek(std::string buf) {}
void entangle::EntangleServer::process_cmd_overwrite(std::string buf) {}
void entangle::EntangleServer::process_cmd_insert(std::string buf) {}
void entangle::EntangleServer::process_cmd_erase(std::string buf) {}
void entangle::EntangleServer::process_cmd_backspace(std::string buf) {}
 */
void entangle::EntangleServer::process(std::string buf) {
	this->count++;
	auto msg = entangle::EntangleMessage(buf, 0, true);
	// drop packet
	if(msg.get_is_invalid()) { return; }
	if(msg.get_cmd().compare("CONN") != 0) {
		if(this->lookaside.count(msg.get_client_id()) == 0) {
			return;
		}
		auto info = this->lookaside.at(msg.get_client_id());
		if(entangle::EntangleServer::dispatch_table.count(msg.get_cmd()) == 0) {
			msg.set_err(entangle::EntangleMessage::error_unimpl);
			msg.set_args(std::vector<std::string> ());
			msg.set_tail("");
			msg.set_msg_id(info.get_last_server_msg() + 1);
			info.set_last_server_msg(msg.get_msg_id());
			this->node->push(msg.to_string(), info.get_hostname(), info.get_port());
		}
		if((msg.get_cmd().compare("DROP") != 0) && (msg.get_msg_id() > info.get_last_client_msg() + 1)) {
			msg.set_err(entangle::EntangleMessage::error_unexpected);
			msg.set_args(std::vector<std::string> ());
			msg.set_tail("");
			msg.set_msg_id(info.get_last_server_msg() + 1);
			info.set_last_server_msg(msg.get_msg_id());
			this->node->push(msg.to_string(), info.get_hostname(), info.get_port());
			return;
		}
		if(msg.get_msg_id() < info.get_last_client_msg() + 1) {
			return;
		}

	}
	try {
		// cf. http://bit.ly/1pF4Trb
		(this->*entangle::EntangleServer::dispatch_table[msg.get_cmd()])(msg.to_string());
	// command does not exist
	} catch(const std::out_of_range& e) {
		return;
	}

//	this->node->push(msg.to_string(), "localhost", 8888);
}
