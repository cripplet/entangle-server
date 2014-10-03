#ifndef _ENTANGLE_DOPT_NODE_H
#define _ENTANGLE_DOPT_NODE_H

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "libs/giga/client.h"
#include "libs/giga/file.h"
#include "libs/msgpp/msg_node.h"

/**
 * a node from the Cormack dOPT tree algorithm
 *
 * cf. http://bit.ly/1uKHmd7 for more information
 */

namespace entangle {
	/**
	 * typedefs as per paper, figure 4
	 */
	typedef size_t sit_t;
	typedef std::string obj_t;
	// workaround C++ strict typing -- we need the function args as input into the transformation matrix
	typedef uint8_t func_type;
	const func_type ins = 0;
	const func_type del = 1;
	const func_type nop = 2;
	typedef struct {
		func_type type;
		size_t pos;
		char c;
	} upd_t;
	typedef std::map<sit_t, size_t> vec_t;
	typedef std::map<size_t, upd_t> log_t;
	// queue element
	typedef struct {
		sit_t s;
		vec_t v;
		upd_t u;
	} qel_t;
	typedef std::vector<qel_t> q_t;

	/**
	 * the auxiliary info for an OTNode
	 */
	class OTNodeLink {
		public:
			OTNodeLink();
			OTNodeLink(const std::shared_ptr<giga::File>& f, std::string hostname, size_t port, sit_t id);
			~OTNodeLink();

			sit_t get_identifier();
			size_t get_port();
			std::string get_hostname();

			size_t get_server_count();
			size_t get_client_count();
			void set_server_count();
			void set_client_count();

			void set_client(const std::shared_ptr<giga::File>& f);
			std::shared_ptr<giga::Client> get_client();
			std::shared_ptr<log_t> get_l();

		private:
			/**
			 * variable names are from the paper
			 */
			sit_t s;
			// equivalent to V[S]
			size_t server_count;
			// equivalent to V[s]
			size_t client_count;
			std::shared_ptr<log_t> l;

			std::shared_ptr<giga::Client> c;

			/**
			 * connection-related stuff
			 */
			std::string hostname;
			size_t port;
	};

	/**
	 * the node itself
	 */
	template <typename T>
	class OTNode {
		typedef void (OTNode<T>::*disp_func)(std::string);
		public:
			OTNode();
			OTNode(size_t port, size_t max_conn);
			~OTNode();

			std::string get_context();

			/**
			 * receive messages
			 */
			void up();
			void dn();

			std::string enc_upd_t(upd_t arg);
			upd_t dec_upd_t(std::string arg);
			bool cmp_upd_t(upd_t s, upd_t o);
			// branching factor of the tree (i.e. the number of clients connected to this link
			size_t size();

			// binds the node to a particular backend
			bool bind(std::string filename);
			bool free();

			// calls which will SEND OUT data
			bool join(std::string hostname, size_t port);
			bool drop(std::string hostname, size_t port);

			// equivalent of local_update
			bool ins(size_t pos, char c);
			bool del(size_t pos);
			bool save();

			static const std::string cmd_join;
			static const std::string cmd_join_ack;
			static const std::string cmd_sync;
			static const std::string cmd_drop;
			static const std::string cmd_insert;
			static const std::string cmd_delete;
			static const std::string cmd_save;

			void set_hook(const std::shared_ptr<T>& hook);

		private:
			std::weak_ptr<T> hook;

			std::shared_ptr<std::atomic<bool>> flag;
			std::shared_ptr<msgpp::MessageNode> node;
			size_t max_conn;

			// background threads running
			std::shared_ptr<std::thread> daemon;
			std::shared_ptr<std::thread> dispat;
			std::shared_ptr<std::thread> proc_q;

			// shared context -- the actual data to be edited
			obj_t x;
			std::shared_ptr<giga::File> f;
			bool is_dirty;

			// tree structure stuff
			std::shared_ptr<std::recursive_mutex> links_l;
			std::map<sit_t, OTNodeLink> links;
			OTNodeLink self;
			q_t q;

			bool is_root;
			size_t host;

			// a target exists
			bool is_bound;

			// join success indicators
			std::shared_ptr<std::atomic<bool>> is_joining;
			bool is_joining_errno;

			// dispatch all commands
			void dispatch();

			std::vector<std::string> parse(std::string arg, size_t n_args = 0);

			// process incoming commands
			bool join_ack(sit_t s);
			bool sync(sit_t s);
			void proc_join(std::string arg);
			void proc_sync(std::string arg);
			void proc_join_ack(std::string arg);
			void proc_drop(std::string arg);
			void proc_save(std::string arg);

			// equivalent of remote_update
			void proc_ins(std::string arg);
			void proc_del(std::string arg);

			// locks for the node's log and queue
			std::shared_ptr<std::recursive_mutex> q_l;

			// differs from the paper -- we're doing the brunt of the work here instead of returning update functions
			// this still *functions* as the transformation matrix, but returns the function *args*, not the *function*
			upd_t t(upd_t u, upd_t up, sit_t p, sit_t pp);
			void apply(const std::shared_ptr<giga::Client>& c, upd_t u);

			// the bulk of the context update logic resides here
			void process();

			static std::map<std::string, disp_func> dispatch_table;
			static std::chrono::milliseconds increment;
	};
}

#include "src/templates/dopt_node.template"

#endif
