#ifndef HEADERHTTP_HPP
#define HEADERHTTP_HPP

#include <webserv.hpp>

//#include<webserv.hpp>

class HeaderHTTP
{
	public:
		HeaderHTTP();
		~HeaderHTTP();
		HeaderHTTP(const HeaderHTTP &header);
		HeaderHTTP &operator=(const HeaderHTTP &header);

		std::vector<std::string> 			vec;
		std::map<std::string, std::string>	map; // map de headers

		std::string							getHeader(std::string name); // devuelve valor del header
		void								setHeader(std::string header); // introducir nuevo header (con nombre)

		static std::vector<std::string>		split(const std::string& input, const std::string& delimiters);
		void								printHeaders();
		std::string							makeHeader();

		void								removeHeader(std::string header);

		void								clear();
};

#endif