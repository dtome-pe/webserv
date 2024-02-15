#include "webserv.hpp"

/* 
static Server *ip_port(std::vector<class Server> &serv, Request &request)
{
	Serv *	ret = NULL;

	// a cada server
	for(int i = 0; i < serv.size(); i++)  
	{
		// comprobamos cada ip y puerto por el que escucha
		for (int i = 0; i < sock_vec; i++)
		{	
			//y lo chequeamos con ip y puerto de conexion entrante (ip y puerto) que se ha guardado en objeto Request
			if (request.ip == sock_vec[i].getIp() || sock_vec[i].getIp == "" && request.port == sock_vec[i].getPort())
			{	
				//si es el primer match, lo guardamos como posible retorno, y salimos del loop de los sockets, ya que
				// este server block seria candidato igualmente
				if (!ret)
				{
					ret = &serv[i];
					break ;
				}
				//si no es el primer match, salimos y devolvemos NULL, al haber varios candidatos por direccion:puerto,
				//habra que ir a mirar el server_name para encontrar un candidato.
				if (ret)
					return (NULL);
			}
		}
	}
	return (ret);
}

static Server *server_name(std::vector<class Server> &serv, Request &request)
{
	Serv *ret;

	for(int i = 0; i < serv.size(); i++)  
	{	
		std::vector<std::string> &serv_name = serv[i].getServNames();
		for (int i = 0; i < serv_name.size(); i++)
		{
			if (serv_name[i] == request.host)
			{	
				ret = &serv[i]
				return (ret);
			}
		}
	}
	return (NULL);
}

static Server *get_first_block(std::vector<class Server> &serv, Request &request)
{
	// a cada server
	for(int i = 0; i < serv.size(); i++)  
	{
		// comprobamos cada ip y puerto por el que escucha
		for (int i = 0; i < sock_vec; i++)
		{	
			//y lo chequeamos con ip y puerto de conexion entrante (ip y puerto) que se ha guardado en objeto Request
			if (request.ip == sock_vec[i].getIp() || sock_vec[i].getIp == "" && request.port == sock_vec[i].getPort())
			{	
				//si es el primer match, lo retornamos y se acabo
				return (&serv[i]);
			}
		}
	}
	return (NULL);
}

static Server *find_serv_block(std::vector<class Server> &serv, Request &request)
{	
	Serv *block;
	
	block = ip_port(serv, request);  //primero buscamos si solo hay un match por direccion y puerto
	if (!block) // si ha devuelto nulo, es que hay mas de un server block con ip:puerto y hay que buscar ahora por server_name
	{
		block = server_name(serv, request);
		if (!block) // si ha devuelto nulo, es que no ha encontrado match con server_name
			return (get_first_block(serv, request)) // asi que hay que devolver primer server que encaje con ip:puerto
	}
	return (block);
} 
*/