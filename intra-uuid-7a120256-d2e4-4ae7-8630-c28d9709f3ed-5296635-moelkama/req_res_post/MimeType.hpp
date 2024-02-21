#ifndef MIMETYPE_HPP
#define MIMETYPE_HPP

#include <map>
#include <iostream>

class   MimeType
{
private:
    std::map<std::string, std::string>  medias;
    std::map<int, std::string>          status;
public:
    MimeType();
    MimeType(const MimeType& other);
    MimeType& operator=(const MimeType& other);
    ~MimeType();
    std::string                     get(const std::string& media, const bool& k) const;
    std::pair<int, std::string>     get_status(const int& st) const;
};

#endif