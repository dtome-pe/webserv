#include<webserv.hpp>

static	int	receive_response(int new_socket, std::vector<unsigned char> *buff)
{
	int	result;

	result = recv(new_socket, buff->data(), 5000, 0);
	if (result != -1)
	{
		(*buff).resize(result);
		return (result);
	}
	else
		return (result);
}

int	handle_client(int new_socket, ConfFile &conf, sockaddr_in &c_addr, sockaddr_in &sock_addr)
{	
	(void) conf;

	int	nbytes;
	std::vector<unsigned char> buff(5000);
	nbytes = receive_response(new_socket, &buff); // recibimos respuesta (recv)
	if (nbytes <= 0)
	{
		/*error o conexion cerrada*/
		if (nbytes == 0)
			cout << "connexion was closed with client" << endl;
		if (nbytes == -1)
			print_error("recv error");
		return (1);
	}
	//print_request(buff);
	std::string	text;
	for (size_t i = 0; i < buff.size(); i++)
		text += buff[i];
	Request	req(text, c_addr, sock_addr);

	const Server *serv = find_serv_block(conf.getServerVector(), req);

	cout << req.getTarget() << endl;
	//std::cout << "selected server is server " << serv->id << std::endl;

	const Locations *loc = find_loc_block(serv, req);

 	Response	msg(req, serv, loc);

	std::string response = msg.makeResponse(); // hacemos respuesta con los valores del clase Response

	send(new_socket, response.c_str(), response.length(), 0);

	return (0);
}
