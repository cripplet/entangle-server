#include <csignal>
#include <iostream>
#include <memory>
#include <thread>
#include <unistd.h>

#include "libs/msgpp/msg_node.h"

#include "libs/giga/file.h"
#include "libs/giga/client.h"

int main() {
	auto s = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8000));
	auto c = std::shared_ptr<msgpp::MessageNode> (new msgpp::MessageNode(8088));

	auto ts = std::thread(&msgpp::MessageNode::up, &*s);
	auto tc = std::thread(&msgpp::MessageNode::up, &*c);

	s->push("from s", "localhost", c->get_port());
	sleep(1);

	std::cout << c->pull() << std::endl;

	raise(SIGINT);

	ts.join();
	tc.join();
}
