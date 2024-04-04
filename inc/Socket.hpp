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
		int					_cgiFd;

		bool				_continue;
		bool				_cgi;
		std::string			_previousRequestLine;
		HeaderHTTP			_previousHeaders;

	
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

		bool			getContinue();
		std::string		getPreviousRequestLine();
		HeaderHTTP		&getPreviousHeaders();

		bool			getCgi();
		int				getCgiFd();

		//setters
		void			setPort(std::string port);
		void			setIp(std::string ip);
		void			setHost(std::string host);
		void			setFd(int fd);

		void			bouncePrevious(Request &request, int type);

		void			setPreviousRequestLine(std::string text);
		void			setPreviousHeaders(HeaderHTTP headers);

		void			setContinue(bool c);
		void			setCgi(bool cgi);
		void			setCgiFd(int fd);
};

#endif
