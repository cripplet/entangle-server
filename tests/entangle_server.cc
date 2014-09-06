#include <memory>

#include <iostream>

#include "libs/catch/catch.hpp"

#include "src/entangle_server.h"

TEST_CASE("entangle|entangle_server-init") {
	auto m = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 10, 8088));
	std::cout << "up" << std::endl;
	m->up();
}
