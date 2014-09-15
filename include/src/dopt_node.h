#ifndef _ENTANGLE_DOPT_NODE
#define _ENTANGLE_DOPT_NODE

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

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
			OTNodeLink(std::string hostname, size_t port, sit_t id);

			sit_t get_identifier();
			size_t get_port();
			std::string get_hostname();

		private:
			/**
			 * variable names are from the paper
			 */
			sit_t s;

			vec_t v;
			log_t l;
			q_t q;

			/**
			 * connection-related stuff
			 */
			std::string hostname;
			size_t port;
	};

	/**
	 * the node itself
	 */
	class OTNode;
	typedef bool (OTNode::*disp_func)(std::string);
	class OTNode {

		public:
			OTNode(size_t port, size_t max_conn);
			~OTNode();

			std::string get_context();

			// void local_update();
			// void remote_update();

			/**
			 * receive messages
			 */
			void up();
			void dn();

			std::string enc_upd_t(upd_t arg);
			upd_t dec_upd_t(std::string arg);
			bool cmp_upd_t(upd_t s, upd_t o);

			// calls which will SEND OUT data
			bool join(std::string hostname, size_t port);
			bool drop(sit_t s);
			bool ins(size_t pos, char c);
			bool del(size_t pos);

			static const std::string cmd_join;

		private:
			std::shared_ptr<std::atomic<bool>> flag;
			std::shared_ptr<msgpp::MessageNode> node;
			size_t max_conn;
			std::shared_ptr<std::thread> daemon;
			std::shared_ptr<std::thread> dispat;

			obj_t x;
			std::shared_ptr<std::mutex> links_l;
			std::map<sit_t, OTNodeLink> links;
			OTNodeLink self;

			// join success indicators
			std::shared_ptr<std::atomic<bool>> is_joining;
			bool is_joining_errno;

			// differs from the paper -- we're doing the brunt of the work here instead of returning update functions
			// this still *functions* as the transformation matrix, but returns the function *args*, not the *function*
			upd_t t(upd_t u, upd_t up, sit_t p, sit_t pp);

			// dispatch all commands
			void dispatch();

			// process incoming commands
			bool proc_join(std::string arg);
			bool proc_join_ack(std::string arg);
			bool proc_drop(std::string arg);
			bool proc_ins(std::string arg);
			bool proc_del(std::string arg);

			static std::map<std::string, disp_func> dispatch_table;
	};
}
#endif