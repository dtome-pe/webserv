#include<webserv.hpp>

HeaderHTTP::HeaderHTTP()
{
	
}

HeaderHTTP::~HeaderHTTP()
{

}

HeaderHTTP::HeaderHTTP(const HeaderHTTP &header)
{
	this->vec = header.vec;
	this->map = header.map;
}

HeaderHTTP &HeaderHTTP::operator=(const HeaderHTTP &header)
{
	this->vec = header.vec;
	this->map = header.map;
	return (*this);
}

void		HeaderHTTP::setHeader(std::string _header)
{	
	vec.push_back(_header);
	int n = _header.find(':');
	std::string name = _header.substr(0, n);
	std::string value = _header.substr(n + 2, _header.length());
	this->map[name] = value;	
}

std::string	HeaderHTTP::getHeader(std::string name)
{
	std::map<std::string, std::string>::iterator it = map.begin();
	
	while (it != map.end())
	{
		if (it->first == name)
			return it->second;
		it++;
	}
	return ("not found");
}

std::string	HeaderHTTP::makeHeader()
{
	std::string text;
	for (size_t i = 0; i < vec.size(); i++)
		text += vec[i] + "\r\n";
	return (text);
}

std::vector<std::string> HeaderHTTP::split(const std::string& input, const std::string& delimiters) 
{
    std::vector<std::string> tokens;
    std::size_t startPos = 0;

    while (true) 
	{
        std::size_t foundPos = input.find_first_of(delimiters, startPos);
        
        if (foundPos != std::string::npos) {
            // Extraer el token desde startPos hasta foundPos
            std::string token = input.substr(startPos, foundPos - startPos);
            tokens.push_back(token);
            
            // Actualizar la posición inicial para la próxima búsqueda
            startPos = foundPos + 1;
        } else {
            // Si no se encuentran más delimitadores, agregar el resto de la cadena
            std::string token = input.substr(startPos);
            tokens.push_back(token);
            break;
        }
    }

    return tokens;
}

void	HeaderHTTP::removeHeader(std::string header)
{
	map.erase(header);

	for (unsigned int i = 0; i < vec.size(); i++)
	{
		if (!vec[i].substr(0, header.length()).compare(header))
			vec.erase(vec.begin() + i);
	}

}

void	HeaderHTTP::clear()
{
	map.clear();
	vec.clear();
}

void	HeaderHTTP::printHeaders()
{	
	std::map<std::string, std::string>::iterator it = map.begin();

	while (it != map.end())
	{
		cout << "Header: " << it->first << " Value: " << it->second
			<< endl;
		it++;
	}
}