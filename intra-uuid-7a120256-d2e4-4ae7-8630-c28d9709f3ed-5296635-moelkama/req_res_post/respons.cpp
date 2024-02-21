#include "respons.hpp"
#include "post.hpp"

respons::respons()
{
    this->Status = mimetype.get_status(201);
}

void    respons::set_header(std::string key, std::string value)
{
    this->headers[key] = value;
}

void    respons::set_Body(std::string body)
{
    this->Body = body;
    set_header(CONTENT_LENGTH, tostring(this->Body.length()));
}

void    respons::set_Status(int st, const one_server& server)
{
    std::string     error_page_name;
    std::string     error_page_extension;
    std::string     line;

    set_header(SERVER, server._server_name);
    this->Status = mimetype.get_status(st);
    error_page_name = server.get_error_page(tostring(st));
    std::ifstream   error_page(error_page_name);
    if (error_page.is_open())
    {
        error_page_extension = error_page_name.substr(error_page_name.find_last_of(".") + 1);
        while (!error_page.eof())
        {
            std::getline(error_page, line);
            this->Body.append(line + LINE_SEPARATOR);
        }
        set_header(CONTENT_TYPE, mimetype.get(error_page_extension, 1));
        set_header(CONTENT_LENGTH, tostring(this->Body.length()));
    }
    else if (st / 100 != 2 && st / 100 != 3 && st != 501)
    {
        this->Body = "<!DOCTYPE html><html lang=\"en\"><head>    <meta charset=\"UTF-8\">    <title>ERROR " + tostring(st) +"</title></head><body><h1>ERROR !!!!!" + tostring(st) +"!</h1></body></html>";
        set_header(CONTENT_TYPE, "text/html");
        set_header(CONTENT_LENGTH, tostring(this->Body.length()));
    }
    error_page.close();
}

void    respons::set_post_info(const post& p)
{
    this->Status = mimetype.get_status(201);
    this->set_header(CONTENT_TYPE, "application/json");
    this->Body = "{\n\t";
    for (std::vector<std::string>::const_iterator it = p.get_out_name().begin(); it != p.get_out_name().end(); it++)
    {
        this->Body += "\"Create_Ressource\": {\n\t";
        this->Body += "\t\"name\": \"";
        this->Body += *it;
        this->Body += "\",\n\t\t\"location\": \"";
        this->Body += p.get_url();
        this->Body += "\"\n\t}\n\t";
    }
    this->Body += "\n}";
    this->set_header(CONTENT_LENGTH, tostring(this->Body.length()));
}

std::string respons::prepare_respons(bool b)
{
    std::string respons;

    respons = HTTP_VERSION;
    respons += " " + tostring(this->Status.first);
    respons += " " + this->Status.second;
    std::map<std::string, std::string>::iterator  it = this->headers.begin();
    std::map<std::string, std::string>::iterator  eit = this->headers.end();
    while (it != eit)
    {
        respons += LINE_SEPARATOR + it->first + HEDER_SEPARATOR + it->second;
        it++;
    }
    if (b)
        respons += BODY_SEPARATOR + this->Body;
    else
        respons += LINE_SEPARATOR;
    return (respons);
}

respons::~respons(){}