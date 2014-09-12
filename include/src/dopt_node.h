#ifndef _ENTANGLE_DOPT_NODE
#define _ENTANGLE_DOPT_NODE

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <vector>

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
	typedef struct {
		uint8_t type;
		size_t pos;
		char c;
	} upd_t;
	typedef std::map<sit_t, size_t> vec_t;
	typedef std::map<size_t, upd_t> log_t;
	typedef struct {
		sit_t s;
		vec_t v;
		upd_t u;
	} qel_t;
	typedef std::vector<qel_t> q_t;
	const bool ins = 0;
	const bool del = 1;

	/**
	 * the auxiliary info for an OTNode
	 */
	class OTNodeLink {
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
	class OTNode {
		public:
			OTNode(std::string hostname, size_t port, size_t max_conn);
			size_t get_identifier();
			std::string get_context();

			// void local_update();
			// void remote_update();

			/**
			 * receive messages
			 */
			void up();
			void dn();

			void join();
			void drop();

		private:
			std::shared_ptr<std::atomic<bool>> flag;
			size_t max_conn;

			obj_t x;
			std::map<sit_t, OTNodeLink> links;

			// differs from the paper -- we're doing the brunt of the work here instead of returning update functions
			// this still *functions* as the transformation matrix, but returns the function *args*, not the *function*
			upd_t t(upd_t u, upd_t up, sit_t p, sit_t pp);
	};
}

#endif
