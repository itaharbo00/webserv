#ifndef ROUTER_POST_HPP
#define ROUTER_POST_HPP

#include "Router.hpp"

class RouterPOST
{
public:
    RouterPOST(const Router *router);
    ~RouterPOST();

    // Méthodes POST principales
    HttpResponse handlePost(const HttpRequest &request, const LocationConfig *location);
    HttpResponse handleDelete(const HttpRequest &request);

    // Upload de fichiers
    std::string saveUploadedFile(const std::string &body, const std::string &uploadDir);
    bool createDirectoryIfNotExists(const std::string &dirPath);

private:
    const Router *p_router;

    // Méthodes internes
    bool validatePostRequest(const HttpRequest &request);
    std::string generateUniqueFilename(const std::string &uploadDir);
    HttpResponse createSuccessResponse(const HttpRequest &request,
                                       const std::string &message);
};

#endif