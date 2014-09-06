#include <memory>
#include <thread>

#include <iostream>

#include "libs/giga/client.h"
#include "libs/giga/file.h"

#include "src/entangle_server.h"

entangle::ClientInfo::ClientInfo(std::string identifier, const std::shared_ptr<giga::File>& file) : id(identifier) {
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
size_t entangle::ClientInfo::get_last_msg() { return(this->last_msg); }
bool entangle::ClientInfo::get_is_valid() { return(this->is_valid); }

void entangle::ClientInfo::set_buf_begin(size_t buf_begin) { this->buf_begin = buf_begin; }
void entangle::ClientInfo::set_buf_size(size_t buf_size) { this->buf_size = buf_size; }
void entangle::ClientInfo::set_buffer(std::string buffer) { this->buffer = buffer; }
void entangle::ClientInfo::set_sync_msg(size_t sync_msg) { this->sync_msg = sync_msg; }
void entangle::ClientInfo::set_syncpos_msg(size_t syncpos_msg) { this->syncpos_msg = syncpos_msg; }
void entangle::ClientInfo::set_last_msg(size_t last_msg) { this->last_msg = last_msg; }
void entangle::ClientInfo::set_is_valid(bool is_valid) { this->is_valid = is_valid; }

entangle::EntangleServer::EntangleServer(std::string filename, size_t max_conn, size_t port) {
	if(0// yadda lock file exists
	) {}
	this->file = std::shared_ptr<giga::File> (new giga::File(filename, "rw+"));
	this->node = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(port, msgpp::MessageNode::ipv4, 5));
}

void entangle::EntangleServer::up() {
	auto t = std::thread(&msgpp::MessageNode::up, &*(this->node));
	while(this->node->get_status()) {
		while(this->node->query()) {
			std::cout << this->node->pull() << std::endl;
		}
	}
	t.join();
	// process remaining items in queue
	while(this->node->query()) {
		std::cout << this->node->pull() << std::endl;
	}
	this->dn();
}

void entangle::EntangleServer::dn() {}
