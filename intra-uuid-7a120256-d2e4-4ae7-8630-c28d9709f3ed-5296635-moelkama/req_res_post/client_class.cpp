#include "client_class.hpp"
#include "request.hpp"
#include "respons.hpp"
#include "post.hpp"

cl::cl(): buffer(buffer_size ,0), cgi(), post(), req(),res()
{
    gettimeofday(&this->spend, NULL);
    witeee = -1;
    lock = 0;
    end_send = 0;
    test1 = 0;
    test2 = 0;
    test3 = 0;
}

cl::cl(const cl& other)
{
    *this = other;
}

cl& cl::operator=(const cl& other)
{
    this->post = other.post;
    this->test1 = other.test1;
    this->test2 = other.test2;
    this->test3 = other.test3;
    return (*this);
}

cl::~cl(){}

void    cl::take_more_time()
{
    gettimeofday(&this->spend, NULL);
}

bool    cl::too_long() const
{
    struct timeval  Now;
    float           time;

    gettimeofday(&Now, NULL);
    time = Now.tv_sec - this->spend.tv_sec + (float)(Now.tv_usec - this->spend.tv_usec) / 1000000;
    if (time > CLIENT_MAX_TIME)
        return (true);
    return (false);
}