#include "post.hpp"
#include "MimeType.hpp"
#include <algorithm>
#include <iostream>

std::string tostring(size_t n)
{
    std::stringstream ss;
    ss << n;
    std::string num = ss.str();
    return (num);
}

post::post()
{
    this->full_size = 0;
    this->clear = false;
    this->post_end = false;
    this->upload_path = "/tmp/";
    this->mode = 0;
    this->chunked_size = 0;
    this->max_size = 0;
    this->first_time = true;
    this->h = false;
    this->is_cgi = false;
}

post::post(const post& other)
{
    this->clear = false;
    this->post_end = false;
    this->upload_path = "/tmp/";
    this->mode = 0;
    this->chunked_size = 0;
    this->max_size = 0;
    this->first_time = true;
    this->h = false;
    this->is_cgi = false;
    *this = other;
}

post&   post::operator=(const post& other)
{
    this->upload_path = other.upload_path;
    this->mode = other.mode;
    this->chunked_size = other.chunked_size;
    this->first_time = other.first_time;
    this->h = other.h;
    this->is_cgi = other.is_cgi;
    return (*this);
}

post::~post()
{
    this->out.close();
    if (!this->clear)
    {
        for (std::vector<std::string>::iterator it = this->out_name.begin(); it != this->out_name.end(); it++)
            remove((this->upload_path + *it).c_str());
    }
}

const std::vector<std::string>& post::get_out_name() const
{
    return (this->out_name);
}

std::string post::get_upload_path() const
{
    return (this->upload_path);
}

std::string post::get_url() const
{
    return (this->url);
}

bool    post::final_check()
{
    size_t  global_size = 0;

    this->out.close();
    for (std::vector<std::string>::iterator it = this->out_name.begin(); it != this->out_name.end(); it++)
    {
        struct stat     file;
        if (stat((this->upload_path + "/" + *it).c_str(), &file) == -1)
            return (false);
        global_size += file.st_size;
    }
    if (global_size < this->full_size)
    {
        this->clear = false;
        return (false);
    }
    return (true);
}

void    post::create_file(std::string content_type)
{
    std::string     extension;
    struct timeval  Now;

    extension = mimetype.get(content_type, 0);
    gettimeofday(&Now, NULL);
    this->out_name.insert(this->out_name.end(), tostring(Now.tv_sec) + "_" + tostring(Now.tv_usec) + "." + extension);
    this->out.open(this->upload_path + this->out_name.back(), std::ios::out);
    if (this->out.is_open() == false)
        throw (500);
}

std::string post::set_headers(std::string body)
{
    std::string line;
    size_t      pos;
    size_t      hed;

    while (this->h && (pos = body.find(LINE_SEPARATOR)) != std::string::npos)
    {
        line = body.substr(0, pos);
        body = body.substr(pos + strlen(LINE_SEPARATOR));
        hed = line.find(HEDER_SEPARATOR);
        if (line.empty())
        {
            this->h = false;
            this->out.close();
            this->create_file(this->headers[CONTENT_TYPE]);
            break;
        }
        if (hed == std::string::npos)
            throw (400);
        this->headers[line.substr(0, hed)] = line.substr(hed + strlen(HEDER_SEPARATOR));
    }
    return (body);
}

void    post::boundarry(std::string body, size_t client_max_size)
{
    std::string start = "--" + this->boundary + "\r\n";
    std::string last = "--" + this->boundary + "--" + "\r\n";
    std::string tmp;
    size_t      exist;

    body = this->brest + body;
    while ((exist = body.find(start)) != std::string::npos)
    {
        tmp = this->set_headers(body.substr(0, exist));
        body = body.substr(exist + start.length());
        if (this->out.is_open())
        {
            this->max_size += tmp.substr(0, tmp.length() - strlen(LINE_SEPARATOR)).length();
            if (this->max_size > client_max_size)
                throw (413);
            this->out<<tmp.substr(0, tmp.length() - strlen(LINE_SEPARATOR));
        }
        this->h = true;
    }
    if ((exist = body.find(last)) != std::string::npos)
    {
        body = this->set_headers(body.substr(0, exist));
        if (this->out.is_open())
        {
            this->max_size += body.length() - strlen(LINE_SEPARATOR);
            if (this->max_size > client_max_size)
                throw (413);
            this->out<<body.substr(0, body.length() - strlen(LINE_SEPARATOR));
        }
        this->out.close();
        this->clear = true;
        this->full_size = this->max_size;
        throw (201);
    }
    body = this->set_headers(body);
    if (this->h)
    {
        this->brest = body;
        return ;
    }
    if (body.length() <= start.length())
    {
        this->brest = body;
        return ;
    }
    this->brest = body.substr(body.length() - start.length());
    this->max_size += body.length() - start.length();
    if (this->max_size > client_max_size)
        throw (413);
    this->out<<body.substr(0, body.length() - start.length());
}

