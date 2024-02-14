#include<webserv.hpp>

HeaderHTTP::HeaderHTTP()
{
	
}

HeaderHTTP::~HeaderHTTP()
{

}

void		HeaderHTTP::setHeader(std::string _header)
{
	this->header.push_back(_header);
	
	int n = _header.find(':');
	std::string name = _header.substr(0, n);
	std::string value = _header.substr(n + 2, _header.length());
	this->headers[name] = value;
	//std::cout << name << ": " << this->headers[name] << std::endl;
}

std::string	HeaderHTTP::getHeader(std::string name)
{
	for (size_t i = 0; i < this->header.size(); i++)
		if (this->header[i].substr(0, this->header[i].find(':') - 1) == name)
			return (this->header[i]);
	return (NULL);
}
/* 
std::string	HeaderHTTP::getHeaderValue(std::string name)
{
	for (size_t i = 0; i < this->header.size(); i++)
		if (this->header[i].substr(0, this->header[i].find(':') - 1) == name)
			return (this->header[i].substr(this->header[i].find(':')
				, this->header[i].length()));
	return (NULL);
}
 */
std::string	HeaderHTTP::makeHeader()
{
	std::string text;
	for (size_t i = 0; i < this->header.size(); i++)
		text += this->header[i] + "\r\n";
	return (text);
}

std::vector<std::string> HeaderHTTP::split(const std::string& input, const std::string& delimiters) {
    std::vector<std::string> tokens;
    std::size_t startPos = 0;

    while (true) {
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