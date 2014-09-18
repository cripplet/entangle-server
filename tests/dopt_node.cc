#include <iostream>
#include <unistd.h>

#include "libs/catch/catch.hpp"

#include "src/dopt_node.h"

TEST_CASE("entangle|dopt_node-enc") {
	auto n = entangle::OTNode(8888, 100);
	entangle::upd_t u = { entangle::del, 100, 'c' };
	REQUIRE(n.enc_upd_t(u).compare("1:100:c") == 0);
	REQUIRE(n.cmp_upd_t({ entangle::del, 100, 'c'}, { entangle::del, 100, 'c' }) == true);
	REQUIRE(n.cmp_upd_t({ entangle::del, 100, 'c'}, { entangle::del, 100, 'd' }) == false);
	REQUIRE(n.cmp_upd_t(n.dec_upd_t("1:100:c"), { entangle::del, 100, 'c' }) == true);
}

TEST_CASE("entangle|dopt_node-ins-del") {
	auto s = entangle::OTNode(8000, 100);
	auto x = entangle::OTNode(8050, 1);
	auto y = entangle::OTNode(8051, 1);

	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());
	REQUIRE_NOTHROW(y.up());
	REQUIRE(x.join("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 1);

	/**
	 * single node, single server topology
	 */
	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);

	REQUIRE(x.del(0) == true);
	sleep(1);
	CHECK(s.get_context().compare("") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);

	REQUIRE(y.join("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 2);

	/**
	 * multi node, single server topology
	 */
	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.del(0) == true);
	sleep(1);
	CHECK(s.get_context().compare("") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	/**
	 * concurrent update checking
	 */
	std::cout << "sending x.ins" << std::endl;
	REQUIRE(x.ins(0, '1') == true);
	std::cout << "sending y.ins" << std::endl;
	REQUIRE(y.ins(0, '1') == true);
	sleep(1);
	std::cout << ":s " << s.get_context() << std::endl;
	CHECK(s.get_context().compare("1") == 0);
	std::cout << ":x " << x.get_context() << std::endl;
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.del(0) == true);
	REQUIRE(y.del(0) == true);
	sleep(1);
	CHECK(s.get_context().compare("") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.drop("localhost", 8000) == true);
	REQUIRE(y.drop("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 0);
	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
	REQUIRE_NOTHROW(y.dn());
}

TEST_CASE("entangle|dopt_node-daemon") {
	auto n = entangle::OTNode(8888, 100);
	auto m = entangle::OTNode(8889, 100);

	REQUIRE(m.join("localhost", 8888) == false);

	REQUIRE_NOTHROW(n.up());
	REQUIRE_NOTHROW(n.dn());

	REQUIRE_NOTHROW(n.up());
	REQUIRE_NOTHROW(m.up());
	REQUIRE(m.join("localhost", 8888) == true);
	sleep(1);
	REQUIRE(m.size() == 1);
	REQUIRE(n.size() == 1);
	REQUIRE(m.join("localhost", 8888) == false);
	REQUIRE(n.join("localhost", 8889) == false);
	REQUIRE(m.size() == 1);
	REQUIRE(n.size() == 1);
	REQUIRE(m.drop("localhost", 8888) == true);
	sleep(1);
	REQUIRE(m.size() == 0);
	REQUIRE(n.size() == 0);
	REQUIRE_NOTHROW(m.dn());
	REQUIRE_NOTHROW(n.dn());

	// auto-call OTNode::dn on stack unwind
	REQUIRE_NOTHROW(n.up());
	sleep(1);
}
