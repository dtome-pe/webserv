#include<webserv.hpp>

int	main(int argc, char *argv[])
{	
	if (argc != 2)
	{
		print_error("Wrong number of arguments.");
		exit(EXIT_SUCCESS);
	}

	Cluster		webserv;
	
	try {
	webserv.parseConfig(argv[1]); // parseamos conf file y volcamos informacion en Cluster
	webserv.setup();  // juntamos el antiguo create / start / init poll de ConfFile
	webserv.run(); // basicamente es el loop del poll con la gestion de la peticion / envío respuesta
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}
