#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include <poll.h>

#define B_LENGTH 5
#define B_CHUNKED 6
#define B_CGI 7

class Response;

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

		std::string			_textRead;
		bool				_readAll;

		int					_bodyType;

		Request				*_request;
		Response			*_response;

		pollfd				*_poll;
		pollfd				*_cgiPoll;

	
	public:
		class Server 			*serv;
		Socket(std::string host_port, Server *s_ptr);

		int					pointingTo;
		int					listener;
		struct addrinfo 	*s_addr;
		struct sockaddr_in 	sock_addr;
		socklen_t 			sock_addrlen;
		
		
		int			start();

		int				get_addr_info(struct addrinfo **s_addr, const char *host, const char *port);
		int				create_s(struct addrinfo *s_addr, struct sockaddr_in sock_addr, socklen_t sock_addrlen);
		int 			bind_s(struct addrinfo *s_addr);
		int 			listen_s();

		int				addToClient(std::string text, bool cgi, int type);

		/*cliente*/
		void			pointTo(int fd);
		int				setNonBlocking(int fd);

		//getters
		std::string		getPort() const;
		std::string		getIp() const;
		int				getFd() const;

		bool			getContinue();
		std::string		getPreviousRequestLine();
		HeaderHTTP		&getPreviousHeaders();

		bool			getCgi();
		int				getCgiFd();

		std::string		&getTextRead();

		bool			getReadAll();

		Request			*getRequest();
		Response		*getResponse();

		//setters
		void			setPort(std::string port);
		void			setIp(std::string ip);
		void			setHost(std::string host);
		void			setFd(int fd);

		pollfd			*getPoll();
		pollfd			*getCgiPoll();

		void			bouncePrevious(Request &request, int type);

		void			setPreviousRequestLine(std::string text);
		void			setPreviousHeaders(HeaderHTTP headers);

		void			setContinue(bool c);
		void			setCgi(bool cgi);
		void			setCgiFd(int fd);

		void			setPoll(pollfd *node);
		void			setCgiPoll(pollfd *node);

		void			setRequest(Request *request);
		void			setResponse(Response *response);
		
		void			setReadAll(bool readAll);

		void			setWaitingForBody(bool waiting);
		void			setBodyType(int type);

		void			appendTextRead(std::string text);
		void			setTextRead(std::string text);
};

#endif
