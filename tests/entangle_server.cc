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
	auto m = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 10, 9999));
	auto c = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8888));

	auto tm = std::thread(&entangle::EntangleServer::up, &*m);
	auto tc = std::thread(&msgpp::MessageNode::up, &*c);
	sleep(1);
	while(!m->get_status());

	c->push(entangle::EntangleMessageConnectRequest(rand(), "abcde", "localhost", 8888).to_string(), "localhost", m->get_port());
	sleep(1);

	REQUIRE(m->get_count() == 1);

	raise(SIGINT);
	tc.join();
	tm.join();

	REQUIRE_NOTHROW(c->pull());
}
