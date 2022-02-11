#include "resource_servlet.h"
#include <flexy/util/log.h>


namespace flexy::http {

static auto& g_logger = FLEXY_LOG_ROOT();

ResourceServlet::ResourceServlet(const std::string& path) : Servlet("ResourceServlet"), path_(path) {

}

int32_t ResourceServlet::handle(const HttpRequest::ptr& request, const HttpResponse::ptr& response, const HttpSession::ptr& session) {
    auto path = path_ + "/" + request->getPath();
    FLEXY_LOG_INFO(g_logger) << "handle path = " << path;
    if (path.find("..") != std::string::npos) {
        response->setBody("invalid path");
        response->setStatus(HttpStatus::NOT_FOUND);
        return 0;
    }
    std::ifstream ifs(path);
    if (!ifs) {
        response->setBody("invalid path");
        response->setStatus(HttpStatus::NOT_FOUND);
        return 0;
    }

    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) {
        ss << line << std::endl;
    }

    response->setBody(ss.str());
    response->setHeader("Content-type", "text/html;charset=utf-8");
    return 0;
}

}