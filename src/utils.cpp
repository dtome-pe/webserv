#include "../inc/webserv.hpp"

void print_error(const char *str)
{
	cerr << str << endl;
}

std::string ip_to_str(sockaddr_in *addr)
{
	std::ostringstream ipStream;
	uint32_t ipAddress = ntohl(addr->sin_addr.s_addr);

	ipStream << ((ipAddress >> 24) & 0xFF) << "."
             << ((ipAddress >> 16) & 0xFF) << "."
             << ((ipAddress >> 8) & 0xFF) << "."
             << (ipAddress & 0xFF);
	return (ipStream.str());	// volcamos ip de network byte order a string.
}

std::string port_to_str(sockaddr_in *addr)
{
	std::stringstream ss;
	ss << ntohs(addr->sin_port);

	std::string portStr = ss.str();

	return (portStr);
}

/*nginx puede tener varios server blocks escuchando en la misma direccion:puerto, pero dos sockets no pueden
bindearse al mismo puerto. Entonces, nginx debe primero resolver ip y puerto de cada listen directive uno a uno, y
en el momento que encuentra otro listen con la misma direccion exacta, se lo debe saltar. Ya que al final, solo
un server block va a gestionar peticion, nginx simplemente abre un puerto, y luego lo redirige al server que toca. Por
eso esta funcion va comprobando los Host:Port de cada server, si encuentra uno identico, no lo anade al vector y
lo ignoraremos, para que no haya un socket duplicado y que no de error. si encuentra un socket que va a apuntar a 0.0.0.0
y uno que va a apuntar a una direccion concreta y ambos al mismo puerto, el 0.0.0.0 tiene prioridad*/

bool look_for_same(Socket &sock, std::vector<Socket>&sock_vec)
{
	for (std::vector<Socket>::iterator it = sock_vec.begin(); it != sock_vec.end(); it++)
	{
		/*si encontramos un socket creado con misma direccion:puerto, retornamos true y no se anade al vector*/
		if (it->getIp() == sock.getIp() && it->getPort() == sock.getPort())
			return (true);
		if (it->getPort() == sock.getPort() && it->getIp() == "0.0.0.0")
			return (true);
		if (it->getPort() == sock.getPort() && sock.getIp() == "0.0.0.0" && it->getIp() != "0.0.0.0")
		{
			sock_vec.erase(it);
			return (false);
		}
	}
	return (false);
}