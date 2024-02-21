#include "webserv.hpp"

extern std::map<int ,cl>    clients;

int Handle::driver(char *requested_data, int bytesreceived,int active_clt,manyServer* servers,int idx)
{
    try
    {
        if (clients[active_clt].too_long())
            throw (408);
        if (clients[active_clt].witeee != -1)
            throw (clients[active_clt].witeee);
        if (clients[active_clt].req.parse_request(std::string("").append(requested_data, bytesreceived), bytesreceived))
        {
            if (clients[active_clt].req.get_method() == "GET")
            {
                if (!std::string("").append(requested_data, bytesreceived).empty())
                    return (1);
                get_method(clients[active_clt].req, servers, active_clt, idx, clients.find(active_clt)->second);
                if (clients[active_clt].end_send)
                {
                    clients.erase(clients.find(active_clt));
                    return (0);
                }
            }
            else if (clients[active_clt].req.get_method() == "POST")
            {
                if (clients[active_clt].post.post_end)
                    throw (201);
                clients[active_clt].post.post_request(clients[active_clt].req, servers->_name_server[idx]);
            }
            else if (clients[active_clt].req.get_method() == "DELETE")
                delete_request(servers->_name_server[idx], clients[active_clt].req);
            else
                throw (501);
        }
    }
    catch(int status)
    {
        std::string msg;

        clients[active_clt].post.post_end = 1;
        clients[active_clt].witeee = status;
        if (!std::string("").append(requested_data, bytesreceived).empty())
            return (1);
        if (status == 307)
            clients[active_clt].res.set_header("Location", clients[active_clt].req.get_path().substr(0, clients[active_clt].req.get_path().find('?')).substr(0, clients[active_clt].req.get_path().length() - 1));
        if (status == 201)
        {
            if (clients[active_clt].post.final_check() == false)
            {
                std::string msg;
                clients[active_clt].res.set_Status(500, servers->_name_server[idx]);
                msg = clients[active_clt].res.prepare_respons(true);
                write(active_clt, msg.c_str(), msg.length());
                clients[active_clt].end_send = 1;
            }
            if (clients[active_clt].post.is_cgi)
                msg = clients[active_clt].cgi.cgi_run(servers->_name_server[idx], clients[active_clt], clients[active_clt].post.get_upload_path() +"/"+ clients[active_clt].post.get_out_name().back());
            else
            {
                clients[active_clt].res.set_header(SERVER, servers->_name_server[idx]._server_name);
                clients[active_clt].res.set_post_info(clients[active_clt].post);
                msg = clients[active_clt].res.prepare_respons(true);
                clients[active_clt].end_send = 1;
            }
        }
        else
        {
            clients[active_clt].res.set_Status(status, servers->_name_server[idx]);
            msg = clients[active_clt].res.prepare_respons(true);
            clients[active_clt].end_send = 1;
        }
        write(active_clt, msg.c_str(), msg.length());
        clients[active_clt].take_more_time();
    }
    catch (const std::exception& e)
    {
        (void)e;
        std::string msg;
        clients[active_clt].res.set_Status(500, servers->_name_server[idx]);
        msg = clients[active_clt].res.prepare_respons(true);
        write(active_clt, msg.c_str(), msg.length());
        clients[active_clt].end_send = 1;
    }
    if (clients[active_clt].end_send)
    {
        clients.erase(clients.find(active_clt));
        return (0);
    }
    return (1);
}