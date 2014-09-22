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

TEST_CASE("entangle|dopt_node-bind") {
	auto s = entangle::OTNode(8000, 1);
	auto x = entangle::OTNode(8050, 1);

	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());

	REQUIRE(s.ins(0, '1') == false);
	REQUIRE(x.join("localhost", 8000) == false);

	REQUIRE(s.bind("") == true);
	REQUIRE(s.bind("") == false);
	REQUIRE(s.ins(0, '1') == true);
	REQUIRE(x.join("localhost", 8000) == true);
	sleep(1);
	REQUIRE(x.drop("localhost", 8000) == true);

	REQUIRE(x.bind("") == true);
	REQUIRE(x.join("localhost", 8000) == false);

	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
}

TEST_CASE("entangle|dopt_node-join") {
	auto s = entangle::OTNode(8000, 1);
	auto x = entangle::OTNode(8050, 0);
	auto y = entangle::OTNode(8051, 0);

	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());
	REQUIRE_NOTHROW(y.up());

	REQUIRE(s.bind("") == true);

	// connection limits
	REQUIRE(x.join("localhost", 8000) == true);
	REQUIRE(y.join("localhost", 8000) == false);
	REQUIRE(s.size() == 1);
	REQUIRE(s.get_context().compare("") == 0);
	REQUIRE(s.get_context().compare(x.get_context()) == 0);
	REQUIRE(x.drop("localhost", 8000) == true);

	// sync data
	REQUIRE(s.ins(0, '1') == true);
	sleep(1);
	REQUIRE(x.join("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.get_context().compare("1") == 0);
	REQUIRE(s.get_context().compare(x.get_context()) == 0);
	REQUIRE(x.drop("localhost", 8000) == true);
	sleep(1);
	REQUIRE(x.get_context().compare("") == 0);
	REQUIRE(s.size() == 0);
	REQUIRE(x.size() == 0);

	// cannot join when local context is modified
	REQUIRE(x.bind("") == true);
	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	REQUIRE(x.get_context().compare("1") == 0);
	REQUIRE(x.join("localhost", 8000) == false);

	// propagated drops on multi-layered topologies
	REQUIRE(x.del(0) == true);
	sleep(1);
	REQUIRE(x.get_context().compare("") == 0);
	REQUIRE(x.free() == true);
	REQUIRE(x.join("localhost", 8000) == true);
	REQUIRE(y.join("localhost", 8050) == true);
	sleep(1);
	REQUIRE(s.size() == 1);
	REQUIRE(x.size() == 2);
	REQUIRE(y.size() == 1);
	REQUIRE(x.drop("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 0);
	REQUIRE(x.size() == 0);
	REQUIRE(y.size() == 0);

	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
	REQUIRE_NOTHROW(y.dn());
}

TEST_CASE("entangle|dopt_node-convergence") {
	auto s = entangle::OTNode(8000, 100);
	auto x = entangle::OTNode(8050, 2);

	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());

	REQUIRE(s.bind("") == true);

	REQUIRE(x.join("localhost", 8000) == true);
	sleep(1);

	CHECK(s.get_context().compare("") == 0);
	CHECK(x.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
	CHECK(x.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("11") == 0);
	CHECK(x.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.ins(0, '2') == true);
	sleep(1);
	CHECK(s.get_context().compare("211") == 0);
	CHECK(x.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.del(1) == true);
	sleep(1);
	CHECK(s.get_context().compare("21") == 0);
	CHECK(x.get_context().compare(s.get_context()) == 0);

	REQUIRE(s.del(0) == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
	CHECK(s.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.del(0) == true);
	sleep(1);
	CHECK(s.get_context().compare("") == 0);
	CHECK(s.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.drop("localhost", 8000) == true);
	sleep(1);
	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
}

TEST_CASE("entangle|dopt_node-daemon") {
	auto n = entangle::OTNode(8888, 100);
	auto m = entangle::OTNode(8889, 100);

	REQUIRE(n.bind("") == true);

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

TEST_CASE("entangle|dopt_node-topography") {
	auto s = entangle::OTNode(8000, 100);
	auto x = entangle::OTNode(8050, 2);
	auto y = entangle::OTNode(8051, 1);

	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());
	REQUIRE_NOTHROW(y.up());

	REQUIRE(s.bind("") == true);

	REQUIRE(x.join("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 1);

	// single node, single server topology
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

	// multi node, single server topology
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

	REQUIRE(x.drop("localhost", 8000) == true);
	REQUIRE(y.drop("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 0);
	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
	REQUIRE_NOTHROW(y.dn());

	// multi-layer tree topology
	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());
	REQUIRE_NOTHROW(y.up());
	REQUIRE(x.join("localhost", 8000) == true);
	REQUIRE(y.join("localhost", 8050) == true);
	REQUIRE(s.size() == 1);
	REQUIRE(x.size() == 2);

	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	REQUIRE(s.get_context().compare("1") == 0);
	REQUIRE(x.get_context().compare(s.get_context()) == 0);
	REQUIRE(y.get_context().compare(s.get_context()) == 0);

	REQUIRE(x.drop("localhost", 8000) == true);
	REQUIRE(y.drop("localhost", 8050) == true);
	sleep(1);
	REQUIRE(s.size() == 0);
	REQUIRE(x.size() == 0);
	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
	REQUIRE_NOTHROW(y.dn());
}

TEST_CASE("entangle|dopt_node-concurrent") {
	auto s = entangle::OTNode(8000, 100);
	auto x = entangle::OTNode(8050, 1);
	auto y = entangle::OTNode(8051, 1);

	REQUIRE_NOTHROW(s.up());
	REQUIRE_NOTHROW(x.up());
	REQUIRE_NOTHROW(y.up());

	REQUIRE(s.bind("") == true);

	REQUIRE(x.join("localhost", 8000) == true);
	REQUIRE(y.join("localhost", 8000) == true);
	sleep(1);
	REQUIRE(s.size() == 2);

	// concurrent update checking
	REQUIRE(x.ins(0, '1') == true);
	REQUIRE(y.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
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

	REQUIRE(x.join("localhost", 8000) == true);
	REQUIRE(y.join("localhost", 8000) == true);
	sleep(1);

	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("11") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.del(0) == true);
	REQUIRE(x.del(0) == true);
	sleep(1);
	CHECK(s.get_context().compare("") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.drop("localhost", 8000) == true);
	REQUIRE(y.drop("localhost", 8000) == true);
	sleep(1);

	REQUIRE(x.join("localhost", 8000) == true);
	REQUIRE(y.join("localhost", 8000) == true);
	sleep(1);

	REQUIRE(x.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("1") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(y.ins(0, '1') == true);
	sleep(1);
	CHECK(s.get_context().compare("11") == 0);
	CHECK(s.get_context().compare(x.get_context()) == 0);
	CHECK(s.get_context().compare(y.get_context()) == 0);

	REQUIRE(x.drop("localhost", 8000) == true);
	REQUIRE(y.drop("localhost", 8000) == true);
	sleep(1);

	REQUIRE_NOTHROW(s.dn());
	REQUIRE_NOTHROW(x.dn());
	REQUIRE_NOTHROW(y.dn());
}
