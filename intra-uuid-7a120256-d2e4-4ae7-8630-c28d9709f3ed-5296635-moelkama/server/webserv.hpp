/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moelkama <moelkama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 18:22:50 by wbouwach          #+#    #+#             */
/*   Updated: 2024/02/17 11:58:53 by moelkama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

# define BUFFER_SIZE 1024
# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <errno.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string.h>
# include <vector>
# include <sys/stat.h>
# include <sstream>
# include <netdb.h>
# include <list>
# include <fstream>
# include <signal.h>
# include <dirent.h>
# include <sys/time.h>
#include <fcntl.h>

# include "../config_file/configFile.hpp"
# include "../req_res_post/request.hpp"
# include "../req_res_post/post.hpp"
# include "../req_res_post/macro.hpp"
# include "../req_res_post/cgi.hpp"
# include "../req_res_post/MimeType.hpp"
# include "../req_res_post/respons.hpp"
# include "../req_res_post/client_class.hpp"
# include "../req_res_post/delete.hpp"
# include "../get_method/getMethod.hpp"

class one_server;
class Webserver;
class Client;

class Handle{
    public:
        Handle();
        int client_socket;
        one_server config;
        Handle(one_server serv) : client_socket(-1), config(serv) {}
        int driver(char *requested_data, int bytesreceived,int active_clt,manyServer* servers,int idx);
        void setConfig(one_server config);
};

class Client {
    private:
        int _socket;
    
    public:
        
        Handle _client_handler;
        Client(int socket);
        int GetCltSocket();
};



class Webserver {
private:
    one_server servers;
    std::list<Client> clientsList;
    std::list<Client>::iterator itClient;

public:
    Webserver(one_server& serv);
    int server_socket;
    int client_socket;
    struct addrinfo server_infos;
    struct addrinfo *sinfo_ptr;
    socklen_t client_addr;
    struct sockaddr_storage storage_sock;
    fd_set readfds, writefds, tmpfdread, tmpfdwrite;
    int maxfds;
    int flag;
    int bytesrev;
    struct timeval          timeout;
    int active_clt;
    char reqData[1024];

    void Init();
    void SelectSetsInit();
    void StartServer(manyServer* servers,int i);
    void DeleteClient();
    int AcceptAndAddClientToSet();
    void CreateServer();
    void checkServerName(std::vector<std::string> name_of_servers);
    void checksameports(std::vector<int> ports);
};

#endif