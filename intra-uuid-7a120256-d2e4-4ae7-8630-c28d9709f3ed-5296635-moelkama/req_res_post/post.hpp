#ifndef POST_HPP
#define POST_HPP

# include <sys/time.h>
# include <dirent.h>
# include <sys/stat.h>
#include "../config_file/configFile.hpp"
#include "request.hpp"
#include "cgi.hpp"
#include <sstream>

class MimeType;

extern  MimeType mimetype;

class post
{
private:
    std::fstream    out;
    bool            first_time;
    size_t          full_size;
    size_t          max_size;
    size_t          chunked_size;
    //
    std::string     boundary;
    std::string     brest;
    std::map<std::string, std::string> headers;
    bool            h;
    //
    void            create_file(std::string extension);
    //
    std::string     content_type;
    std::string     content_length;
    std::string     transfer_encoding;
    ///
    short           mode;
    std::vector<std::string>     out_name;
    std::string     url;
    std::string     upload_path;
    std::string     rest;
    bool            clear;
public:
    bool            post_end;
    bool            is_cgi;
    post();
    post&   operator=(const post& other);
    post(const post& other);
    ~post();
    void        parse_uri(request& req, const one_server& server);
    void        init(request& req, one_server& server);
    void        chunked(std::string body, size_t client_max_size);
    void        boundarry(std::string body, size_t client_max_size);
    void        raw(std::string content);
    void        post_request(request& req, one_server& server);
    const std::vector<std::string>& get_out_name() const;
    std::string get_upload_path() const;
    std::string get_url() const;
    std::string set_headers(std::string b);
    bool        final_check();
};

std::string tostring(size_t n);
size_t      hex_to_dec(std::string n);

#endif