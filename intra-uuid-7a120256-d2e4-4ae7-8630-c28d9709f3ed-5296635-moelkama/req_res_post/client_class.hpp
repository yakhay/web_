#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include "post.hpp"
#include "request.hpp"
#include "macro.hpp"

class   cl
{
public:
    int              witeee;
    struct timeval      spend;
    int                 lock;
    std::string         path;
    static const size_t buffer_size = 1024;
    std::vector<char >  buffer;
    std::ifstream       fileStream;
    cgi                 cgi;
    post                post;
    request             req;
    respons             res;
    int                 test1;
    int                 test2;
    int                 test3;
    bool                end_send;
    std::vector<class Location>::iterator location;
    cl();
    cl(const cl& other);
    cl& operator=(const cl& other);
    void    take_more_time();
    bool    too_long() const;
    ~cl();
};

#endif