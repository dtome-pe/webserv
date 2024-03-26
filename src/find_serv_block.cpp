#include "webserv.hpp"


static const Server *ip_port(const std::vector<class Server> &serv, Request &request)
{
	const Server *	ret = NULL;

	for(size_t i = 0; i < serv.size(); i++)  
	{	
		for (size_t j = 0; j < serv[i].ip_port.size(); j++)
		{
			if ((request.getIp() == serv[i].getIp(serv[i].ip_port[j]) ||
					serv[i].getIp(serv[i].ip_port[j]) == "") && request.getPort() == serv[i].getPort(serv[i].ip_port[j]))
			{
				if (!ret)
				{
					ret = &serv[i];
					break ;
				}
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
		for (size_t j = 0; j < serv_name.size(); j++)
		{
			if (serv_name[j] == request.getHost()) // comparamos con lo que hemos cogido del Host header para determinar
											// por server_name
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
		for (size_t j = 0; j < serv[i].ip_port.size(); j++)
		{	
			//si es el primer match, lo retornamos y se acabo
			if ((request.getIp() == serv[i].getIp(serv[i].ip_port[j]) ||
					serv[i].getIp(serv[i].ip_port[j]) == "") && request.getPort() == serv[i].getPort(serv[i].ip_port[j]))
			{	
				return (&serv[i]);
			}
		}
	}
	return (NULL);
}


const Server *find_serv_block(const std::vector<class Server> &serv, Request &request)
{	
	const Server *block;
	
	//cout << "request ip: " << request.ip << "request port: " << request.port << endl;

	block = ip_port(serv, request);  //primero buscamos si solo hay un match por direccion y puerto
	if (!block) // si ha devuelto nulo, es que hay mas de un server block con ip:puerto y hay que buscar ahora por server_name
	{
		block = server_name(serv, request);
		if (!block) // si ha devuelto nulo, es que no ha encontrado match con server_name
			return (get_first_block(serv, request)); // asi que hay que devolver primer server que encaje con ip:puerto
	}
	request.setServer(block);
	return (block);
} 
