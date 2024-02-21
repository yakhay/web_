#ifndef RESPONS_HPP
#define RESPONS_HPP

#include "../config_file/configFile.hpp"
#include "MimeType.hpp"

class post;

class respons
{
private:
    std::pair<int, std::string>         Status;
    std::string                         Body;
    std::map<std::string, std::string>  headers;
public:
    respons();
    respons(const respons& other){*this = other;}
    respons& operator=(const respons& other){this->Status = other.Status;this->Body = other.Body; this->headers = other.headers;return (*this);}
    ~respons();
    void    set_Body(std::string body);
    void    set_header(std::string key, std::string value);
    void    set_Status(int st, const one_server& server);
    std::string prepare_respons(bool b);
    void    set_post_info(const post& p);
};

#endif