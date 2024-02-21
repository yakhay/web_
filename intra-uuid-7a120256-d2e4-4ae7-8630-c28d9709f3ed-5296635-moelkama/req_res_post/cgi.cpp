#include "cgi.hpp"
#include "client_class.hpp"

cgi::cgi()
{
    this->is_runing = false;
    this->first = true;
    this->env = NULL;
    this->args = new char*[3];
    this->args[0] = NULL;
    this->args[1] = NULL;
    this->args[2] = NULL;
}

cgi::~cgi()
{
    if (this->first)
    {
        for (int i = 0; this->args[i]; i++)
            delete this->args[i];
        delete[] this->args;
    }
}

void cgi::set_cgi(const Location& location, const std::string& path)
{
    std::string extension;
    size_t      point;

    point = path.find_last_of(".");
    if (point == std::string::npos)
        return ;
    extension = path.substr(point);
    this->args[0] = strdup(location.get_cgi(extension).c_str());
    (void)location;
}

std::string    create_file_name()
{
    struct timeval  Now;

    gettimeofday(&Now, NULL);
    return (tostring(Now.tv_sec) + "_" + tostring(Now.tv_usec));
}

void    cgi::cgi_init(const one_server& server, const request& req, const std::string& input)
{
    Location    location;
    std::string uri;
    std::string path;
    size_t      find;
    int         idx = 0;

    uri = req.get_path();
    find = uri.find("?");
    path = uri.substr(0, find);
    location = server.get_location(path);
    set_cgi(location, req.get_cgi_script());
    this->args[1] = strdup(req.get_cgi_script().c_str());
    this->input = input;
    (void)server;
    (void)req;
    (void)input;
    this->output = "/tmp/" + create_file_name();
    
    if (req.get_method() == "POST")
    {
        struct stat fileStat;

        stat(input.c_str(), &fileStat);
        this->env_m["CONTENT_TYPE"] = req.get_header("Content-Type");//// get
        this->env_m["CONTENT_LENGTH"] = tostring(fileStat.st_size);
    }
    this->env_m["SERVER_NAME"] = server._server_name;
    this->env_m["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->env_m["SERVER_PROTOCOL"] = HTTP_VERSION;
    this->env_m["SERVER_PORT"] = tostring(server.listen);
    this->env_m["PATH_INFO"] = this->args[1];
    this->env_m["REQUEST_METHOD"] = req.get_method();
    this->env_m["SCRIPT_FILENAME"] = this->args[1];
    this->env_m["REDIRECT_STATUS"] = "200";
    if (find != std::string::npos)
        this->env_m["QUERY_STRING"] = uri.substr(find + 1, uri.find("#"));
    this->env = new char*[this->env_m.size() + 1];
    for (std::map<std::string, std::string>::const_iterator it = this->env_m.cbegin(); it != this->env_m.cend(); it++)
        this->env[idx++] = strdup((it->first + "=" + it->second).c_str());
    this->env[idx] = NULL;
    this->env_m.clear();
}

void    cgi::cgi_execute()
{
    this->pid = fork();
    if (this->pid == -1)
        this->status = 1;
    else if (!this->pid)
    {
        if (!std::freopen(this->output.c_str(), "w", stdout))
            exit(1);
        if (!this->input.empty()  && !std::freopen(this->input.c_str(), "r", stdin))
            exit(1);
        if (access(this->args[0], X_OK) == 0)
            execve(this->args[0], this->args, this->env);
        exit(1);
    }
}

std::string cgi::cgi_run(one_server& server, cl& client, const std::string& input)
{
    std::string line;

    if (this->first)
    {
        this->first = false;
        this->is_runing = true;
        this->cgi_init(server, client.req, input);
        gettimeofday(&this->begin, NULL);
        this->cgi_execute();
    }
    else
    {
        if (this->is_runing)
        {
            struct timeval  Now;
            float           time;

            gettimeofday(&Now, NULL);
            time = Now.tv_sec - this->begin.tv_sec + (float)(Now.tv_usec - this->begin.tv_usec) / 1000000;
            if (waitpid(this->pid, &this->status, WNOHANG) != -1)
            {
                if (time < CGI_TIMEOUT)
                    return "";
                kill(this->pid, SIGINT);
                waitpid(this->pid, NULL, 0);
                this->status = -1;
            }
            this->is_runing = false;
            for (int i = 0; this->env[i]; i++)
                delete this->env[i];
            for (int i = 0; this->args[i]; i++)
                delete this->args[i];
            delete[] this->env;
            delete[] this->args;
            if (this->status == -1)
            {
                client.res.set_Status(504, server);
                client.end_send = 1;
            }
            else if (WEXITSTATUS(this->status) != 0)
            {
                client.res.set_Status(500, server);
                client.end_send = 1;
            }
            else
                client.res.set_Status(200, server);
            remove(input.c_str());
            in.open(this->output, std::ios::in);
            line = client.res.prepare_respons(WEXITSTATUS(this->status));
        }
        else
        {
            char    buffer[BUFFER_SIZE + 1];
            bzero(buffer, BUFFER_SIZE + 1);
            size_t x = in.read(buffer, BUFFER_SIZE).gcount();
            if (in.eof())
            {
                in.close();
                client.end_send = 1;
            }
            line.append(buffer, x);
        }
    }
    if (client.end_send)
        remove(this->output.c_str());
    return (line);
}