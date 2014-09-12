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

TEST_CASE("entangle|dopt_node-daemon") {
	auto n = entangle::OTNode(8888, 100);
	REQUIRE_NOTHROW(n.up());
	sleep(1);
	REQUIRE_NOTHROW(n.dn());

	// auto-call OTNode::dn on stack unwind
	REQUIRE_NOTHROW(n.up());
	sleep(1);
}
