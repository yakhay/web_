/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moelkama <moelkama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/26 18:40:51 by wbouwach          #+#    #+#             */
/*   Updated: 2024/02/19 14:34:44 by moelkama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "getMethod.hpp"

bool isDirectory(const std::string& path) {
  struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) {
        return S_ISDIR(fileStat.st_mode);
    }
    return false;
}
std::string determine_content(const std::string& fileExtension)
{
    return (mimetype.get(fileExtension, 1));
}

void send_response_200(const std::string &filename, const std::string &contentType, int newsockfd, cl& client, one_server& server) {
    if (client.test2 == 0)
    {

        client.fileStream.open(filename, std::ios::binary);
        if (!client.fileStream.is_open())
        {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            throw (404);
        }

        struct stat statbuf;
        if (stat(filename.c_str(), &statbuf) == -1)
        {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            throw (404);
        }

        size_t size = statbuf.st_size;

        client.res.set_header("Content-Length", tostring(size));
        client.res.set_header("Content-type", contentType);
        client.res.set_Status(200, server);
        std::string response = client.res.prepare_respons(true);
        int dd = send(newsockfd, response.c_str(), response.size(), 0);
        client.take_more_time();
        if (dd < 0) {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            client.fileStream.close();
            return;
        }
        client.test2 = 1;
    }
    else
    {
        client.fileStream.read(client.buffer.data(), client.buffer_size);
        if (client.fileStream.gcount() < 0)
        {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            client.fileStream.close();
            return;
        }
        int valwrite = send(newsockfd, client.buffer.data(), client.fileStream.gcount(), 0);
        client.take_more_time();
        if (valwrite < 0) {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            client.fileStream.close();
            return;
        }
    }
    if (client.fileStream.eof())
    {
        client.fileStream.close();
        client.end_send = 1;
        if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
    }
}
std::string ft_check_index(std::vector<class Location>::iterator loca, std::string path)
{
    
    size_t i = 0;
    std::string new_path;
    size_t find = path.find_last_of("/");
    struct stat fileStat;
    
    while (i < loca->index.size())
    {
        if (find != std::string::npos && path[find + 1] == '\0')
            new_path = path + loca->index[i];
        else
            new_path = path+ "/" + loca->index[i];
        if (stat(new_path.c_str(), &fileStat) == 0)
            return (new_path);
        i++;
    }
    return ("-");
}
void get_method(request &req, manyServer *server, int newsockfd, int idx, cl& client) {
    std::string path = req.get_path().substr(0, req.get_path().find("?"));
    std::string new_path;
    std::string contentType;
    std::string root;

    if (client.lock == 0)
    {
         size_t find = path.find_last_of("/");
        if (find != std::string::npos && (path.c_str())[find + 1] == '\0' && strlen(path.c_str()) > 1)
        {
            std::string str_ret = path.substr(0,path.size() - 1);
            std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
            response += "Location: "+  str_ret +"\r\n\r\n";
            int dd = send(newsockfd, response.c_str(), response.size(), 0);
            client.take_more_time();
            if (dd == -1)
            {
                client.end_send = 1;
                client.fileStream.close();
                return;
            }
            close(newsockfd);
            client.end_send = 1;
            return ;
        }
        client.lock = 1;
        client.location = server->_name_server[idx]._location.begin();
        while (client.location != server->_name_server[idx]._location.end())
        {
            if (path.find(client.location->_name_Location) == 0)
            {
                root = client.location->_root;
                if (root.back() == '/')
                    root.pop_back();
                client.path = root + "/" + path.substr(client.location->_name_Location.size(),path.size() - client.location->_name_Location.size());
                break;
            }
            client.location++;
        }
        
        std::string old_path = client.path;
        client.path = correct_path(client.path);
        if (old_path.back() == '/')
            client.path.push_back('/');
        if (client.path.find(root) != 0)
            throw (403);
        if (client.location == server->_name_server[idx]._location.end())
            throw (404);
       if (std::find((client.location)->_allow_methods.cbegin(), (client.location)->_allow_methods.cend(), "GET") == (client.location)->_allow_methods.cend())
            throw (405);
        struct stat fileStat;
        if (stat(client.path.c_str(), &fileStat) != 0) {
            if (!S_ISREG(fileStat.st_mode) && !S_ISDIR(fileStat.st_mode)) 
                throw(404);
        }
    }
    new_path = client.path;
    if (client.location->_return)
    {
        std::string str_ret(client.location->_return , strlen(client.location->_return));
        std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
        response += "Location: "+  str_ret +"\r\n\r\n";
        int dd = send(newsockfd, response.c_str(), response.size(), 0);
        client.take_more_time();
        if (dd == -1)
            {
                client.end_send = 1;
                client.fileStream.close();
                return;
            }
        close(newsockfd);
        client.end_send = 1;
        return ;
    }
    else if (!isDirectory(new_path) ) {
        
            if ((new_path.find_last_of('.') != std::string::npos) && !(client.location->get_cgi(new_path.substr(new_path.find_last_of('.'))).empty()) && client.location->cgi_status == 1)
            {
                std::string msg;
                
                client.req.set_cgi_script(new_path);
                msg = client.cgi.cgi_run(server->_name_server[idx], client, "");
                if (write(newsockfd, msg.c_str(), msg.length()) == -1)
                {
                    client.end_send = 1;
                    close(newsockfd);
                }
                client.take_more_time();
            }
            else
            {
                if (new_path.find_last_of(".") == std::string::npos)
                    contentType = "application/octet-stream";
                else
                contentType = determine_content(new_path.substr(new_path.find_last_of(".") + 1));
                send_response_200(new_path, contentType, newsockfd, client, server->_name_server[idx]);
            }
         }
        else if (client.location->index.size() > 0 && ft_check_index(client.location, new_path) != "-")
        {
            
            new_path = ft_check_index(client.location, new_path);
            if ((new_path.find_last_of('.') != std::string::npos) && !(client.location->get_cgi(new_path.substr(new_path.find_last_of('.'))).empty()) && client.location->cgi_status == 1)
            {
                std::string msg;
            
                client.req.set_cgi_script(new_path);
                msg = client.cgi.cgi_run(server->_name_server[idx], client, "");
                if (write(newsockfd, msg.c_str(), msg.length()) == -1)
                {
                    client.end_send = 1;
                    close(newsockfd);
                }
                client.take_more_time();
            }
            else
            {
                if (new_path.find_last_of(".") == std::string::npos)
                    contentType = "application/octet-stream";
                else
                    contentType = determine_content(new_path.substr(new_path.find_last_of(".") + 1));
                send_response_200(new_path, contentType, newsockfd, client, server->_name_server[idx]);
            }
        }
    else if (client.location->_autoindex == 1) {
        if (client.test3 == 0) {
            std::remove("/tmp/listing.html");
            std::fstream outputFile("/tmp/listing.html", std::ios::in | std::ios::out | std::ios::app);
            outputFile.seekp(0, std::ios::beg);
            outputFile << "";
            if (!outputFile.is_open()) {
                throw(500);
            }
            client.test3 = 1;

            std::string buffer = "<html><head>\n<title>Directory Listing</title></head><body>\n<h1>Directory Listing</h1><ul>\n";
            DIR *dir = opendir(new_path.c_str());

            if (dir == nullptr) {
                throw(500);
            }
            struct dirent *entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string filename = entry->d_name;
                if (entry->d_type == DT_REG) {
                    
                    if (buffer.find(entry->d_name) == std::string::npos)
                        buffer = buffer + "<li>file :  <a href=\"" + "http://" + server->_name_server[idx]._Host + ":" + tostring(server->_name_server[idx].listen)  +path + "/" + filename + "\">" + filename + "</a></li> \n";
                    }
                     else if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
                            buffer += "<li> dir : <a href=\"" + std::string("http://") + server->_name_server[idx]._Host + ":" + tostring(server->_name_server[idx].listen)  + path + "/" + filename + "\">" + filename + "</a></li>\n";
                    
                }
            }
            outputFile << buffer + "</ul></body></html>\r\n";
            outputFile.close();
            closedir(dir);
            contentType = "text/html"; 
        }
        
        send_response_200("/tmp/listing.html", contentType, newsockfd, client, server->_name_server[idx]);
    } 
    else if (client.location->_autoindex == 0)
        throw(403);
}
