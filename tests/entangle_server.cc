#include <csignal>
#include <memory>
#include <unistd.h>

#include <iostream>

#include "libs/catch/catch.hpp"
#include "libs/exceptionpp/exception.h"
#include "libs/msgpp/msg_node.h"

#include "src/entangle_server.h"

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
	auto tm = std::thread(&entangle::EntangleServer::up, &*m);
	while(!m->get_status());

	auto c = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8888));

	raise(SIGINT);
	tm.join();
}
