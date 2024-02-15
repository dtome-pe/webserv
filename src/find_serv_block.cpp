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
			//y lo chequeamos con el Host header (ip y puerto) de Request
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

static Server *find_serv_block(ConfFile &conf, Request &request)
{	
	Serv *block;
	
	block = ip_port(conf.getServVec, request);  //primero buscamos si solo hay un match por direccion y puerto
	if (!block) // si ha devuelto nulo, es que hay mas de un server block con ip:puerto y hay que buscar ahora por server_name
	{
		block = server_name();
	}
} 
*/