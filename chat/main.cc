#include <flexy/env/application.h>
#include <flexy/env/env.h>
#include <flexy/util/log.h>
#include "resource_servlet.h"
#include "chat_servlet.h"
#include "protocol.h"

static auto& g_logger = FLEXY_LOG_ROOT();

int main(int argc, char** argv) {
    flexy::Application app;
    if (!app.init(argc, argv)) {
        return -1;
    }
    app.serverReady = [&app]() {
        FLEXY_LOG_INFO(g_logger) << "onServerReady";
        auto svrs = app.getServer<flexy::http::HttpServer>();
        if (svrs.empty()) {
            FLEXY_LOG_ERROR(g_logger) << "no httpserver alive";
            return;
        }

        for (auto& http_server : svrs) {
            auto slt_dispatch = http_server->getServletDispatch();

            auto slt = std::make_shared<flexy::http::ResourceServlet>(flexy::EnvMgr::GetInstance().getCwd());
            slt_dispatch->addGlobServlet("/html/*", slt);
            FLEXY_LOG_INFO(g_logger) << "addServlet";
        }

        auto ws_svrs = app.getServer<flexy::http::WSServer>();
        if (ws_svrs.empty()) {
            FLEXY_LOG_ERROR(g_logger) << "no websocket server alive";
            return;
        }

        for (auto& ws_server : ws_svrs) {
            auto slt_dispatch = ws_server->getWSServletDispatch();
            auto slt = std::make_shared<chat::ChatWSServlet>();
            slt_dispatch->addServlet("/flexy/chat", slt);
        }
    };
    return app.run();
}