int hexCharToInt(char hexChar)
{
    if (hexChar >= '0' && hexChar <= '9')
        return (hexChar - '0');
    else if (hexChar >= 'A' && hexChar <= 'F')
        return (hexChar - 'A' + 10);
    else if (hexChar >= 'a' && hexChar <= 'f')
        return (hexChar - 'a' + 10);
    throw (400);
}

size_t  hex_to_dec(std::string n)
{
    size_t  result;

    result = 0;
    for (size_t i = 0; i < n.length(); ++i)
        result = result * 16 + hexCharToInt(n[i]);
    return result;
}

void    post::chunked(std::string body, size_t client_max_size)
{
    size_t      hex_end;
    std::string n;
    std::string tmp;
    tmp = body;
    this->rest += body;
    body = "";
    if (this->chunked_size == 0)
    {
        hex_end = this->rest.find(LINE_SEPARATOR);
        if (hex_end == std::string::npos)
        {
            if (this->rest.length() >= 16)
                throw (400);
            return ;
        }
        n = this->rest.substr(0, hex_end);
        if (n.empty())
        {
            this->rest = this->rest.substr(hex_end + strlen(LINE_SEPARATOR));
            this->chunked("", client_max_size);
            return ;
        }
        this->chunked_size = hex_to_dec(n);
        if (this->mode == 1)
        {
            this->max_size += this->chunked_size;
            if (!this->is_cgi && this->max_size > client_max_size)
                throw (413);
        }
        if (this->chunked_size == 0)
        {
            this->out.close();
            this->clear = true;
            this->full_size = this->max_size;
            throw (201);
        }
        this->rest = this->rest.substr(hex_end + strlen(LINE_SEPARATOR));
    }
    if (this->chunked_size >= this->rest.length())
    {
        if (this->mode == 1 && this->is_cgi)
            this->raw(this->rest);
        else if (this->mode == 3)
            this->boundarry(this->rest, client_max_size);
        else
            this->out<<this->rest;
        this->chunked_size -= this->rest.length();
        this->rest = "";
    }
    else if (this->chunked_size)
    {
        if (this->mode == 1 && this->is_cgi)
            this->raw(this->rest.substr(0, this->chunked_size));
        else if (this->mode == 3)
            this->boundarry(this->rest.substr(0, this->chunked_size), client_max_size);
        else
            this->out<<this->rest.substr(0, this->chunked_size);
        this->rest = this->rest.substr(this->chunked_size);
        this->chunked_size = 0;
        this->chunked(body, client_max_size);
    }
}

void    post::raw(std::string content)
{
    if (this->mode == 4 || (this->mode == 1 && this->is_cgi))
    {
        size_t  pos;
        content = this->brest + content;
        if ((pos = content.find("--" + this->boundary + "--" + "\r\n")) != std::string::npos)
        {
            this->out<<content;
            this->out.close();
            this->clear = true;
            this->full_size = this->max_size;
            throw (201);
        }
        else
        {
            if (content.length() > ("--" + this->boundary + "--" + "\r\n").length())
            {
                this->out<<content.substr(0, content.length() - ("--" + this->boundary + "--" + "\r\n").length());
                this->brest = content.substr(content.length() - ("--" + this->boundary + "--" + "\r\n").length());
            }
            else
                this->brest += content;
        }
    }
    else
    {
        if (this->max_size < content.length())
            content = content.substr(0, this->max_size);
        this->out<<content;
        this->max_size -= content.length();
        if (this->max_size == 0)
        {
            this->out.close();
            this->clear = true;
            throw (201);
        }
    }
}

