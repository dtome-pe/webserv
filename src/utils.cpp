#include "../inc/webserv.hpp"
#include <ctime>

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

	return (ss.str());
}

std::string int_to_str(int n)
{
	std::stringstream ss;
	ss << n;

	return (ss.str());
}

unsigned int	str_to_int(std::string str)
{
 	unsigned int i;
 	std::istringstream(str) >> i;
	
	return (i);
}

// Function to get the current time in RFC 1123 format
std::string getCurrentTime() 
{
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer[80];

    std::time(&rawtime);
    timeinfo = std::gmtime(&rawtime);
	timeinfo->tm_hour += 2;
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return buffer;
}

std::string getLastModified(std::string path) 
{
	struct stat result;
    if (stat(path.c_str(), &result) == 0) 
    {
        std::time_t mod_time = result.st_mtime; // Get last modified time
        std::tm* timeinfo = std::gmtime(&mod_time);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

        return std::string(buffer);
    }
	return ("error");
}

uint64_t timeEpoch()
{
	std::time_t currentTime;
	currentTime = std::time(NULL);
	return (static_cast<uint64_t>(currentTime));
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

bool	checkIfHeader(std::string text)
{
	//cout << "entra en checkifheader. text: " << text << endl;
	if (text.compare(0, 14, "Content-type: ") || text.compare(0, 9, "Status: ") || text.compare(0, 11, "Location: "))
		return (false);
	cout << "check if header true" << endl;
	return (true);
}