#ifndef _ENTANGLE_MSG_TYPES_H
#define _ENTANGLE_MSG_TYPES_H

#include "src/entangle_msg.h"

namespace entangle {
	class EntangleMessageResponse : public EntangleMessage {
		public:
			EntangleMessageResponse(size_t msg_id, std::string client_id, std::string cmd, size_t err = entangle::EntangleMessage::error_no_err, std::string arg = "");
	};

	/**
	 * CONNECT
	 */
	class EntangleMessageConnectRequest : public EntangleMessage {
		public:
			EntangleMessageConnectRequest(size_t msg_id, std::string auth, std::string tail);
	};
	class EntangleMessageConnectResponse : public EntangleMessageResponse {
		public:
			EntangleMessageConnectResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * DROP
	 */
	class EntangleMessageDropRequest : public EntangleMessage {
		public:
			EntangleMessageDropRequest(size_t msg_id, std::string client_id, std::string auth);
	};
	class EntangleMessageDropResponse : public EntangleMessageResponse {
		public:
			EntangleMessageDropResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * RESIZE
	 */
	class EntangleMessageResizeRequest : public EntangleMessage {
		public:
			EntangleMessageResizeRequest(size_t msg_id, std::string client_id, std::string auth, size_t arg);
	};
	class EntangleMessageResizeResponse : public EntangleMessageResponse {
		public:
			EntangleMessageResizeResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * SYNC
	 */
	class EntangleMessageSyncRequest : public EntangleMessage {
		public:
			EntangleMessageSyncRequest(size_t msg_id, std::string client_id, std::string auth, bool arg);
	};
	class EntangleMessageSyncResponse : public EntangleMessageResponse {
		public:
			EntangleMessageSyncResponse(size_t msg_id, std::string client_id, std::string diff);
	};
	class EntangleMessageSyncResponseAck : public EntangleMessage {
		public:
			EntangleMessageSyncResponseAck(size_t msg_id, std::string client_id, std::string auth);
	};

	/**
	 * SYNCPOS
	 */
	class EntangleMessageSyncPosResponse : public EntangleMessageResponse {
		public:
			EntangleMessageSyncPosResponse(size_t msg_id, std::string client_id, std::string client_pos_id, size_t client_pos_offset);
	};
	class EntangleMessageSyncPosResponseAck : public EntangleMessage {
		public:
			EntangleMessageSyncPosResponseAck(size_t msg_id, std::string client_id, std::string auth);
	};

	/**
	 * SEEK
	 */
	class EntangleMessageSeekRequest : public EntangleMessage {
		public:
			EntangleMessageSeekRequest(size_t msg_id, std::string client_id, std::string auth, bool is_relative, bool is_forward, size_t offset);
	};
	class EntangleMessageSeekResponse : public EntangleMessageResponse {
		public:
			EntangleMessageSeekResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * OVERWRITE
	 */
	class EntangleMessageOverwriteRequest : public EntangleMessage {
		public:
			EntangleMessageOverwriteRequest(size_t msg_id, std::string client_id, std::string auth, std::string data);
	};
	class EntangleMessageOverwriteResponse : public EntangleMessageResponse {
		public:
			EntangleMessageOverwriteResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * INSERT
	 */
	class EntangleMessageInsertRequest : public EntangleMessage {
		public:
			EntangleMessageInsertRequest(size_t msg_id, std::string client_id, std::string auth, std::string data);
	};
	class EntangleMessageInsertResponse : public EntangleMessageResponse {
		public:
			EntangleMessageInsertResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * ERASE
	 */
	class EntangleMessageEraseRequest : public EntangleMessage {
		public:
			EntangleMessageEraseRequest(size_t msg_id, std::string client_id, std::string auth, size_t size);
	};
	class EntangleMessageEraseResponse : public EntangleMessageResponse {
		public:
			EntangleMessageEraseResponse(size_t msg_id, std::string client_id);
	};

	/**
	 * BACKSPACE
	 */
	class EntangleMessageBackspaceRequest : public EntangleMessage {
		public:
			EntangleMessageBackspaceRequest(size_t msg_id, std::string client_id, std::string auth, size_t size);
	};
	class EntangleMessageBackspaceResponse : public EntangleMessageResponse {
		public:
			EntangleMessageBackspaceResponse(size_t msg_id, std::string client_id);
	};
}

#endif
