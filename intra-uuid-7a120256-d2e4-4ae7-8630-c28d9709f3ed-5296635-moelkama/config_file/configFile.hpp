# ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>
#include <map>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>

class Location
{
public :
    int _autoindex;
    std::string  _name_Location;
    std::string  _root;
    int _upload;
    char *upload_path;
    std::vector<std::string> index;
    char *_return;
    int cgi_status;
    std::map<std::string, std::string> cgi;
    std::vector<std::string> _allow_methods;
    Location(std::string location);
   
    ~Location();
    Location();
    std::string get_upload_path() const;
    std::string get_cgi(std::string extension) const;
    bool        is_index(const std::string& name) const;
};

bool cmpr(const Location& loc1, const Location& loc2);
std::string correct_path(std::string path);

class one_server
{
    public :
        int                                 listen;
        std::string                         _Host;
        std::string                         _server_name;
        std::string                          client_body_size;
        std::map<std::string, std::string>  error_page;

        std::vector<class Location>         _location;
        one_server();
        one_server(std::string string_to_pars);

        Location    get_location(std::string url) const;
        std::string get_path(const std::string& url) const;
        std::string get_error_page(std::string status) const;
};

class manyServer
{
    public :
        std::vector<class one_server> _name_server;
        int _count_server;
        std::string *par;
        manyServer();
        manyServer(std::string file);

};
#endif