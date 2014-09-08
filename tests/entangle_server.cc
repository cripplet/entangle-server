#include <csignal>
#include <cstdlib>
#include <memory>
#include <unistd.h>

#include "libs/catch/catch.hpp"
#include "libs/exceptionpp/exception.h"
#include "libs/msgpp/msg_node.h"

#include "src/entangle_server.h"
#include "src/msg_types.h"

TEST_CASE("entangle|entangle_server-init") {
	auto m = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 10, 8088));
	auto n = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 10, 8088));

	auto tm = std::thread(&entangle::EntangleServer::up, &*m);
	while(!m->get_status());

	REQUIRE_THROWS_AS(n->up(), exceptionpp::RuntimeError);

	raise(SIGINT);
	tm.join();
}

TEST_CASE("entangle|entangle_server-conn") {
	auto m = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 10, 9999, "test-server"));
	auto c = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8888));

	auto tm = std::thread(&entangle::EntangleServer::up, &*m);
	auto tc = std::thread(&msgpp::MessageNode::up, &*c);
	sleep(1);
	while(!m->get_status());

	c->push(entangle::EntangleMessageConnectRequest(rand(), "abcde", "localhost", 8888).to_string(), "localhost", m->get_port());
	sleep(1);

	REQUIRE(m->get_count() == 1);

	entangle::EntangleMessage msg;
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull("", true)));
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_denied);
	REQUIRE(msg.get_client_id().compare("") == 0);
	REQUIRE(msg.get_msg_id() == 0);

	raise(SIGINT);
	tc.join();
	tm.join();
}
