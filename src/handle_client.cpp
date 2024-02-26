#include<webserv.hpp>

int	handle_client(int new_socket, ConfFile &conf, Socket &listener, std::string &text)
{
	Request	req(text, listener);

	const Server *serv = find_serv_block(conf.getServerVector(), req);

	const Locations *loc = find_loc_block(serv, req);

 	Response	msg(req, serv, loc);

	std::string response = msg.makeResponse(); // hacemos respuesta con los valores del clase Response

	send(new_socket, response.c_str(), response.length(), 0);

	cout << "sent response" << endl;

	return (0);
}
