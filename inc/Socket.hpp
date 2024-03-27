#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include <poll.h>

class Socket
{
	private:
		std::string			_host;
		std::string	 		_ip;
		std::string 		_port;
		int					_fd;
		bool				_continueBool;
		std::string			_continueRequestLine;
		HeaderHTTP			_continueHeaders;

	
	public:

		class Server 			*serv;
		Socket(std::string host_port, Server *s_ptr);

		int					pointingTo;
		int					listener;
		struct addrinfo 	*s_addr;
		struct sockaddr_in 	sock_addr;
		socklen_t 			sock_addrlen;
		Request				*request_ptr;
		
		
		void			start();

		void			get_addr_info(struct addrinfo **s_addr, const char *host, const char *port);
		void			create_s(struct addrinfo *s_addr, struct sockaddr_in sock_addr, socklen_t sock_addrlen);
		void 			bind_s(struct addrinfo *s_addr);
		void 			listen_s();

		/*cliente*/
		void			pointTo(int fd);
		void			setNonBlocking(int fd);

		//getters
		std::string		getPort() const;
		std::string		getIp() const;
		int				getFd() const;

		bool			getContinueBool();
		std::string		getContinueRequestLine();
		HeaderHTTP		&getContinueHeaders();

		//setters
		void			setPort(std::string port);
		void			setIp(std::string ip);
		void			setHost(std::string host);
		void			setFd(int fd);

		void			bounceContinue(Request &request);
		void			setContinueBool(bool c);
		void			setContinueRequestLine(std::string text);
		void			setContinueHeaders(HeaderHTTP headers);			
};

#endif
