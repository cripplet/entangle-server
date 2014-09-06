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
entangle::EntangleMessageSyncResponse::EntangleMessageSyncResponse(size_t msg_id, std::string client_id, std::string diff) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_sync) {
	this->tail = diff;
}
entangle::EntangleMessageSyncResponseAck::EntangleMessageSyncResponseAck(size_t msg_id, std::string client_id, std::string auth) : entangle::EntangleMessage(true, msg_id, client_id, auth, entangle::EntangleMessage::cmd_sync) {}

entangle::EntangleMessageSyncPosResponse::EntangleMessageSyncPosResponse(size_t msg_id, std::string client_id, std::string client_pos_id, size_t client_pos_offset) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_syncpos) {
	this->args.push_back(client_pos_id);
	this->args.push_back(std::to_string(client_pos_offset));
}
entangle::EntangleMessageSyncPosResponseAck::EntangleMessageSyncPosResponseAck(size_t msg_id, std::string client_id, std::string auth) : entangle::EntangleMessage(true, msg_id, client_id, auth, entangle::EntangleMessage::cmd_syncpos) {}

entangle::EntangleMessageSeekRequest::EntangleMessageSeekRequest(size_t msg_id, std::string client_id, std::string auth, bool is_relative, bool is_forward, size_t offset) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_seek) {
	this->args.push_back(std::to_string(is_relative));
	this->args.push_back(std::to_string(is_forward));
	this->args.push_back(std::to_string(offset));
}
entangle::EntangleMessageSeekResponse::EntangleMessageSeekResponse(size_t msg_id, std::string client_id) : entangle::EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_seek) {}

entangle::EntangleMessageOverwriteRequest::EntangleMessageOverwriteRequest(size_t msg_id, std::string client_id, std::string auth, std::string data) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_overwrite) {
	this->tail = data;
}
entangle::EntangleMessageOverwriteResponse::EntangleMessageOverwriteResponse(size_t msg_id, std::string client_id) : EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_overwrite) {}

entangle::EntangleMessageInsertRequest::EntangleMessageInsertRequest(size_t msg_id, std::string client_id, std::string auth, std::string data) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_insert) {
	this->tail = data;
}
entangle::EntangleMessageInsertResponse::EntangleMessageInsertResponse(size_t msg_id, std::string client_id) : EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_insert) {}

entangle::EntangleMessageEraseRequest::EntangleMessageEraseRequest(size_t msg_id, std::string client_id, std::string auth, size_t size) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_erase) {
	this->args.push_back(std::to_string(size));
}
entangle::EntangleMessageEraseResponse::EntangleMessageEraseResponse(size_t msg_id, std::string client_id) : EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_erase) {}

entangle::EntangleMessageBackspaceRequest::EntangleMessageBackspaceRequest(size_t msg_id, std::string client_id, std::string auth, size_t size) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_backspace) {
	this->args.push_back(std::to_string(size));
}
entangle::EntangleMessageBackspaceResponse::EntangleMessageBackspaceResponse(size_t msg_id, std::string client_id) : EntangleMessageResponse(msg_id, client_id, entangle::EntangleMessage::cmd_backspace) {}
