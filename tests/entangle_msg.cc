#include "libs/catch/catch.hpp"
#include "libs/exceptionpp/exception.h"

#include "src/entangle_msg.h"

TEST_CASE("entangle|entangle_msg") {
	auto m = entangle::EntangleMessage(":182:::CONN::foobar123");
        REQUIRE(m.to_string().compare("0:182:::CONN:0:foobar123") == 0);
	REQUIRE(m.get_ack() == 0);
	REQUIRE(m.get_msg_id() == 182);
	REQUIRE(m.get_err() == 0);

	REQUIRE_THROWS_AS(entangle::EntangleMessage(":182:::CONN:foobar123"), exceptionpp::InvalidOperation);
}
