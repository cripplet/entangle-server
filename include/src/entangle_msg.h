#ifndef _ENTANGLE_MSG_H
#define _ENTANGLE_MSG_H

#include <string>
#include <vector>

namespace entangle {
	class EntangleMessage {
		public:
			/**
			 * of the form ACK:MSG_ID:CLIENT_ID:AUTH:CMD:ERR:AUX
			 *
			 * if silent_fail is true, return with constructed object, and with ::invalid set to true
			 */
			EntangleMessage(std::string string, size_t n_args = 0, bool silent_fail = false);

			EntangleMessage(bool ack, size_t msg_id, std::string client_id, std::string auth, std::string cmd, size_t err = error_no_err, std::vector<std::string> args = std::vector<std::string>(), std::string tail = "");

			bool get_ack();
			size_t get_msg_id();
			std::string get_client_id();
			std::string get_auth();
			std::string get_cmd();
			size_t get_err();
			std::vector<std::string> get_args();
			std::string get_tail();
			bool get_is_invalid();

			void set_err(size_t err);
			void set_msg_id(size_t msg_id);
			void set_args(std::vector<std::string> args);
			void set_tail(std::string tail);

			std::string to_string();

			static const size_t error_no_err = 0;
			static const size_t error_unexpected = 400;
			static const size_t error_denied = 401;
			static const size_t error_unimpl = 401;
			static const size_t error_desync = 409;
			static const size_t error_max_conn = 503;

			static const std::string cmd_connect;
			static const std::string cmd_drop;
			static const std::string cmd_resize;
			static const std::string cmd_sync;
			static const std::string cmd_syncpos;
			static const std::string cmd_seek;
			static const std::string cmd_overwrite;
			static const std::string cmd_insert;
			static const std::string cmd_erase;
			static const std::string cmd_backspace;

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
			bool is_invalid;
	};
}

#endif
