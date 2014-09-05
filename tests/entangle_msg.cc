#include <iostream>

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

TEST_CASE("entangle|msg_types-drop") {
	auto req = entangle::EntangleMessageDropRequest(182, "aEiOOf", "pw");
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:DROP:0:") == 0);

	auto res = entangle::EntangleMessageDropResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::DROP:0:") == 0);
}

TEST_CASE("entangle|msg_types-resize") {
	auto req = entangle::EntangleMessageResizeRequest(182, "aEiOOf", "pw", 100);
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:RESIZE:0:100") == 0);

	auto res = entangle::EntangleMessageResizeResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::RESIZE:0:") == 0);
}

TEST_CASE("entangle|msg_types-sync") {
	auto req = entangle::EntangleMessageSyncRequest(182, "aEiOOf", "pw", 0);
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:SYNC:0:0") == 0);

	auto res = entangle::EntangleMessageSyncResponse(182, "aEiOOf", 1, 2, "foo");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::SYNC:0:1:2:foo") == 0);

	auto res_ack = entangle::EntangleMessageSyncResponseAck(182, "aEiOOf", "pw");
	REQUIRE(res_ack.to_string().compare("1:182:aEiOOf:pw:SYNC:0:") == 0);
}

TEST_CASE("entangle|msg_types-seek") {
	auto req = entangle::EntangleMessageSeekRequest(182, "aEiOOf", "pw", 0, 1, 100);
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:SEEK:0:0:1:100") == 0);

	auto res = entangle::EntangleMessageSeekResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::SEEK:0:") == 0);
}

TEST_CASE("entangle|msg_types-overwrite") {
	auto req = entangle::EntangleMessageOverwriteRequest(182, "aEiOOf", "pw", "some data");
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:OVER:0:some data") == 0);

	auto res = entangle::EntangleMessageOverwriteResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::OVER:0:") == 0);
}

TEST_CASE("entangle|msg_types-insert") {
	auto req = entangle::EntangleMessageInsertRequest(182, "aEiOOf", "pw", "some data");
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:INSERT:0:some data") == 0);

	auto res = entangle::EntangleMessageInsertResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::INSERT:0:") == 0);
}

TEST_CASE("entangle|msg_types-erase") {
	auto req = entangle::EntangleMessageEraseRequest(182, "aEiOOf", "pw", 100);
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:ERASE:0:100") == 0);

	auto res = entangle::EntangleMessageEraseResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::ERASE:0:") == 0);
}

TEST_CASE("entangle|msg_types-backspace") {
	auto req = entangle::EntangleMessageBackspaceRequest(182, "aEiOOf", "pw", 100);
	REQUIRE(req.to_string().compare("0:182:aEiOOf:pw:BACK:0:100") == 0);

	auto res = entangle::EntangleMessageBackspaceResponse(182, "aEiOOf");
	REQUIRE(res.to_string().compare("1:182:aEiOOf::BACK:0:") == 0);
}