void    post::parse_uri(request& req, const one_server& server)
{
    Location        location;
    struct stat     path_info;
    struct stat     content_info;
    DIR             *dir;
    struct dirent   *entry;
    std::string     extension;
    std::string     path;
    size_t          exist;

    location = server.get_location(this->url);
    if (std::find(location._allow_methods.cbegin(), location._allow_methods.cend(), "POST") == location._allow_methods.cend())
        throw (405);
    if (location._return)
    {
        req.set_path(std::string(location._return) + "/");
        throw (307);
    }
    path = server.get_path(this->url);
    if (stat(path.c_str(), &path_info) != 0)
        throw (404);
    if (location.cgi_status)
    {
        if (S_ISREG(path_info.st_mode))
        {
            exist = path.find_last_of(".");
            if (exist != std::string::npos)
                extension = path.substr(exist);
            if (!location.get_cgi(extension).empty())
            {
                this->is_cgi = true;
                req.set_cgi_script(path);
                return ;
            }
        }
        else if (S_ISDIR(path_info.st_mode))
        {
            dir = opendir(path.c_str());
            while (dir && location._autoindex && (entry = readdir(dir)))
            {
                stat((path + "/" + entry->d_name).c_str(), &content_info);
                if (S_ISREG(content_info.st_mode))
                {
                    exist = std::string(entry->d_name).find_last_of(".");
                    if (exist != std::string::npos)
                        extension = std::string(entry->d_name).substr(exist);
                    if (location.is_index(entry->d_name) && !location.get_cgi(extension).empty())
                    {
                        req.set_cgi_script(path + "/" + entry->d_name);
                        this->is_cgi = true;
                        closedir(dir);
                        return ;
                    }
                }
            }
            closedir(dir);
        }
        else
            throw (404);
    }
    if (!location._upload)
        throw (403);
    this->upload_path = std::string(location.upload_path) + "/";
}

void    post::init(request& req,  one_server& server)
{
    this->first_time = false;
    this->url = req.get_path().substr(0, req.get_path().find("?"));
    this->parse_uri(req, server);
    this->content_type = req.get_header(CONTENT_TYPE);
    if (this->content_type.empty())
        throw (400);
    this->content_length = req.get_header(CONTENT_LENGTH);
    this->transfer_encoding = req.get_header(TRANSFER_ENCODING);
    if (this->content_type.substr(0, this->content_type.find(";")) == MULTIPART)
        this->boundary = this->content_type.substr(this->content_type.find("boundary=") + strlen("boundary="));
    if (!this->transfer_encoding.empty())
    {
        if (!this->content_length.empty())
            throw (400);
        if (this->transfer_encoding != "chunked")
            throw (405);
        this->mode = 1;
        if (this->boundary.empty())
            this->create_file(this->content_type);
        else if (this->is_cgi)
        {
            this->out_name.insert(this->out_name.end(), create_file_name());
            this->out.open(this->upload_path + this->out_name.back(), std::ios::out);
        }
        else
            this->mode = 3;
        return ;
    }
    if (!this->boundary.empty())
    {
        if (!this->content_length.empty())
            throw (400);
        this->mode = 2;
        if (!is_cgi)
            return ;
        this->mode = 4;
        this->out_name.insert(this->out_name.end(), create_file_name());
        this->out.open(this->upload_path + this->out_name.back(), std::ios::out);
        return ;
    }
    if (this->content_length.empty())
        throw (411);
    for (size_t i = 0; this->content_length[i]; i++)
        if (!isdigit(this->content_length[i]))
            throw (400);
    this->max_size = atoll(this->content_length.c_str());
    this->full_size = this->max_size;
    if (!this->is_cgi && atoll(this->content_length.c_str()) > atoll(server.client_body_size.c_str()))
        throw (413);
    if (this->is_cgi)
    {
        this->out_name.insert(this->out_name.end(), create_file_name());
        this->out.open(this->upload_path + this->out_name.back(), std::ios::out);
    }
    else
        this->create_file(this->content_type);
}

void    post::post_request(request& req, one_server& server)
{
    std::string content;

    content = req.get_body();
    if (this->first_time)
        this->init(req, server);
    if (this->mode == 1 || this->mode == 3)
        this->chunked(content, atoll(server.client_body_size.c_str()));
    else if (this->mode == 2)
        this->boundarry(content, atoll(server.client_body_size.c_str()));
    else
        this->raw(content);
}