#include "libs/catch/catch.hpp"
#include "libs/exceptionpp/exception.h"

#include "src/entangle_msg.h"
#include "src/msg_types.h"

TEST_CASE("entangle|entangle_msg") {
	auto m = entangle::EntangleMessage("0:182:CLIENT_ID:AUTH:CONN:0:foobar123");
	REQUIRE(m.to_string().compare("0:182:CLIENT_ID:AUTH:CONN:0:foobar123") == 0);
	REQUIRE(m.get_ack() == 0);
	REQUIRE(m.get_msg_id() == 182);
	REQUIRE(m.get_err() == 0);

	m = entangle::EntangleMessage(":182:::CONN::foobar123:foo");
	REQUIRE(m.to_string().compare("0:182:::CONN:0:foobar123:foo") == 0);
	REQUIRE(m.get_tail().compare("foobar123:foo") == 0);

	REQUIRE_THROWS_AS(entangle::EntangleMessage(":::::"), exceptionpp::InvalidOperation);
	REQUIRE_THROWS_AS(entangle::EntangleMessage("A::::::"), exceptionpp::InvalidOperation);
	REQUIRE_THROWS_AS(entangle::EntangleMessage("100::::::"), exceptionpp::InvalidOperation);
	REQUIRE_THROWS_AS(entangle::EntangleMessage(":A:::::"), exceptionpp::InvalidOperation);
	REQUIRE_THROWS_AS(entangle::EntangleMessage(":::::A:"), exceptionpp::InvalidOperation);

	m = entangle::EntangleMessage(":182:::CONN::foobar123:foo:bar:baz", 3);
	REQUIRE(m.to_string().compare("0:182:::CONN:0:foobar123:foo:bar:baz") == 0);
	REQUIRE(m.get_args().at(0).compare("foobar123") == 0);
	REQUIRE(m.get_args().at(1).compare("foo") == 0);
	REQUIRE(m.get_args().at(2).compare("bar") == 0);
	REQUIRE(m.get_tail().compare("baz") == 0);

	REQUIRE_THROWS_AS(entangle::EntangleMessage("::::::", 1), exceptionpp::InvalidOperation);
}

TEST_CASE("entangle|msg_types-conn") {
	auto req = entangle::EntangleMessageConnectRequest(182, "pw", "server-pw");
	REQUIRE(req.to_string().compare("0:182::pw:CONN:0:server-pw") == 0);

	auto res = entangle::EntangleMessageConnectResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::CONN:0:") == 0);
}
