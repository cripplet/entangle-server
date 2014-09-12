#include <atomic>
#include <iomanip>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
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

entangle::OTNode::OTNode(size_t port, size_t max_conn) {
	this->node = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(port, msgpp::MessageNode::ipv4, 5, max_conn + 1));
	this->max_conn = max_conn;
	this->self = entangle::OTNodeLink("localhost", port, rand());
	this->flag = std::shared_ptr<std::atomic<bool>> (new std::atomic<bool> (0));
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
	// cf. http://bit.ly/1o7a4Rq
	size_t curr;
	size_t next = -1;
	std::vector<std::string> v;
	do {
		curr = next + 1;
		next = arg.find_first_of(":", curr);
		v.push_back(arg.substr(curr, next - curr));
	} while (next != std::string::npos);
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
}

void entangle::OTNode::dn() {
	if(*(this->flag) == 0) {
		return;
	}
	*(this->flag) = 0;
	raise(SIGINT);
	this->daemon->join();
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
