#include "MimeType.hpp"

MimeType::MimeType()
{
    this->status[200] = "OK";
    this->status[201] = "Created";
    this->status[204] = "No Content";
    this->status[301] = "Moved Permanently";
    this->status[307] = "Temporary Redirect";
    this->status[400] = "Bad Request";
    this->status[403] = "Forbiden";
    this->status[404] = "Not Found";
    this->status[405] = "Not Allowed";
    this->status[408] = "Request Timeout";
    this->status[409] = "Conflict";
    this->status[411] = "Length Required";
    this->status[413] = "Request Entity Too Large";
    this->status[414] = "Request-URI Too Long";
    this->status[415] = "Unsupported Media Type";
    this->status[418] = "I'm a teapot";
    this->status[500] = "Internal Server Error";
    this->status[501] = "Not Implemented";
    this->status[504] = "Gateway Timeout";
    this->status[505] = "HTTP Version Not Supported";

    this->medias["text/html"]                                                                   = "html";
    this->medias["text/css"]                                                                    = "css";
    this->medias["application/javascript"]                                                      = "js";
    this->medias["application/json"]                                                            = "json";
    this->medias["application/xml"]                                                             = "xml";
    this->medias["text/plain"]                                                                  = "txt";
    this->medias["image/jpg"]                                                                   = "jpg";
    this->medias["image/jpeg"]                                                                  = "jpeg";
    this->medias["image/png"]                                                                   = "png";
    this->medias["image/gif"]                                                                   = "gif";
    this->medias["image/bmp"]                                                                   = "bmp";
    this->medias["image/x-icon"]                                                                = "ico";
    this->medias["application/pdf"]                                                             = "pdf";
    this->medias["application/zip"]                                                             = "zip";
    this->medias["application/x-tar"]                                                           ="tar";
    this->medias["application/gzip"]                                                            = "gz";
    this->medias["application/x-rar-compressed"]                                                = "rar"; 
    this->medias["application/x-7z-compressed"]                                                 = "7z";
    this->medias["audio/mpeg"]                                                                  = "mp3";
    this->medias["audio/wav"]                                                                   = "wav";
    this->medias["video/mp4"]                                                                   = "mp4";
    this->medias["video/x-msvideo"]                                                             = "avi";
    this->medias["application/vnd.ms-powerpoint"]                                               = "ppt";
    this->medias["application/vnd.openxmlformats-officedocument.presentationml.presentation"]   = "pptx";
    this->medias["application/msword"]                                                          = "doc";
    this->medias["application/vnd.openxmlformats-officedocument.wordprocessingml.document"]     = "docx";
    this->medias["application/vnd.ms-excel"]                                                    = "xls";
    this->medias["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"]           = "xlsx";
    this->medias["text/csv"]                                                                    = "csv";
    this->medias["application/x-shockwave-flash"]                                               = "swf";
    this->medias["image/svg+xml"]                                                               = "svg";
    this->medias["video/mpeg"]                                                                  = "mpg";
    this->medias["video/mpeg"]                                                                  = "mpeg";
    this->medias["video/webm"]                                                                  = "webm";
    this->medias["audio/ogg"]                                                                   = "ogg";
    this->medias["video/ogg"]                                                                   = "ogg";
    this->medias["image/webp"]                                                                  = "webp";
    this->medias["image/tiff"]                                                                  = "tif";
    this->medias["image/tiff"]                                                                  = "tiff";
    this->medias["application/font-woff"]                                                       = "woff";
    this->medias["application/font-woff2"]                                                      = "woff2";
    this->medias["application/x-font-ttf"]                                                      = "ttf";
    this->medias["application/x-font-opentype"]                                                 = "otf";
    this->medias["application/vnd.ms-fontobject"]                                               = "eot";
    this->medias["application/octet-stream"]                                                    = "bin";
    this->medias["application/x-perl"]                                                          = "pl";
    this->medias["video/quicktime"]                                                             = "mov";
}

MimeType::MimeType(const MimeType& other)
{
    *this = other;
}

MimeType& MimeType::operator=(const MimeType& other)
{
    this->medias.clear();
    for (std::map<std::string, std::string>::const_iterator it = other.medias.cbegin(); it != other.medias.cend(); it++)
        this->medias[it->first] = it->second;
    return (*this);
}

MimeType::~MimeType()
{
    this->medias.clear();
}

std::string MimeType::get(const std::string& media, const bool& k) const
{
    std::map<std::string, std::string>::const_iterator  it;
    std::map<std::string, std::string>::const_iterator  eit;

    it = this->medias.cbegin();
    eit = this->medias.cend();
    while (it != eit)
    {
        if (!k && it->first == media)
            return (it->second);
        else if (k && it->second == media)
            return (it->first);
        it++;
    }
    if (it == eit)
        throw (415);
    return (it->second);
}

std::pair<int, std::string>     MimeType::get_status(const int& st) const
{
    return (*(this->status.find(st)));
}