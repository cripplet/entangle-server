#ifndef _ENTANGLE_SERVER_H
#define _ENTANGLE_SERVER_H

#include <map>
#include <memory>
#include <string>

#include "src/entangle_msg.h"

namespace entangle {
	class ClientInfo {
		public:
			ClientInfo(std::string identifier);

			std::string get_identifier();
			size_t get_buf_begin();
			size_t get_buf_size();
			std::string get_buffer();
			size_t get_client_pos();
			size_t get_sync_msg();
			size_t get_last_msg();
			bool get_is_valid();

			void set_buf_begin();
			void set_buf_size();
			void set_buffer();
			void set_sync_msg();
			void set_last_msg();
			void set_is_valid();

		private:
			size_t buf_begin;
			size_t buf_size;
			std::string buffer;
			std::unique_ptr<giga::Client> client;
			size_t sync_msg;
			size_t last_msg;
			bool is_valid;
	}

	class EntangleServer {
		public:
			EntangleServer(std::string filename, size_t max_conn);

			void up();
			void dn();

		private:
			std::mutex l; // to deal with threads in up
			std::unique_ptr<msgpp::MessageNode> node;
			std::unique_ptr<giga::File> file;
			std::map<std::string, ClientInfo> lookaside;
			size_t max_conn;
	};
}

#endif
