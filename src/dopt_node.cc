#include <atomic>
#include <ctime>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <iostream>

#include "libs/exceptionpp/exception.h"
#include "libs/msgpp/msg_node.h"

#include "src/dopt_node.h"

/**
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

entangle::OTNodeLink::OTNodeLink() {}
entangle::OTNodeLink::OTNodeLink(std::string hostname, size_t port, sit_t id) {
	this->hostname = hostname;
	this->port = port;
	this->s = id;
}

entangle::sit_t entangle::OTNodeLink::get_identifier() { return(this->s); }
size_t entangle::OTNodeLink::get_port() { return(this->port); }
std::string entangle::OTNodeLink::get_hostname() { return(this->hostname); }

std::map<std::string, entangle::disp_func> entangle::OTNode::dispatch_table;
const std::string entangle::OTNode::cmd_join = "JOIN";
const std::string entangle::OTNode::cmd_join_ack = "JACK";

entangle::OTNode::OTNode(size_t port, size_t max_conn) {
	this->node = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(port, msgpp::MessageNode::ipv4, 5, max_conn + 1));
	this->max_conn = max_conn;
	this->self = entangle::OTNodeLink("localhost", port, rand());
	this->flag = std::shared_ptr<std::atomic<bool>> (new std::atomic<bool> (0));
	this->is_joining = std::shared_ptr<std::atomic<bool>> (new std::atomic<bool> (0));
	this->links_l = std::shared_ptr<std::mutex> (new std::mutex ());
	this->is_joining_errno = 0;

	// set up dispatch table
	entangle::OTNode::dispatch_table.clear();
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_join] = &entangle::OTNode::proc_join;
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_join_ack] = &entangle::OTNode::proc_join_ack;
}
entangle::OTNode::~OTNode() { this->dn(); }

std::string entangle::OTNode::enc_upd_t(entangle::upd_t arg) {
	std::stringstream buf;
	buf << (size_t) arg.type << ":" << arg.pos << ":" << arg.c;
	return(buf.str());
}

/**
 * expected format: T:P:C
 */
entangle::upd_t entangle::OTNode::dec_upd_t(std::string arg) {
	entangle::upd_t u = { 0, 0, '\0' };
	std::vector<std::string> v = this->parse(arg);

	if(v.size() != 3) {
		return(u);
	}
	u.type = (entangle::func_type) std::stoll(v.at(0));
	u.pos = (size_t) std::stoll(v.at(1));
	u.c = (uint8_t) v.at(2).at(0);
	return(u);
}

bool entangle::OTNode::cmp_upd_t(entangle::upd_t s, entangle::upd_t o) {
	return((s.type == o.type) && (s.pos == o.pos) && (s.c == o.c));
}

// start listening for active packets
void entangle::OTNode::up() {
	if(*(this->flag) == 1) {
		return;
	}
	*(this->flag) = 1;
	this->daemon = std::shared_ptr<std::thread> (new std::thread(&msgpp::MessageNode::up, &*(this->node)));
	this->dispat = std::shared_ptr<std::thread> (new std::thread(&entangle::OTNode::dispatch, this));
	while(this->node->get_status() == 0);
}

/**
 * expected format: C[4]:A
 */
void entangle::OTNode::dispatch() {
	while(*(this->flag) == 1) {
		std::string msg = this->node->pull("", true);
		if(msg.compare("") != 0) {
			if(entangle::OTNode::dispatch_table.count(msg.substr(0, 4)) != 0) {
				(this->*entangle::OTNode::dispatch_table[msg.substr(0, 4)])(msg.substr(5));
			}
		}
	}
}

void entangle::OTNode::dn() {
	if(*(this->flag) == 0) {
		return;
	}
	*(this->flag) = 0;
	this->node->dn();
	this->daemon->join();
	this->dispat->join();
}

