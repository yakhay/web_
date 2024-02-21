/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getMethod.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moelkama <moelkama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/26 18:40:54 by wbouwach          #+#    #+#             */
/*   Updated: 2024/02/06 14:31:13 by moelkama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETMETHOD_HPP
# define GETMETHOD_HPP

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

# include "../req_res_post/request.hpp"
# include "../req_res_post/respons.hpp"
# include "../req_res_post/post.hpp"
# include "../req_res_post/macro.hpp"

# include "../config_file/configFile.hpp"
# include <sys/stat.h>
# include <sstream>
# include <fstream>
#include <filesystem>
# include <sys/socket.h>
#include <fcntl.h>
#include <filesystem>
#include "../req_res_post/client_class.hpp"

void get_method(request &req, manyServer *server, int newsockfd, int idx, cl& client);
bool isDirectory(const std::string& path);
#endif