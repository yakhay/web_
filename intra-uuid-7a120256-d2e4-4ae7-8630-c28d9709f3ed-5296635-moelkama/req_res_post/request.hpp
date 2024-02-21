#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "macro.hpp"
#include <iostream>
#include <cstring>
#include <map>

class   request
{
private:
    std::string                         cgi_script;
    std::string                         method;
    std::string                         path;
    std::string                         version;
    std::map<std::string, std::string>  headers;
    std::string                         body;
    size_t                              index_line;
    std::string                         rest;
    bool                                start_read;
    void                                encoded();
    void                                check_path();
    void                                check_method();
public:
    request(){this->index_line = 0;this->start_read = false;}
    void    headers_parse(std::string header);
    void    request_line_parse(std::string req_line);
    bool    parse_request(std::string buffer, ssize_t size);
    void    set_cgi_script(std::string script);
    std::string get_cgi_script() const;
    std::string get_header(const std::string& key) const;
    std::string get_method() const;
    std::string get_path() const;
    std::string get_version() const;
    std::string get_body() const;
    void    set_path(std::string new_path);
};

#endif