#include <string>
#include <vector>

#include "src/msg_types.h"

entangle::EntangleMessageConnectRequest::EntangleMessageConnectRequest(size_t msg_id, std::string auth, std::string tail) : entangle::EntangleMessage(false, msg_id, "", auth, entangle::EntangleMessage::cmd_connect, entangle::EntangleMessage::error_no_err, std::vector<std::string> (), tail) {}
entangle::EntangleMessageConnectResponse::EntangleMessageConnectResponse(size_t msg_id, std::string client_id) : entangle::EntangleMessage(true, msg_id, client_id, "", entangle::EntangleMessage::cmd_connect) {}

entangle::EntangleMessageDropRequest::EntangleMessageDropRequest(size_t msg_id, std::string client_id, std::string auth) : entangle::EntangleMessage(false, msg_id, client_id, auth, entangle::EntangleMessage::cmd_drop) {}
entangle::EntangleMessageDropResponse::EntangleMessageDropResponse(size_t msg_id, std::string client_id) : entangle::EntangleMessage(true, msg_id, client_id, "", entangle::EntangleMessage::cmd_drop) {}
