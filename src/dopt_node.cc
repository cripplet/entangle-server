#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

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

	this->server_count = 0;
	this->client_count = 0;
	this->l = std::shared_ptr<entangle::log_t> (new entangle::log_t ());
}

entangle::sit_t entangle::OTNodeLink::get_identifier() { return(this->s); }
size_t entangle::OTNodeLink::get_port() { return(this->port); }
std::string entangle::OTNodeLink::get_hostname() { return(this->hostname); }

size_t entangle::OTNodeLink::get_server_count() { return(this->server_count); }
size_t entangle::OTNodeLink::get_client_count() { return(this->client_count); }
void entangle::OTNodeLink::set_server_count() { this->server_count++; }
void entangle::OTNodeLink::set_client_count() { this->client_count++; }

std::shared_ptr<entangle::log_t> entangle::OTNodeLink::get_l() { return(this->l); }

std::map<std::string, entangle::disp_func> entangle::OTNode::dispatch_table;
const std::string entangle::OTNode::cmd_join = "JOIN";
const std::string entangle::OTNode::cmd_join_ack = "JACK";
const std::string entangle::OTNode::cmd_sync = "SYNC";
const std::string entangle::OTNode::cmd_drop = "DROP";
const std::string entangle::OTNode::cmd_insert = "INSE";
const std::string entangle::OTNode::cmd_delete = "DELE";
std::chrono::milliseconds entangle::OTNode::increment = std::chrono::milliseconds(50);

entangle::OTNode::OTNode(size_t port, size_t max_conn) {
	this->node = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(port, msgpp::MessageNode::ipv4, 5, max_conn + 5));
	this->max_conn = max_conn;
	this->self = entangle::OTNodeLink("localhost", port, rand());
	this->flag = std::shared_ptr<std::atomic<bool>> (new std::atomic<bool> (0));
	this->is_joining = std::shared_ptr<std::atomic<bool>> (new std::atomic<bool> (0));
	this->links_l = std::shared_ptr<std::recursive_mutex> (new std::recursive_mutex ());
	this->is_joining_errno = 0;
	this->is_root = true;
	this->host = 0;

	this->is_bound = false;

	this->links = std::map<entangle::sit_t, entangle::OTNodeLink> ();
	this->x = entangle::obj_t ();
	this->q = entangle::q_t ();

	// set up dispatch table
	entangle::OTNode::dispatch_table.clear();
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_join] = &entangle::OTNode::proc_join;
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_sync] = &entangle::OTNode::proc_sync;
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_join_ack] = &entangle::OTNode::proc_join_ack;
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_drop] = &entangle::OTNode::proc_drop;
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_insert] = &entangle::OTNode::proc_ins;
	entangle::OTNode::dispatch_table[entangle::OTNode::cmd_delete] = &entangle::OTNode::proc_del;

	// OTNodeLink access locks
	this->q_l = std::shared_ptr<std::recursive_mutex> (new std::recursive_mutex ());
}
entangle::OTNode::~OTNode() { this->dn(); }

bool entangle::OTNode::bind(std::string filename) {
	if(this->is_bound) { return(false); }
	this->is_bound = true;
	return(true);
}

bool entangle::OTNode::free() {
	this->x.assign("");
	this->is_bound = false;
	return(true);
}

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

size_t entangle::OTNode::size() {
	std::lock_guard<std::recursive_mutex> l(*(this->links_l));
	return(this->links.size());
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
	this->proc_q = std::shared_ptr<std::thread> (new std::thread(&entangle::OTNode::process, this));
	while(this->node->get_status() == 0) {
			std::this_thread::sleep_for(entangle::OTNode::increment);
	}
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
		} else {
			std::this_thread::sleep_for(entangle::OTNode::increment);
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
	this->proc_q->join();
}

bool entangle::OTNode::join(std::string hostname, size_t port) {
	if(*(this->is_joining) == 1) { return(false); }
	if(this->is_bound) { return(false); }
	*(this->is_joining) = 1;
	this->is_joining_errno = 0;
	std::stringstream buf;
	buf << entangle::OTNode::cmd_join << ":" << this->self.get_identifier() << ":" << this->self.get_port() << ":" << this->self.get_hostname();
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
			std::this_thread::sleep_for(entangle::OTNode::increment);
		}
		return(!this->is_joining_errno);
	}
	return(succ);
}

