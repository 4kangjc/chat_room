#pragma once

#include <flexy/http/servlet.h>

namespace flexy::http {

class ResourceServlet : public Servlet {
public:
    using ptr = std::shared_ptr<ResourceServlet>;
    ResourceServlet(const std::string& path);
    virtual int32_t handle(const HttpRequest::ptr& request, const HttpResponse::ptr& response, const HttpSession::ptr&) override;
private:
    std::string path_;
};

}