#include<webserv.hpp>

int	main(int argc, char *argv[])
{	
	Cluster		webserv;
	std::string	file;

	if (argc > 2)
	{
		print_error("Wrong number of arguments.");
		return (0);
	}
	if (argc < 2)
		file = "conf/default.conf";
	else
		file = argv[1];
	try {
		webserv.parseConfig(file);
		webserv.setup();
		webserv.run();
		webserv.clean();
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		webserv.clean();
	}
}
