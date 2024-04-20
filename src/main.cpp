#include<webserv.hpp>

int	main(int argc, char *argv[])
{	
	if (argc != 2)
	{
		print_error("Wrong number of arguments.");
		return (0);
	}

	Cluster		webserv;
	
	try {
	webserv.parseConfig(argv[1]);
	webserv.setup();
	webserv.run();
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}
