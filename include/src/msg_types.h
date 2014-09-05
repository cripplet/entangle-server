#ifndef _ENTANGLE_MSG_TYPES_H
#define _ENTANGLE_MSG_TYPES_H

#include "src/entangle_msg.h"

namespace entangle {
	class EntangleMessageResponse : public EntangleMessage {
		public:
			EntangleMessageResponse(size_t msg_id, std::string client_id, std::string cmd, size_t err = entangle::EntangleMessage::error_no_err, std::string arg = "");
	};

	class EntangleMessageConnectRequest : public EntangleMessage {
		public:
			EntangleMessageConnectRequest(size_t msg_id, std::string auth, std::string tail);
	};
	class EntangleMessageConnectResponse : public EntangleMessageResponse {
		public:
			EntangleMessageConnectResponse(size_t msg_id, std::string client_id);
	};

	class EntangleMessageDropRequest : public EntangleMessage {
		public:
			EntangleMessageDropRequest(size_t msg_id, std::string client_id, std::string auth);
	};
	class EntangleMessageDropResponse : public EntangleMessageResponse {
		public:
			EntangleMessageDropResponse(size_t msg_id, std::string client_id);
	};

	class EntangleMessageResizeRequest : public EntangleMessage {
		public:
			EntangleMessageResizeRequest(size_t msg_id, std::string client_id, std::string auth, size_t buffer_size);
	};
	class EntangleMessageResizeResponse : public EntangleMessageResponse {
		public:
			EntangleMessageResizeResponse(size_t msg_id, std::string client_id);
	};
}

#endif
