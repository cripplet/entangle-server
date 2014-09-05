#include <stdexcept>
#include <string>
#include <vector>

#include "src/msg_types.h"

entangle::EntangleMessageResponse::EntangleMessageResponse(size_t msg_id, std::string client_id, std::string cmd, size_t err, std::string arg) : entangle::EntangleMessage(true, msg_id, client_id, "", cmd, err) {
	if(arg.compare("") != 0) {
		this->args.push_back(arg);
	}
}

entangle::EntangleMessageConnectRequest::EntangleMessageConnectRequest(size_t msg_id, std::string auth, std::string tail) : entangle::EntangleMessage(false, msg_id, "", auth, entangle::EntangleMessage::cmd_connect, entangle::EntangleMessage::error_no_err, std::vector<std::string> (), tail) {}
entangle::EntangleMessageConnectResponse::EntangleMessageConnectResponse(size_t msg_id, std::string client_id) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_connect) {}

entangle::EntangleMessageDropRequest::EntangleMessageDropRequest(size_t msg_id, std::string client_id, std::string auth) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_drop) {}
entangle::EntangleMessageDropResponse::EntangleMessageDropResponse(size_t msg_id, std::string client_id) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_drop) {}

entangle::EntangleMessageResizeRequest::EntangleMessageResizeRequest(size_t msg_id, std::string client_id, std::string auth, size_t arg) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_resize, entangle::EntangleMessage::error_no_err, std::vector<std::string> { std::to_string(arg) }) {}
entangle::EntangleMessageResizeResponse::EntangleMessageResizeResponse(size_t msg_id, std::string client_id) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_resize) {}

entangle::EntangleMessageSyncRequest::EntangleMessageSyncRequest(size_t msg_id, std::string client_id, std::string auth, bool arg) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_sync, entangle::EntangleMessage::error_no_err, std::vector<std::string> { std::to_string(arg) }) {}
entangle::EntangleMessageSyncResponse::EntangleMessageSyncResponse(size_t msg_id, std::string client_id, size_t buff_offset, size_t client_offset, std::string diff) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_sync, entangle::EntangleMessage::error_no_err) {
	this->args.push_back(std::to_string(buff_offset));
	this->args.push_back(std::to_string(client_offset));
	this->tail = diff;
}
entangle::EntangleMessageSyncResponseAck::EntangleMessageSyncResponseAck(size_t msg_id, std::string client_id, std::string auth) : entangle::EntangleMessage(true, msg_id, client_id, auth, entangle::EntangleMessage::cmd_sync) {}
