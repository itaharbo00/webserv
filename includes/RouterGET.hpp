#ifndef ROUTER_GET_HPP
#define ROUTER_GET_HPP

#include "Router.hpp"

class RouterGET
{
public:
    RouterGET(const Router *router);
    ~RouterGET();

    // Méthodes GET principales
    HttpResponse serveStaticFile(const HttpRequest &request);

    // Autoindex
    std::string generateDirectoryListing(const std::string &dirPath, const std::string &uri);

    // Utilitaires de fichiers
    std::string readFile(const std::string &filePath);
    bool fileExists(const std::string &filePath) const;
    bool isRegularFile(const std::string &filePath) const;
    bool isDirectory(const std::string &filePath) const;
    bool isFileTooLarge(const std::string &filePath) const;
    std::string getFileExtension(const std::string &filename) const;
    std::string getMimeType(const std::string &extension) const;

private:
    const Router *p_router;

    // Méthodes internes
    std::string formatFileSize(size_t size) const;
    std::string formatFileTime(time_t mtime) const;
    std::string getDirectoryListingHTML(const std::string &dirPath,
                                        const std::string &uri) const;
};

#endif