#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "libs/exceptionpp/exception.h"
#include "libs/msgpp/msg_node.h"

#include "src/dopt_node.h"

/**
 *	type definitions reference
 *
 *	these are copied over from include/src/dopt_node.h for easy lookups
 *
 *	typedef size_t sit_t;
 *	typedef std::string obj_t;
 *	typedef uint8_t func_type;
 *	const func_type ins = 0;
 *	const func_type del = 1;
 *	typedef struct {
 *		func_type type;
 *		size_t pos;
 *		char c;
 *	} upd_t;
 *	typedef std::map<sit_t, size_t> vec_t;
 *	typedef std::map<size_t, upd_t> log_t;
 *	typedef struct {
 *		sit_t s;
 *		vec_t v;
 *		upd_t u;
 *	} qel_t;
 *	typedef std::vector<qel_t> q_t;
 */

entangle::OTNodeLink::OTNodeLink() {
	this->c = NULL;
}
entangle::OTNodeLink::OTNodeLink(const std::shared_ptr<giga::File>& f, std::string hostname, size_t port, sit_t id) {
	this->hostname = hostname;
	this->port = port;
	this->s = id;

	this->server_count = 0;
	this->client_count = 0;
	this->l = std::shared_ptr<entangle::log_t> (new entangle::log_t ());

	this->set_client(f);
}
entangle::OTNodeLink::~OTNodeLink() {
	if(this->c != NULL) {
		this->c->close();
	}
}

std::shared_ptr<giga::Client> entangle::OTNodeLink::get_client() { return(this->c); }
void entangle::OTNodeLink::set_client(const std::shared_ptr<giga::File>& f) {
	if(f != NULL) {
		this->c = f->open();
	}
}

entangle::sit_t entangle::OTNodeLink::get_identifier() { return(this->s); }
size_t entangle::OTNodeLink::get_port() { return(this->port); }
std::string entangle::OTNodeLink::get_hostname() { return(this->hostname); }

size_t entangle::OTNodeLink::get_server_count() { return(this->server_count); }
size_t entangle::OTNodeLink::get_client_count() { return(this->client_count); }
void entangle::OTNodeLink::set_server_count() { this->server_count++; }
void entangle::OTNodeLink::set_client_count() { this->client_count++; }

std::shared_ptr<entangle::log_t> entangle::OTNodeLink::get_l() { return(this->l); }
