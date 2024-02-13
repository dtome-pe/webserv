#include<webserv.hpp>

void	main_list(char *argv[])
{
	t_data	data;

	init_data(&data); // iniciamos lista de servidores y poll
	parse_config(argv[1], &data); //
	print_servers(data.serv_list); // printamos la info de los servers
	init_servers(&data, data.serv_list); // inicializamos los servers de la lista y los sockets de sus respectivas listas
	init_poll(&data, data.serv_list); // 

	poll_loop(data.poll, data.fd_size);
}

void	main_vec(char *argv[])
{
	ConfFile	F(argv[1]);

	F.parse_config(); //
//	F.print_servers(); // printamos la info de los servers
//	F.init_serv(); // inicializamos los servers de la lista y los sockets de sus respectivas listas
//	F.init_poll(); //
//	poll_loop(F.poll_ptr, F.fd_size);
}

int	main(int argc, char *argv[])
{
	if (argc != 2)
	{
		print_error("Wrong number of arguments.");
		exit(EXIT_SUCCESS);
	}
	//main_list(argv);
	main_vec(argv);
}
