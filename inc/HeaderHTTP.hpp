#ifndef HEADERHTTP_HPP
#define HEADERHTTP_HPP

#include<iostream>
#include<vector>

//#include<webserv.hpp>

class HeaderHTTP
{
	public:
		HeaderHTTP();
		~HeaderHTTP();

		std::vector<std::string>	header; // lista de headers

		void						setHeader(std::string header); // introducir nuevo header (con nombre)
		std::string					getHeader(std::string name); // devuelve header concreto con nombre
		std::string					getHeaderValue(std::string name); // devuelve header concreto sin nombre
		std::string					makeHeader(); // devuelve el texto del header con el formato completo
};

#endif