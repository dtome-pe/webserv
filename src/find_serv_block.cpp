#include "webserv.hpp"


static const Server *ip_port(const std::vector<class Server> &serv, Request &request)
{
	const Server *	ret = NULL;

	// a cada server
	for(size_t i = 0; i < serv.size(); i++)  
	{
		// comprobamos cada ip y puerto por el que escucha
		for (size_t i = 0; i < serv[i].sock_vec.size(); i++)
		{	
			//y lo chequeamos con ip y puerto de conexion entrante (ip y puerto) que se ha guardado en objeto Request
			if ((request.ip == serv[i].sock_vec[i].getIp() || serv[i].sock_vec[i].getIp() == "") && request.port == serv[i].sock_vec[i].getPort())
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

static const Server *server_name(const std::vector<class Server> &serv, Request &request)
{
	const Server *ret;

	for(size_t i = 0; i < serv.size(); i++)  
	{	
		const std::vector<std::string> &serv_name = serv[i].getVServerName();
		for (size_t i = 0; i < serv_name.size(); i++)
		{
			if (serv_name[i] == request.ip)
			{	
				ret = &serv[i];
				return (ret);
			}
		}
	}
	return (NULL);
}

static const Server *get_first_block(const std::vector<class Server> &serv, Request &request)
{
	// a cada server
	for(size_t i = 0; i < serv.size(); i++)  
	{
		// comprobamos cada ip y puerto por el que escucha
		for (size_t i = 0; i < serv[i].sock_vec.size(); i++)
		{	
			//y lo chequeamos con ip y puerto de conexion entrante (ip y puerto) que se ha guardado en objeto Request
			if ((request.ip == serv[i].sock_vec[i].getIp() || serv[i].sock_vec[i].getIp() == "") && request.port == serv[i].sock_vec[i].getPort())
			{	
				//si es el primer match, lo retornamos y se acabo
				return (&serv[i]);
			}
		}
	}
	return (NULL);
}

const Server *find_serv_block(const std::vector<class Server> &serv, Request &request)
{	
	const Server *block;
	
	block = ip_port(serv, request);  //primero buscamos si solo hay un match por direccion y puerto
	if (!block) // si ha devuelto nulo, es que hay mas de un server block con ip:puerto y hay que buscar ahora por server_name
	{
		block = server_name(serv, request);
		if (!block) // si ha devuelto nulo, es que no ha encontrado match con server_name
			return (get_first_block(serv, request)); // asi que hay que devolver primer server que encaje con ip:puerto
	}
	return (block);
} 
