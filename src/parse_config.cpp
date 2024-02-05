#include "../inc/webserv.hpp"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fstream>

static int	parse_element(t_serv *list, std::string &line)
{
	if (!line.compare(0, 7, "\tlisten")) // si recibimos petición
	{
		if (parse_listen(&list->serv->sock_list, line))
			return (1);
	}
	return (0);
}

int	parse_config(const std::string& file, t_data *data)
{
	std::ifstream in;
	std::string line;
	(void)		data;

	in.open(file.c_str(), std::ios::in); //abrimos archivo de configuración (nginx)
	if (!in.is_open())
	{
		print_error("File could not be found or opened.\n");
		exit(EXIT_FAILURE);
	}
	while (in.good()) // leemos por lineas el archivo de configuración
	{
		getline(in, line);
		if (!line.compare("server:")) // si leemos el indicador "server:", creamos nuevo server
		{
			std::cout << "new server!!" << std::endl;
			serv_back(&data->serv_list, serv_new()); // añadimos nuevo server creado
		}
		if (line.at(0) == '\t') // si leemos un tab, significa que viene una instrucción para el server
		{
			if (parse_element(serv_last(data->serv_list), line)) //parseamos la instrucción
			{
				print_error("Wrong config file format.\n");
				exit(EXIT_FAILURE);
			}	
		}
	}
	return (0);	
}