bool entangle::OTNode::drop(std::string hostname, size_t port) {
	std::lock_guard<std::recursive_mutex> l(*(this->links_l));
	std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));

	size_t target_id = 0;
	auto target = this->links.end();
	for(auto it = this->links.begin(); it != this->links.end(); ++it) {
		auto info = it->second;
		if((info.get_hostname().compare(hostname) == 0) && (info.get_port() == port)) {
			target_id = it->first;
			target = it;
			break;
		}
	}

	if(target == this->links.end()) {
		return(false);
	}

	std::stringstream buf;
	buf << entangle::OTNode::cmd_drop << ":" << this->self.get_identifier();
	this->node->push(buf.str(), hostname, port, true);
	this->links.erase(target);

	// disconnecting from host
	if(!this->is_root && target_id == this->host) {
		this->q.clear();
		this->is_bound = false;
		this->x.assign("");
		// disconnect all connected clients
		auto it = this->links.begin();
		while(it != this->links.end()) {
			this->node->push(buf.str(), it->second.get_hostname(), it->second.get_port(), true);
			this->links.erase(it++);
		}
	}

	return(true);
}

bool entangle::OTNode::ins(size_t pos, char c) {
	if(!this->is_bound) { return(false); }
	std::lock_guard<std::recursive_mutex> l(*(this->q_l));
	this->q.push_back(entangle::qel_t { this->self.get_identifier(), std::map<entangle::sit_t, size_t> (), entangle::upd_t { entangle::ins, pos, c } });
	return(true);
}

bool entangle::OTNode::del(size_t pos) {
	if(!this->is_bound) { return(false); }
	std::lock_guard<std::recursive_mutex> l(*(this->q_l));
	this->q.push_back(entangle::qel_t { this->self.get_identifier(), std::map<entangle::sit_t, size_t> (), entangle::upd_t { entangle::del, pos, '\0' } });
	return(true);
}

/**
 * cf. fig. 2, Cormack 1995 (A Counterexample to dOPT)
 */
void entangle::OTNode::process() {
	std::map<entangle::func_type, std::string> tlb;
	tlb[entangle::nop] = "NOPE";
	tlb[entangle::ins] = entangle::OTNode::cmd_insert;
	tlb[entangle::del] = entangle::OTNode::cmd_delete;

	while(*(this->flag) == 1) {
		{
			std::lock_guard<std::recursive_mutex> links_l(*(this->links_l));
			std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));

			/**
			 * remote: update instance of type qel_t
			 */
			auto qel = this->q.begin();
			while(qel != this->q.end()) {
				size_t S = this->self.get_identifier();
				size_t s = qel->s;

				bool to_delete = false;

				// local update
				if(S == s) {

					// broadcast remote update
					for(auto info = this->links.begin(); info != this->links.end(); ++info) {
						// V[S] := V[S] + 1
						info->second.set_server_count();
						auto V = entangle::vec_t();
						V[S] = info->second.get_server_count();
						V[info->first] = info->second.get_client_count();
						std::stringstream buf;
						buf << tlb[qel->u.type] << ":" << S << ":" << V[S] << ":" << V[info->first] << ":" << (size_t) qel->u.type << ":" << qel->u.pos << ":" << qel->u.c;
						this->node->push(buf.str(), info->second.get_hostname(), info->second.get_port(), true);

						// L[V[s] + V[S]] := U
						auto L = this->links[info->first].get_l();
						(*L)[V[info->first] + V[S]] = qel->u;
					}

					// X := U(X)
					this->apply(qel->u);
					to_delete = true;
					goto proc_loop_tail;
				// remote update
				} else {
					// invalid update
					if(this->links.count(s) == 0) {
						to_delete = true;
						goto proc_loop_tail;
					}
					auto V = entangle::vec_t();
					V[S] = this->links[s].get_server_count();
					V[s] = this->links[s].get_client_count();

					// delay until v[s] = V[s] + 1 (proceed if V >= v)
					if(qel->v[s] != V[s] + 1) {
						if(qel->v[s] < V[s] + 1) {
							to_delete = true;
						}
						goto proc_loop_tail;
					}

					// L[V[s] + v[S] + 1 .. V[s] + V[s] + 1] := ...
					auto L = this->links[s].get_l();
					for(size_t k = V[s] + V[S]; k >= V[s] + qel->v[S] + 1; --k) {
						if(L->count(k - 1) != 0) {
							(*L)[k] = L->at(k - 1);
						}
					}

					// L[V[s] + v[S]] := u
					(*L)[V[s] + qel->v[S]] = qel->u;

					// For k := V[s] + v[S] + 1 to ...
					for(size_t k = (V[s] + qel->v[S] + 1); k < (V[s] + V[S] + 1); ++k) {
						// Let U = L[k]
						if(L->count(k) != 0) {
							auto U = L->at(k);
							// L[k] := T(U, u ...
							L->at(k) = this->t(U, qel->u, S, s);
							// u := T(u, U, ...
							qel->u = this->t(qel->u, U, s, S);
						}
					}

					// V[s] := V[s] + 1
					this->links[s].set_client_count();

					// broadcast remote update to everyone but the originating sender
					for(auto info = this->links.begin(); info != this->links.end(); ++info) {
						if(info->first != s) {
							auto V = entangle::vec_t();
							// as far as the client is aware, this is a local update
							info->second.set_server_count();
							V[S] = info->second.get_server_count();
							V[info->first] = info->second.get_client_count();
							std::stringstream buf;
							buf << tlb[qel->u.type] << ":" << S << ":" << V[S] << ":" << V[info->first] << ":" << (size_t) qel->u.type << ":" << qel->u.pos << ":" << qel->u.c;
							this->node->push(buf.str(), info->second.get_hostname(), info->second.get_port(), true);

							(*(info->second.get_l()))[V[info->first] + V[S]] = qel->u;
						}
					}

					// X := u(X)
					this->apply(qel->u);
					to_delete = true;
					goto proc_loop_tail;
				}
				proc_loop_tail:
				if(to_delete) {
					qel = this->q.erase(qel);
				} else {
					++qel;
				}
			}
		}
		std::this_thread::sleep_for(entangle::OTNode::increment);
	}
}

