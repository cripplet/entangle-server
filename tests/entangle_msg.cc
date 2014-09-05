#include "libs/catch/catch.hpp"
#include "libs/exceptionpp/exception.h"

#include "src/entangle_msg.h"

TEST_CASE("entangle|entangle_msg") {
	auto m = entangle::EntangleMessage(":182:::CONN::foobar123");
        REQUIRE(m.to_string().compare("0:182:::CONN:0:foobar123") == 0);
}
