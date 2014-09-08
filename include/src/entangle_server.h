#ifndef _ENTANGLE_SERVER_H
#define _ENTANGLE_SERVER_H

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "libs/giga/client.h"
#include "libs/giga/file.h"
#include "libs/msgpp/msg_node.h"

#include "src/entangle_msg.h"

namespace entangle {
	class ClientInfo {
		public:
			ClientInfo(std::string identifier, std::string hostname, size_t get_port, const std::shared_ptr<giga::File>& file, std::string auth = "");

			std::string get_identifier();
			size_t get_port();
			std::string get_hostname();
			std::string get_auth();
			size_t get_buf_begin();
			size_t get_buf_size();
			std::string get_buffer();
			size_t get_client_pos();
			std::shared_ptr<giga::Client> get_client();
			size_t get_sync_msg();
			size_t get_syncpos_msg();
			size_t get_last_client_msg();
			size_t get_last_server_msg();
			bool get_is_valid();

			void set_buf_begin(size_t buf_begin);
			void set_buf_size(size_t buf_size);
			void set_buffer(std::string buffer);
			void set_sync_msg(size_t sync_msg);
			void set_syncpos_msg(size_t syncpos_msg);
			void set_last_client_msg(size_t last_msg);
			void set_last_server_msg(size_t last_msg);
			void set_is_valid(bool is_valid);

			void drop();

		private:
			std::string id;
			std::string hostname;
			size_t port;
			std::string auth;
			size_t buf_begin;
			size_t buf_size;
			std::string buffer;
			std::shared_ptr<giga::Client> client;
			size_t sync_msg;
			size_t syncpos_msg;
			size_t last_client_msg;
			size_t last_server_msg;
			bool is_valid;
	};

	// cf. http://bit.ly/1Ao5p36
	class EntangleServer;
	typedef void (EntangleServer::*disp_func)(std::string);
	class EntangleServer {
		public:
			EntangleServer(std::string filename, size_t max_conn, size_t port, std::string token = "");

			bool get_status();
			size_t get_port();
			size_t get_count();

			void up();

		private:
			FILE *file_lock;
			std::shared_ptr<msgpp::MessageNode> node;
			std::shared_ptr<giga::File> file;
			std::shared_ptr<std::atomic<bool>> flag;
			std::map<std::string, std::shared_ptr<ClientInfo>> lookaside;
			size_t max_conn;
			std::string token;
			std::thread node_t;
			size_t count;

			void dn();

			std::string get_token();

			void process(std::string buf);
			void process_cmd_connect(std::string buf);

			static std::map<std::string, disp_func> dispatch_table;
	};
}

#endif
