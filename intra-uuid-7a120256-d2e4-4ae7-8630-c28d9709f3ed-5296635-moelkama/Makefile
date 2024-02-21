NAME = serv

SRRC = config_file/configFile.cpp \
	server/Handling.cpp req_res_post/client_class.cpp\
	req_res_post/delete.cpp req_res_post/respons.cpp req_res_post/cgi.cpp req_res_post/post.cpp req_res_post/request.cpp req_res_post/MimeType.cpp get_method/getMethod.cpp\
	server/webserver.cpp server/main.cpp

OBJ = $(SRRC:.cpp=.o)

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	c++ $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all