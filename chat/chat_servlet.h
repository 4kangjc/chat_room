#pragma once

#include "flexy/http/ws_servlet.h"

namespace chat {

class ChatWSServlet : public flexy::http::WSServlet {
public:
    using ptr = std::shared_ptr<ChatWSServlet>;
    ChatWSServlet();
    virtual int32_t onConnect(const flexy::http::HttpRequest::ptr& header, const flexy::http::WSSession::ptr& session) override;
    virtual int32_t onClose(const flexy::http::HttpRequest::ptr& header, const flexy::http::WSSession::ptr& session) override;
    virtual int32_t handle(const flexy::http::HttpRequest::ptr& header, const flexy::http::WSFrameMessage::ptr& msg,
                           const flexy::http::WSSession::ptr& session) override;
};

}