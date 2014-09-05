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
			EntangleMessageResizeRequest(size_t msg_id, std::string client_id, std::string auth, size_t arg);
	};
	class EntangleMessageResizeResponse : public EntangleMessageResponse {
		public:
			EntangleMessageResizeResponse(size_t msg_id, std::string client_id);
	};

	class EntangleMessageSyncRequest : public EntangleMessage {
		public:
			EntangleMessageSyncRequest(size_t msg_id, std::string client_id, std::string auth, bool arg);
	};
	class EntangleMessageSyncResponse : public EntangleMessageResponse {
		public:
			EntangleMessageSyncResponse(size_t msg_id, std::string client_id, size_t buff_offset, size_t client_offset, std::string diff);
	};
	class EntangleMessageSyncResponseAck : public EntangleMessage {
		public:
			EntangleMessageSyncResponseAck(size_t msg_id, std::string client_id, std::string auth);
	};
}

#endif
