#ifndef _ENTANGLE_MSG_H
#define _ENTANGLE_MSG_H

#include <string>
#include <vector>

namespace entangle {
	class EntangleMessage {
		public:
			/**
			 * of the form ACK:MSG_ID:CLIENT_ID:AUTH:CMD:ERR:AUX
			 */
			EntangleMessage(std::string string, size_t n_args = 0);
			EntangleMessage(bool ack, size_t msg_id, std::string client_id, std::string auth, std::string cmd, size_t err, std::vector<std::string> args, std::string tail);

			bool get_ack();
			size_t get_msg_id();
			std::string get_client_id();
			std::string get_auth();
			std::string get_cmd();
			size_t get_err();
			std::vector<std::string> get_args();
			std::string get_tail();

			std::string to_string();

		protected:
			size_t length;
			bool ack;
			size_t msg_id;
			std::string client_id;
			std::string auth;
			std::string cmd;
			size_t err;
			std::vector<std::string> args;
			std::string tail;
	};
}

#endif