bool entangle::OTNode::join(std::string hostname, size_t port) {
	if(*(this->is_joining) == 1) { return(false); }
	*(this->is_joining) = 1;
	this->is_joining_errno = 0;
	std::stringstream buf;
	buf << "JOIN:" << this->self.get_identifier() << ":" << this->self.get_port() << ":" << this->self.get_hostname();
	int succ = (this->node->push(buf.str(), hostname, port, true) == buf.str().length());
	*(this->is_joining) = succ;
	if(succ) {
		// wait for confirmation from proc_join_ack
		int t = time(NULL);
		while(*(this->is_joining) == 1) {
			// timeout
			if((size_t) (time(NULL) - t) > this->node->get_timeout()) {
				*(this->is_joining) = 0;
				this->is_joining_errno = 1;
				break;
			}
			sleep(1);
		}
		return(!this->is_joining_errno);
	}
	return(succ);
}
bool entangle::OTNode::drop(entangle::sit_t s) {
	std::stringstream buf;
	buf << "DROP:" << s;
	auto info = this->links.at(s);
	int succ = this->node->push(buf.str(), info.get_hostname(), info.get_port(), true) == buf.str().length();
	if(succ) {
		std::lock_guard<std::mutex> l(*(this->links_l));
		this->links.erase(s);
	}
	return(succ);
}
bool entangle::OTNode::ins(size_t pos, char c) {
	// for ... broadcast
	return(true);
}
bool entangle::OTNode::del(size_t pos) {
	return(true);
}

/**
 * dispatch stuff
 */

std::vector<std::string> entangle::OTNode::parse(std::string arg, size_t n_args) {
	std::vector<std::string> v, u;

	// cf. http://bit.ly/1o7a4Rq
	size_t curr;
	size_t next = -1;

	do {
		curr = next + 1;
		next = arg.find_first_of(":", curr);
		v.push_back(arg.substr(curr, next - curr));
	} while (next != std::string::npos);

	// fold the last several args into the required number of elements
	if(n_args == 0) {
		n_args = v.size();
	}

	for(size_t i = 0; i < n_args; ++i) {
		u.push_back(v.at(i));
	}
	if(v.size() > n_args) {
		for(size_t i = n_args; i < v.size(); ++i) {
			u.back().append(":");
			u.back().append(v.at(i));
		}
	}

	return(u);
}

/**
 * expected format: SIT_ID:H:P
 */
void entangle::OTNode::proc_join(std::string arg) {
	std::vector<std::string> v = this->parse(arg, 3);
	if(v.size() != 3) {
		return;
	}
	entangle::sit_t client_id;
	size_t client_port;
	std::string client_hostname;
	try {
		client_id = (entangle::sit_t) std::stoll(v.at(0));
		client_port = (size_t) std::stoll(v.at(1));
		client_hostname = v.at(2);
	} catch(const std::invalid_argument& e) {
		return;
	}

	if(this->links.count(client_id) == 0) {
		this->links[client_id] = OTNodeLink(client_hostname, client_port, client_id);
		this->join_ack(client_id);
	}
}

bool entangle::OTNode::join_ack(entangle::sit_t s) {
	std::stringstream buf;
	buf << "JACK:" << this->self.get_identifier() << ":" << this->self.get_port() << ":" << this->self.get_hostname();

	auto info = this->links.at(s);
	return(this->node->push(buf.str(), info.get_hostname(), info.get_port(), true) == buf.str().length());
}

// allow the original JOIN client to add the server to the list of links
void entangle::OTNode::proc_join_ack(std::string arg) {
	std::vector<std::string> v = this->parse(arg, 3);
	if(v.size() != 3) {
		return;
	}
	entangle::sit_t client_id;
	size_t client_port;
	std::string client_hostname;
	try {
		client_id = (entangle::sit_t) std::stoll(v.at(0));
		client_port = (size_t) std::stoll(v.at(1));
		client_hostname = v.at(2);
	} catch(const std::invalid_argument& e) {
		return;
	}

	if(this->links.count(client_id) == 0) {
		this->links[client_id] = OTNodeLink(client_hostname, client_port, client_id);
		this->is_joining_errno = 0;
		*(this->is_joining) = 0;
	}
}
/*

			void up();
			void dn();

			void join();
			void drop();

		private:
			std::shared_ptr<std::atomic<bool>> flag;
			std::shared_ptr<msgpp::MessageNode> node;
			size_t max_conn;

			obj_t x;
			std::map<sit_t, OTNodeLink> links;

			// differs from the paper -- we're doing the brunt of the work here instead of returning update functions
			// this still *functions* as the transformation matrix, but returns the function *args*, not the *function*
			upd_t t(upd_t u, upd_t up, sit_t p, sit_t pp);
	};
*/