// we only expect u to be an INSERT or DELETE operation
void entangle::OTNode::apply(entangle::upd_t u) {
	if(u.type == entangle::nop) {
		return;
	}
	if(u.type == entangle::ins) {
		this->x.insert(u.pos, 1, u.c);
	} else if(u.type == entangle::del) {
		this->x.erase(u.pos, 1);
	}
}

std::string entangle::OTNode::get_context() {
	std::lock_guard<std::recursive_mutex> links_l(*(this->links_l));
	std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));
	return(this->x);
}

/**
 * cf. fig. 2, Cormack 1995 (A Counterexample to dOPT)
 */
entangle::upd_t entangle::OTNode::t(entangle::upd_t u, entangle::upd_t up, entangle::sit_t p, entangle::sit_t pp) {
	entangle::upd_t nop = { entangle::nop, 0, '\0' };
	if(u.type == entangle::nop) {
		return(nop);
	}
	if(up.type == entangle::nop) {
		return(u);
	}
	if(u.type == entangle::ins) {
		// INS, INS
		if(up.type == entangle::ins) {
			if(u.pos < up.pos) {
				return(u);
			}
			if(u.pos > up.pos) {
				entangle::upd_t r = { entangle::ins, u.pos + 1, u.c };
				return(r);
			}
			if((u.pos == up.pos) && (u.c == up.c)) {
				return(nop);
			}
			if((u.pos == up.pos) && (p < pp)) {
				return(u);
			}
			if((u.pos == up.pos) && (p > pp)) {
				entangle::upd_t r = { entangle::ins, u.pos + 1, u.c };
				return(r);
			}
		// INS, DEL
		} else {
			if(u.pos <= up.pos) {
				return(u);
			}
			if(u.pos > up.pos) {
				entangle::upd_t r = { entangle::ins, u.pos - 1, u.c };
				return(r);
			}
		}
	} else {
		// DEL, INS
		if(up.type == entangle::ins) {
			if(u.pos < up.pos) {
				return(u);
			}
			if(u.pos >= up.pos) {
				entangle::upd_t r = { entangle::del, u.pos + 1, '\0' };
				return(r);
			}
		// DEL, DEL
		} else {
			if(u.pos < up.pos) {
				return(u);
			}
			if(u.pos > up.pos) {
				entangle::upd_t r = { entangle::del, u.pos - 1, '\0' };
				return(r);
			}
			if(u.pos < up.pos) {
				return(nop);
			}
		}
	}
	return(u);
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
 * expected format: S:P:H
 */
void entangle::OTNode::proc_join(std::string arg) {
	if(!this->is_bound) { return; }
	std::lock_guard<std::recursive_mutex> l(*(this->links_l));

	if(this->links.size() == this->max_conn) {
		return;
	}

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
		this->sync(client_id);
	}
}

