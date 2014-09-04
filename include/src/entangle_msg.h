#ifndef _ENTANGLE_MSG_H
#define _ENTANGLE_MSG_H

#include <string>
#include <vector>

namespace entangle {
	class EntangleMessage {
		public:
			EntangleMessage(std::string string);
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
