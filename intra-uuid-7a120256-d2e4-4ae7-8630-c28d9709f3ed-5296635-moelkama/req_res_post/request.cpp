#include "request.hpp"
#include "post.hpp"

std::string request::get_method() const
{
    return (this->method);
}

std::string request::get_path() const
{
    return (this->path);
}

std::string request::get_version() const
{
    return (this->version);
}

std::string request::get_body() const
{
    return (this->body);
}

std::string request::get_header(const std::string& key) const
{
    try
    {
        return (headers.at(key));
    }
    catch(const std::exception& e)
    {
        return ("");
    }
}

void    request::encoded()
{
    std::string hex;
    char        c;

    for (size_t i = 0; this->path[i]; i++)
    {
        if (this->path[i] == '%')
        {
            hex = this->path.substr(i + 1, 2);
            c = hex_to_dec(hex);
            this->path = this->path.substr(0, i) + c + this->path.substr(i + 3);
            i -= 2;
        }
    }
}

void    request::check_path()
{
    this->encoded();
    if (this->path.empty())
        throw (400);
    if (this->path.length() >= URL_MAX)
        throw (414);
    for (size_t i = 0; this->path[i]; i++)
    {
        if (std::string(URL_CHARACTERS).find(this->path[i]) == std::string::npos)
            throw (400);
    }
}

void    request::check_method()
{
    if (method.empty())
        throw (400);
}

void    request::request_line_parse(std::string req_line)
{
    size_t      ispace;

    ispace = req_line.find(" ");
    if (ispace == std::string::npos)
        throw (400);
    this->method = req_line.substr(0, ispace);
    req_line = req_line.substr(ispace);
    req_line = req_line.substr(req_line.find_first_not_of(" "));
    //------------------------------

    ispace = req_line.find(" ");
    if (ispace == std::string::npos)
        throw (400);
    this->path = req_line.substr(0, ispace);
    req_line = req_line.substr(ispace);
    req_line = req_line.substr(req_line.find_first_not_of(" "));
    //---------------------------------

    this->version = req_line.substr(0, req_line.find(" "));
    req_line = req_line.substr(this->version.length());
    if (!req_line.empty() && req_line.find_first_not_of(' ') != std::string::npos)
        throw (400);
    check_method();
    check_path();
    if (this->version != HTTP_VERSION)
        throw (505);
}

void    request::headers_parse(std::string header)
{
    std::string key;
    std::string value;
    size_t      colone;

    colone = header.find(HEDER_SEPARATOR);
    if (colone == std::string::npos)
        throw (400);
    key = header.substr(0, colone);
    value = header.substr(colone + strlen(HEDER_SEPARATOR));
    if (key.empty())
        throw (400);
    if (value.empty())
        throw (400);
    this->headers[key] = value;
}

bool    request::parse_request(std::string buffer, ssize_t  size)
{
    std::string line;
    size_t      start_body;
    size_t      end_line;

    buffer = this->rest + buffer;
    this->rest = "";
    if (this->start_read)
        return (this->body = buffer, this->start_read);
    start_body = buffer.find(BODY_SEPARATOR);
    if (start_body != std::string::npos)
    {
        this->body = buffer.substr(start_body + strlen(BODY_SEPARATOR));
        buffer = buffer.substr(0, start_body + strlen(LINE_SEPARATOR));
        this->start_read = true;
    }
    if (!this->start_read && size < BUFFER_SIZE)
        throw (400);
    while ((end_line = buffer.find(LINE_SEPARATOR)) != std::string::npos)
    {
        line = buffer.substr(0, end_line);
        if (this->index_line == 0)
            this->request_line_parse(line);
        else
            this->headers_parse(line);
        buffer = buffer.substr(end_line + strlen(LINE_SEPARATOR));
        this->index_line++;
    }
    this->rest += buffer;
    if (this->rest.length() > 2 * URL_MAX)
        throw (400);
    return (this->start_read);
}

void    request::set_cgi_script(std::string script)
{
    this->cgi_script = script;
}

std::string request::get_cgi_script() const
{
    return (this->cgi_script);
}

void    request::set_path(std::string new_path)
{
    this->path = new_path;
}