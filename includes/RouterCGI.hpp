#ifndef ROUTER_CGI_HPP
#define ROUTER_CGI_HPP

#include "Router.hpp"

class RouterCGI
{
public:
    RouterCGI(const Router *router);
    ~RouterCGI();

    // Méthode principale CGI
    HttpResponse executeCgi(const HttpRequest &request, const LocationConfig *location,
                            const std::string &scriptPath);

    // Validation et setup
    bool validateCgiRequest(const std::string &scriptPath,
                            const std::string &extension, const LocationConfig *location,
                            std::string &cgiPath) const;

    // Construction de l'environnement CGI
    char **buildCgiEnv(const HttpRequest &request, const LocationConfig *location,
                       const std::string &scriptPath);

    // Utilitaires
    std::string getCgiExtension(const std::string &filePath) const;
    void freeCgiEnv(char **env) const;

private:
    const Router *p_router;

    // Méthodes internes d'environnement
    void buildBasicCgiEnv(std::vector<std::string> &env, const HttpRequest &request,
                          const std::string &scriptPath) const;
    void buildServerCgiEnv(std::vector<std::string> &env, const HttpRequest &request) const;
    void buildContentCgiEnv(std::vector<std::string> &env, const HttpRequest &request) const;
    void buildHttpHeadersCgiEnv(std::vector<std::string> &env, const HttpRequest &request) const;
    char **convertEnvToArray(const std::vector<std::string> &env) const;

    // Exécution CGI
    bool setupCgiPipes(int pipe_in[2], int pipe_out[2]) const;
    void executeCgiChild(int pipe_in[2], int pipe_out[2], const std::string &cgiPath,
                         const std::string &scriptPath, char **env) const;
    std::string executeCgiParent(int pipe_in[2], int pipe_out[2], pid_t pid,
                                 const HttpRequest &request) const;
    void parseCgiOutput(const std::string &cgiOutput, std::string &headers,
                        std::string &body) const;
    HttpResponse buildCgiResponse(const std::string &cgiHeaders, const std::string &cgiBody,
                                  const HttpRequest &request) const;
};

#endif