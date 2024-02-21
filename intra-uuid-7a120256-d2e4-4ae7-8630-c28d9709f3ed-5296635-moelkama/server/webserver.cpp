#include "webserv.hpp"

std::map<int ,cl>   clients;

void Webserver::Init(){
    memset(&server_infos, 0, sizeof(server_infos));
    server_infos.ai_family      = AF_INET;
    server_infos.ai_socktype    = SOCK_STREAM;
    server_infos.ai_flags       = AI_PASSIVE;
    int result = getaddrinfo(this->servers._Host.c_str(), tostring(this->servers.listen).c_str(), &server_infos, &sinfo_ptr);
    if (result != 0) {
        std::cerr << "Error: getaddrinfo failed: " << gai_strerror(result) << std::endl;
        exit(1);
    }
}

Webserver::Webserver(one_server& serv) : servers(serv)
{
    std::cout << "listen port : "<< serv.listen << " host: "<< serv._Host << std::endl;
}

void Webserver::CreateServer(){
    Init();
    int optval = 1;
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_socktype, sinfo_ptr->ai_protocol)) == -1)
    {
       perror("Error: SOCKET failed -> ");
        exit(1);
    }
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
        perror("Error: FCNTL <Server Socket> -> ");
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
         perror("Error: SETSOCKOPT failed -> ");
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
    {
        perror("Error: BIND failed -> ");
        exit(1);
    }
    if (listen(server_socket, FD_SETSIZE) == -1)
    {
       perror("Error: LISTEN failed -> ");
        exit(1);
    }
    freeaddrinfo(sinfo_ptr);
}

void Webserver::SelectSetsInit(){

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    // FD_ZERO(&tmpfdread);
    // FD_ZERO(&tmpfdwrite);
    FD_SET(server_socket, &readfds);
    FD_SET(server_socket, &writefds);
    maxfds = server_socket;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
}

void Webserver::StartServer(manyServer* servers,int idx)
{
    signal(SIGPIPE, SIG_IGN);
    tmpfdread = readfds;
    tmpfdwrite = writefds;

    int active = select(maxfds + 1, &tmpfdread, &tmpfdwrite, NULL, &timeout);

    if (active == -1)
        perror("Error: SELECT failed -> ");

    if (FD_ISSET(server_socket, &tmpfdread))
    {
        client_socket = AcceptAndAddClientToSet();
    }
     for (itClient = clientsList.begin(); itClient != clientsList.end();) {
        bytesrev = 0;
        active_clt = itClient->GetCltSocket();
        if (clients.find(active_clt) == clients.end())
            clients[active_clt] = cl();

        if (FD_ISSET(active_clt, &tmpfdread)) {
            if (fcntl(active_clt, F_SETFL, O_NONBLOCK) == -1)
                perror("Error: FCNTL <New Connection> -> ");
            bytesrev = recv(active_clt, reqData, sizeof(reqData), 0);
            clients[active_clt].take_more_time();

            if (bytesrev == 0) {
                DeleteClient();
                continue;
            } else if (bytesrev < 0) {
                DeleteClient();
                continue;
            } else {
                flag = true;
            }
        }

        if (FD_ISSET(active_clt, &tmpfdwrite) && flag == true) {
            if (itClient->_client_handler.driver(reqData, bytesrev, active_clt, servers, idx) == 0)
            {
                DeleteClient();
                continue;
            }
        }

        itClient++;
    }
}

void Webserver::DeleteClient()
{
    if (active_clt > 0)
        close(active_clt);
    if (clients[active_clt].fileStream.is_open())
        clients[active_clt].fileStream.close();
    FD_CLR(active_clt, &readfds);
    FD_CLR(active_clt, &writefds);
    clientsList.erase(itClient++);
}

int Webserver::AcceptAndAddClientToSet()
{
    int connection = accept(server_socket, (struct sockaddr *)&storage_sock, &client_addr);
    if (connection < 0)
        perror("Error: ACCEPT failed -> ");
    if (fcntl(connection, F_SETFL, O_NONBLOCK) == -1)
        perror("Error: FCNTL <New Connection> -> ");
    if (connection == -1)
        perror("Error: ACCEPT <New Connection> -> ");
    clientsList.push_back(Client(connection));
    flag = false;
    clientsList.back()._client_handler.setConfig(this->servers);
    FD_SET(clientsList.back().GetCltSocket(), &readfds);
    FD_SET(clientsList.back().GetCltSocket(), &writefds);
    if (clientsList.back().GetCltSocket() > maxfds)
        maxfds = clientsList.back().GetCltSocket();  
    return (connection);
}

Handle::Handle(){}

void Handle::setConfig(one_server config)
{
    this->config = config;
}

Client::Client(int socket)
{
    _client_handler.client_socket = socket;
    this->_socket = socket;
    
}

int Client::GetCltSocket()
{
    return (this->_socket);
}