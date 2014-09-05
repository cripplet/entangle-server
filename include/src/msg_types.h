#ifndef _ENTANGLE_MSG_TYPES_H
#define _ENTANGLE_MSG_TYPES_H

#include "src/entangle_msg.h"

namespace entangle {
	class EntangleMessageConnectRequest : public EntangleMessage {
		public:
			EntangleMessageConnectRequest(size_t msg_id, std::string auth, std::string tail);
	};
	class EntangleMessageConnectResponse : public EntangleMessage {
		public:
			EntangleMessageConnectResponse(size_t msg_id, std::string client_id);
	};
	class EntangleMessageDropRequest : public EntangleMessage {
		public:
			EntangleMessageDropRequest(size_t msg_id, std::string client_id, std::string auth);
	};
	class EntangleMessageDropResponse : public EntangleMessage {
		public:
			EntangleMessageDropResponse(size_t msg_id, std::string client_id);
	};
}

#endif
