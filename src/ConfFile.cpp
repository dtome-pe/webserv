#include<webserv.hpp>
#include <poll.h>

ConfFile::ConfFile(std::string _file)
{
	file = _file;
}

ConfFile::~ConfFile()
{
}

void	ConfFile::parse_config()
{
	std::ifstream in;
	std::string line;

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
			//serv_back(&data->serv_list, serv_new()); // añadimos nuevo server creado
			Server	S;
			this->serv_vec.push_back(S); // añadimos nuevo server creado
		}
		if (line.at(0) == '\t') // si leemos un tab, significa que viene una instrucción para el server
		{
			if (parse_element(line)) //parseamos la instrucción
			{
				print_error("Wrong config file format.\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

int		ConfFile::parse_element(std::string &line)
{
	if (!line.compare(0, 7, "\tlisten")) // si recibimos petición
	{
		size_t space_pos = line.find(' ');
		size_t semicolon_pos = line.find(';');
		std::string port = line.substr(space_pos + 1, semicolon_pos - space_pos - 1);
		if (port.find_first_not_of("0123456789") != std::string::npos)
			return (1);
		//sock_back(&list->serv->sock_list, sock_new(port));
		Socket S;
		S.port = port;
		this->serv_vec.end()->sock_vec.push_back(S);
	}
	return (0);
}

void	ConfFile::print_servers()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{
		std::cout << "server " << std::endl;
		for (size_t j = 0; j < this->serv_vec[i].sock_vec.size(); j++)
			std::cout << "socket: \n" << this->serv_vec[i].sock_vec[j].port << std::endl;
	}
}

void	ConfFile::init_serv()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
		for (size_t j = 0; j < this->serv_vec[i].sock_vec.size(); j++)
			this->serv_vec[i].sock_vec[j].start();
}

void	ConfFile::init_poll()
{
	int i = 0;
	//for (t_serv *ptr = lst; ptr != NULL; ptr = ptr->next) 
	for (size_t i = 0; i < this->serv_vec.size(); i++) // recorremos todos los servers
	{
		//i += sock_count(ptr->serv->sock_list);
		i += this->serv_vec[i].sock_vec.size(); // contamos todos los sockets de todos los servers
	}
 	this->poll = new struct pollfd[i]; // reservamos tantos polls como sockets haya
	i = 0;
	//for (t_serv *ptr = lst; ptr != NULL; ptr = ptr->next)
	for (size_t i = 0; i < this->serv_vec.size(); i++) // recorremos todos los servers
	{
		//for (t_sock *sock_ptr = ptr->serv->sock_list; sock_ptr != NULL; sock_ptr = sock_ptr->next)
		for (size_t j = 0; j < this->serv_vec[i].sock_vec.size(); j++) // recorremos todos los sockets de cada server
		{
			//this->poll[i].fd = sock_ptr->sock->s_fd; // asignamos el fd de cada socket a un poll
			this->poll[i].fd = this->serv_vec[i].sock_vec[j].s_fd; // asignamos el fd de cada socket a un poll
			this->poll[i].events = POLLIN; // ?
			i++;
		}
	}
	this->fd_size = i;
	this->fd_count = i;
}

void	ConfFile::poll_loop()
{
	/*datos para nueva conexion*/
	int c_fd;
    struct sockaddr_in c_addr;
    socklen_t addrlen;
	/*datos para nueva conexion*/
	
	while (1)
	{
		this->poll = NULL;
		int poll_count = poll(this->poll, this->fd_size, -1); // ?

		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < this->fd_size; i++) // buscamos que socket esta listo para recibir cliente
		{
			if (this->poll[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{
				//if (check_if_listener(this->poll[i].fd, list)) // comentamos de momento
				//{
					/*aceptamos nueva conexion, y gestionamos inmediatamente peticion cliente, ya que subject
					especifica solo UN POLL, para I/O entre cliente y servidor*/
				addrlen = sizeof (c_addr);
				c_fd = accept(this->poll[i].fd, (struct sockaddr *) &c_addr, &addrlen); // el cliente acepta el socket
				
				if (c_fd == -1)
					print_error("client accept error");
				else
				{
					handle_client(c_fd); // gestionamos cliente inmediatamente, efectuando I/O entre cliente y servidor en un poll
					close(c_fd);
				//	add_pollfd(&this->poll, c_fd, &this->fd_count, &this->fd_size);
				//	cout << "pollserver: new connection" << endl;
				}
			}
				//else /*si no es listener, es peticion de cliente*/
				//{
				//	if (handle_client(this->poll[i].fd))
				//	{	/*devuelve uno, conexion terminada o error en recv*/
				//		close(this->poll[i].fd);   /*cerramos fd y eliminamos de array pollfd*/
				//		remove_pollfd(&this->poll, i, &this->fd_count); 
				//	}
					/*si ha devuelto cero, peticion ha sido resuelta y la conexion sigue abierta*/
				//}
		}
	}
}