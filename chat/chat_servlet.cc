#include "chat_servlet.h"
#include "protocol.h"
#include <flexy/thread/mutex.h>
#include <flexy/util/log.h>
#include <flexy/util/util.h>

using namespace flexy;

namespace chat {

static auto& g_logger = FLEXY_LOG_ROOT();

int32_t SendMessage(const http::WSSession::ptr& session, const ChatMessage::ptr& msg) {
    FLEXY_LOG_INFO(g_logger) << msg->toString() << " - " << session;
    return session->sendMessage(msg->toString()) > 0 ? 0 : 1;
}

class ChatRoom {
public:
    void del(const std::string& name) {
        WRITELOCK(mutex_);
        sessions_.erase(name);
    }
    void add(const std::string& name, const http::WSSession::ptr& session) {
        WRITELOCK(mutex_);
        sessions_[name] = session;
    }
    bool has(const std::string& name) {
        READLOCK(mutex_);
        return sessions_.count(name);
    }
    void notify(const ChatMessage::ptr& msg, const http::WSSession::ptr& session = nullptr) {
        ReadLock lock(mutex_);
        auto sessions = sessions_;
        lock.unlock();
        for (auto& [name, sess] : sessions) {
            if (sess == session) continue;
            SendMessage(sess, msg);
        }
    }
private:
    std::map<std::string, http::WSSession::ptr> sessions_;
    rw_mutex mutex_;
};

static ChatRoom chat_room;

ChatWSServlet::ChatWSServlet() : http::WSServlet("chat_servlet") {

}

int32_t ChatWSServlet::onConnect(const http::HttpRequest::ptr& header, const http::WSSession::ptr& session) {
    FLEXY_LOG_INFO(g_logger) << "onConnect " << session;   
    return 0;
}

int32_t ChatWSServlet::onClose(const http::HttpRequest::ptr& header, const http::WSSession::ptr& session) {
    auto id = header->getHeader("$id");
    FLEXY_LOG_INFO(g_logger) << "onClose " << session << " id = " << id;
    if (!id.empty()) {
        chat_room.del(id);
        auto nty_ptr = std::make_shared<ChatMessage>();
        auto& nty = *nty_ptr;
        nty["type"] = "user_leave";
        nty["time"] = TimeToStr();
        nty["name"] = id;
        chat_room.notify(nty_ptr);
    }
    return 0;
}

int32_t ChatWSServlet::handle(const http::HttpRequest::ptr& header, const http::WSFrameMessage::ptr& msgx,
               const http::WSSession::ptr& session) {
    FLEXY_LOG_INFO(g_logger) << "handle " << session << " opcode = " << msgx->getOpcode()
    << " data = " << msgx->getData();

    auto msg_ptr = ChatMessage::Create(msgx->getData());
    auto& msg = *msg_ptr;
    auto id = header->getHeader("$id");
    if (!msg_ptr) {
        if (!id.empty()) {
            chat_room.del(id);
        }
        return 1;
    }

    auto rsp_ptr = std::make_shared<ChatMessage>();
    auto& rsp = *rsp_ptr;
    auto type = msg["type"];
    if (type == "login_request") {
        rsp["type"] = "login_response";
        auto name = msg["name"];
        if (name.empty()) {
            rsp["result"] = "400";
            rsp["msg"] = "name is null";
            goto send;
        }
        if (!id.empty()) {
            rsp["result"] = "401";
            rsp["msg"] = "logined";
            goto send;
        }
        if (chat_room.has(name)) {
            rsp["result"] = "402";
            rsp["msg"] = "name exits";
            goto send;
        }
        id = name;
        header->setHeader("$id", id);
        rsp["result"] = "200";
        rsp["msg"] = "ok";
        chat_room.add(id, session);

        auto nty_ptr = std::make_shared<ChatMessage>();
        auto& nty = *nty_ptr;
        nty["type"] = "user_enter";
        nty["time"] = TimeToStr();
        nty["name"] = name;
        chat_room.notify(nty_ptr, session);
        goto send;
    } else if (type == "send_request") {
        rsp["type"] = "send_response";
        auto m = msg["msg"];
        if (m.empty()) {
            rsp["result"] = "500";
            rsp["msg"] = "msg is null";
            goto send;
        }
        if (id.empty()) {
            rsp["result"] = "501";
            rsp["msg"] = "msg is null";
            goto send;
        }

        rsp["result"] = "200";
        rsp["msg"] = "ok";

        auto nty_ptr = std::make_shared<ChatMessage>();
        auto& nty = *nty_ptr;
        nty["type"] = "msg";
        nty["time"] = TimeToStr();
        nty["name"] = id;
        nty["msg"] = m;
        chat_room.notify(nty_ptr);
        goto send;
    }

    return 0;
send:
    return SendMessage(session, rsp_ptr);
}

}