bool entangle::OTNode::sync(entangle::sit_t s) {
	std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));

	std::stringstream buf;
	buf << entangle::OTNode::cmd_sync << ":" << this->get_context() << std::endl;

	auto info = this->links.at(s);
	return(this->node->push(buf.str(), info.get_hostname(), info.get_port(), true) == buf.str().length());
}

bool entangle::OTNode::join_ack(entangle::sit_t s) {
	if(!this->is_bound) { return(false); }

	std::lock_guard<std::recursive_mutex> links_l(*(this->links_l));
	std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));

	std::stringstream buf;
	buf << entangle::OTNode::cmd_join_ack << ":" << this->self.get_identifier() << ":" << this->self.get_port() << ":" << this->self.get_hostname();

	auto info = this->links.at(s);
	return(this->node->push(buf.str(), info.get_hostname(), info.get_port(), true) == buf.str().length());
}

/**
 * allow the original JOIN client to add the server to the list of links
 *
 * expected format: S:P:H
 */
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

	{
		std::lock_guard<std::recursive_mutex> links_l(*(this->links_l));
		std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));
		if(this->links.count(client_id) == 0) {
			this->links[client_id] = OTNodeLink(client_hostname, client_port, client_id);
			this->is_joining_errno = 0;
			*(this->is_joining) = 0;
			this->is_root = false;
			this->host = client_id;
			this->bind("");
		}
	}
}

/**
 * sync server-side state with self
 *
 * expected format: X
 */
void entangle::OTNode::proc_sync(std::string arg) {
	std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));
	this->x.assign(arg.substr(0, arg.length() - 1));
}

/**
 * expected format: S
 */
void entangle::OTNode::proc_drop(std::string arg) {
	std::vector<std::string> v = this->parse(arg, 1);
	if(v.size() != 1) {
		return;
	}
	entangle::sit_t client_id;
	try {
		client_id = (entangle::sit_t) std::stoll(v.at(0));
	} catch(const std::invalid_argument& e) {
		return;
	}

	{
		std::lock_guard<std::recursive_mutex> links_l(*(this->links_l));
		std::lock_guard<std::recursive_mutex> q_l(*(this->q_l));
		if(this->links.count(client_id) == 1) {
			this->links.erase(client_id);
		}

		// disconnecting from host
		if(!this->is_root && client_id == this->host) {
			std::stringstream buf;
			buf << entangle::OTNode::cmd_drop << ":" << this->self.get_identifier();
			this->q.clear();
			this->free();
			// disconnect all connected clients
			auto it = this->links.begin();
			while(it != this->links.end()) {
				this->node->push(buf.str(), it->second.get_hostname(), it->second.get_port(), true);
				this->links.erase(it++);
			}
		}
	}
}

/**
 * expected format: S:S_count:count:U
 */
void entangle::OTNode::proc_ins(std::string arg) {
	std::vector<std::string> v = this->parse(arg, 4);
	if(v.size() != 4) {
		return;
	}
	entangle::sit_t client_id;
	size_t client_count;
	size_t server_count;
	entangle::upd_t update;
	try {
		client_id = (entangle::sit_t) std::stoll(v.at(0));
		client_count = (size_t) std::stoll(v.at(1));
		server_count = (size_t) std::stoll(v.at(2));
	} catch(const std::invalid_argument& e) {
		return;
	}
	update = this->dec_upd_t(v.at(3));

	std::map<sit_t, size_t> client_v;
	client_v[client_id] = client_count;
	client_v[this->self.get_identifier()] = server_count;
	entangle::qel_t qel = { client_id, client_v, update };
	{
		std::lock_guard<std::recursive_mutex> l(*(this->q_l));
		this->q.push_back(qel);
	}
}

/**
 * expected format: S:S_count:count:U
 */
void entangle::OTNode::proc_del(std::string arg) {
	std::vector<std::string> v = this->parse(arg, 4);
	if(v.size() != 4) {
		return;
	}
	entangle::sit_t client_id;
	size_t client_count;
	size_t server_count;
	entangle::upd_t update;
	try {
		client_id = (entangle::sit_t) std::stoll(v.at(0));
		client_count = (size_t) std::stoll(v.at(1));
		server_count = (size_t) std::stoll(v.at(2));
	} catch(const std::invalid_argument& e) {
		return;
	}
	update = this->dec_upd_t(v.at(3));

	std::map<sit_t, size_t> client_v;
	client_v[client_id] = client_count;
	client_v[this->self.get_identifier()] = server_count;
	entangle::qel_t qel = { client_id, client_v, update };
	{
		std::lock_guard<std::recursive_mutex> l(*(this->q_l));
		this->q.push_back(qel);
	}
}
