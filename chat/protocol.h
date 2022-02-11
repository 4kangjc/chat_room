#pragma once

#include <string>
#include <map>
#include <memory>

namespace chat {

class ChatMessage {
public:
    using ptr = std::shared_ptr<ChatMessage>;
    static ChatMessage::ptr Create(const std::string& v);
    ChatMessage();
    std::string get(const std::string& name);
    void set(const std::string& name, const std::string& val);
    std::string toString() const;
private:
    std::map<std::string, std::string> datas_;
};

}