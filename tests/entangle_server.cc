#include <csignal>
#include <cstdlib>
#include <memory>
#include <unistd.h>

#include <iostream>

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

TEST_CASE("entangle|entangle_server-drop") {
	auto m = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 10, 9999, "test-server"));
	auto c = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8888));
	auto tm = std::thread(&entangle::EntangleServer::up, &*m);
	auto tc = std::thread(&msgpp::MessageNode::up, &*c);
	entangle::EntangleMessage msg;
	sleep(1);
	while(!m->get_status());

	c->push(entangle::EntangleMessageConnectRequest(1, "abcde", "localhost", 8888, "test-server").to_string(), "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 1);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull()));
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_no_err);
	REQUIRE(msg.get_client_id().compare("") != 0);

	std::string client_id = msg.get_client_id();

	// wrong auth token
	c->push(entangle::EntangleMessageDropRequest(0, client_id, "aaaaa").to_string(), "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 2);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull()));
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_denied);
	REQUIRE(msg.get_cmd().compare("DROP") == 0);
	REQUIRE(msg.get_client_id().compare(client_id) == 0);

	raise(SIGINT);
	tc.join();
	tm.join();
}

TEST_CASE("entangle|entangle_server-conn") {
	auto m = std::shared_ptr<entangle::EntangleServer> (new entangle::EntangleServer("tests/files/server-init", 1, 9999, "test-server"));
	auto c = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8888));
	auto e = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(7777));
	auto tm = std::thread(&entangle::EntangleServer::up, &*m);
	auto tc = std::thread(&msgpp::MessageNode::up, &*c);
	auto te = std::thread(&msgpp::MessageNode::up, &*e);
	entangle::EntangleMessage msg;
	sleep(1);
	while(!m->get_status());

	// drop nonsense packets
	c->push("random nonsense", "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 1);
	REQUIRE_THROWS_AS(c->pull(), exceptionpp::RuntimeError);

	// drop wrong port
	c->push("0:0::abcde:CONN:0:localhost:aaaa:test-server", "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 2);
	REQUIRE_THROWS_AS(c->pull(), exceptionpp::RuntimeError);

	// wrong command
	c->push("0:0::abcde:INVALID_CMD:0:localhost:8888:test-server", "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 3);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull()));
	REQUIRE(msg.get_cmd().compare("INVALID_CMD") == 0);
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_unimpl);
	REQUIRE(msg.get_client_id().compare("") == 0);

	// wrong server token
	c->push(entangle::EntangleMessageConnectRequest(0, "abcde", "localhost", 8888).to_string(), "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 4);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull()));
	REQUIRE(msg.get_cmd().compare("CONN") == 0);
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_denied);
	REQUIRE(msg.get_client_id().compare("") == 0);

	// correct data
	c->push(entangle::EntangleMessageConnectRequest(0, "abcde", "localhost", 8888, "test-server").to_string(), "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 5);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull()));
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_no_err);
	REQUIRE(msg.get_client_id().compare("") != 0);

	// double registry
	c->push(entangle::EntangleMessageConnectRequest(0, "abcde", "localhost", 8888, "test-server").to_string(), "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 6);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(c->pull()));
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_invalid);
	REQUIRE(msg.get_client_id().compare("") == 0);

	// max conn
	e->push(entangle::EntangleMessageConnectRequest(0, "abcde", "localhost", 7777, "test-server").to_string(), "localhost", m->get_port());
	sleep(1);
	REQUIRE(m->get_count() == 7);
	REQUIRE_NOTHROW(msg = entangle::EntangleMessage(e->pull()));
	REQUIRE(msg.get_err() == entangle::EntangleMessage::error_max_conn);
	REQUIRE(msg.get_client_id().compare("") == 0);

	raise(SIGINT);
	tc.join();
	te.join();
	tm.join();
}
