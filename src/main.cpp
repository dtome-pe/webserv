#include<webserv.hpp>

/* void	main_list(char *argv[])
{
	t_data	data;

	init_data(&data); // iniciamos lista de servidores y poll
	parse_config(argv[1], &data); //
	print_servers(data.serv_list); // printamos la info de los servers
	init_servers(&data, data.serv_list); // inicializamos los servers de la lista y los sockets de sus respectivas listas
	init_poll(&data, data.serv_list); // 

	poll_loop(data.poll, data.fd_size);
} */

void	main_vec(char *argv[])
{
	ConfFile	F(argv[1]);

	F.parse_config(); //
//	F.check_info();
	F.print_servers(); // printamos la info de los servers
//	F.create_sockets(); // creamos sockets, hacemos getaddrinfo resolviendo hostname a ip, pero si hay un socket ya en el vector
				// con misma direccion y puerto, no lo anadimos.
	//F.print_sockets();
//	F.start_sockets(); // inicializamos los sockets y le pasamos ip y puerto a su server mediante puntero, como nuevo elemento
						// de vector ip_port (ip ya resuelta del host)
//	F.print_servers();
//	F.init_poll(); //
//	poll_loop(F.poll_ptr, F.fd_size, F);
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
