#include "protocol.h"
#include <json/json.h>

namespace chat {

ChatMessage::ptr ChatMessage::Create(const std::string& v) {
    Json::Value node;
    Json::Reader reader;
    if (!reader.parse(v, node)) {
        return nullptr;
    }
    ChatMessage::ptr rt = std::make_shared<ChatMessage>();
    auto names = node.getMemberNames();
    for (auto& name : names) {
        rt->datas_[name] = node[name].asString();
    }
    return rt;
}

ChatMessage::ChatMessage() {
}

std::string ChatMessage::get(const std::string& name) {
    auto it = datas_.find(name);
    return it == datas_.end() ? "" : it->second;
}

void ChatMessage::set(const std::string& name, const std::string& val) {
    datas_[name] = val;
}

std::string ChatMessage::toString() const {
    Json::Value json;
    for (auto& [name, value] : datas_) {
        json[name] = value;
    }
    Json::FastWriter writer;
    return writer.write(json);
}

}