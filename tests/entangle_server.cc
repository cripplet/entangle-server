#include <csignal>
#include <memory>
#include <unistd.h>

#include <iostream>

#include "libs/catch/catch.hpp"
#include "libs/exceptionpp/exception.h"

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
