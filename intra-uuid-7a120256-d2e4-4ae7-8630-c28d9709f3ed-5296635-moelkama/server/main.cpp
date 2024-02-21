#include "webserv.hpp"

MimeType    mimetype;


void   Webserver::checkServerName(std::vector<std::string> name_of_servers)
{
    for (size_t i = 0; i < name_of_servers.size(); i++)
    {
        for (size_t j = 0; j < name_of_servers.size(); j++)
        {
            if (i != j && name_of_servers[i] == name_of_servers[j])
            {
                std::cerr << "Error: Server name is not unique" << std::endl;
                exit(1);
            }
        }
    }
}

void Webserver::checksameports(std::vector<int> ports)
{
    for (size_t i = 0; i < ports.size(); i++)
    {
        for (size_t j = 0; j < ports.size(); j++)
        {
            if (i != j && ports[i] == ports[j])
            {
                std::cerr << "Error: Server port numbers" << std::endl;
                exit(1);
            }
        }
    }
}

int main (int ac, char **av)
{
    const char *parameter;
    if (ac > 2)
        return (1);
    if (ac == 1)
       parameter = "./config_file/file.conf";
    else
        parameter = av[1];
    manyServer* servers = new manyServer(parameter);
    std::vector<Webserver> servs;
    std::vector<std::string> name_of_servers;
    std::vector<int> ports;
    for (size_t i = 0; i < servers->_name_server.size(); i++)
    {
        std::cout << "------------------------" << std::endl;
        std::cout << servers->_name_server[i].listen << std::endl;
        Webserver serv(servers->_name_server[i]);
        name_of_servers.push_back(servers->_name_server[i]._server_name);
        ports.push_back(servers->_name_server[i].listen);
        serv.checkServerName(name_of_servers);
        serv.checksameports(ports);
        serv.CreateServer();
        serv.SelectSetsInit();
        servs.push_back(serv);
    }
    while (1)
    {
        for (size_t i = 0; i < servers->_name_server.size(); i++)
        {
            servs[i].StartServer(servers,i);
        }
    }
    return (0);
}