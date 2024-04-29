#include <webserv.hpp>

void        createSocketAndAddToSockVecIfDifferent(std::vector<Server> &servVec,  std::vector<Socket> &sockVec, int i)
{
    for (size_t j = 0; j < servVec[i].host_port.size(); j++)
	{
        Socket s(servVec[i].host_port[j], &servVec[i]);		
        if (!look_for_same(s, sockVec))
            sockVec.push_back(s);
    }
}

void        startSocketAndAddToPollFd(std::vector<Socket> &sockVec, std::vector<pollfd> &pollVec, int i)
{
    sockVec[i].start();
	pollfd node;
	node.fd = sockVec[i].getFd();
	node.events = POLLIN;
	pollVec.push_back(node);
}

void        setSignals()
{
    signal(SIGINT, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);   
}

int         createNonBlockingClientSocketAndAddToPollAndSock(struct sockaddr_in c_addr, std::vector<pollfd> &pollVec, int i, int c_fd, std::vector<Socket> &sockVec)
{
    Socket client(ip_to_str(&c_addr) + port_to_str(&c_addr), NULL);
    cout << "i: " << i << " client " << c_fd << " with ip: " << ip_to_str(&c_addr) << ":" << port_to_str(&c_addr) << " added. It points to fd " << pollVec[i].fd << endl;
    client.pointTo(pollVec[i].fd);
    client.setFd(c_fd);
    if (client.setNonBlocking(c_fd) == 1)
        return (1);
    add_pollfd(pollVec, sockVec, client, c_fd, false);
    return (0);
}

int         handleAcceptError()
{
    if (errno != EAGAIN && errno != EWOULDBLOCK)
        return 1;
    else
        return 0;
}

void        readNothing(Socket &client, std::vector<pollfd> &pollVec)
{
	cout << "entra en readNothing" << endl;
    client.addToClient("", client.getRequest()->getCgi(), 0);
    pollVec[findPoll(pollVec, client)].events = POLLIN | POLLOUT;
    client.getRequest()->otherInit();   
}

void        readEnough(int ret, std::vector<pollfd> &pollVec, Socket &client, int i)
{
    if (ret == DONE)
    {
        pollVec[i].events = POLLIN | POLLOUT; // vamos anadiendo a request, si request ha acabado, pondriamos fd en pollout
        client.getRequest()->otherInit();
    }
    else if (ret == DONE_ERROR)
    {
        close(pollVec[i].fd);
        client.getPoll()->events = POLLIN | POLLOUT; // vamos anadiendo a request, si request ha acabado, pondriamos fd en pollout
        client.getRequest()->otherInit();
    }
}

void           set400AndCloseConnection(Cluster &cluster, Socket &client, Request &req, unsigned int i)
{
    client.getResponse()->setResponse(400, req);
    cluster.closeConnection(i, cluster.getPollVector(), cluster.getSocketVector());

}

static bool     unallowedCharacterInRequestURI(std::string uri)
{
        const std::string unallowedChars = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
                                        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x7F"
                                        "\"<>^`{|}";
        
        for (size_t i = 0; i < uri.length(); ++i) {
        char ch = uri[i];

        for (size_t j = 0; j < sizeof(unallowedChars) - 1; ++j) {
            if (ch == unallowedChars[j]) {
                return true;
            }
        }
    }
    return false;
}

void            setResponse(Cluster &cluster, Socket &client, Request &req, unsigned int i)
{
    if (client.getResponse()->getCode() != "")
		client.getResponse()->setResponse(str_to_int(client.getResponse()->getCode()), req);
	else
	{
		if (req.getCgi())
			client.getResponse()->setResponse(cgi((*client.getResponse()), req, "", "output"), req);
		else
		{
			if (!req.good)
				set400AndCloseConnection(cluster, client, req, i);
			else if (req.getMethod() != "GET" && req.getMethod() != "PUT" && req.getMethod() != "DELETE" && req.getMethod() != "POST")
				client.getResponse()->setResponse(501, req);
            else if (req.getHeader("Transfer-Encoding") != "not found" && req.getHeader("Transfer-Encoding") != "chunked")
                client.getResponse()->setResponse(501, req);
            else if (unallowedCharacterInRequestURI(req.getTarget()))
                client.getResponse()->setResponse(400, req);
            else if (req.getTarget().length() > 2048)
                client.getResponse()->setResponse(414, req);
            else if (req.getHeader("Transfer-Encoding") == "not found" && req.getHeader("Content-Length") == "not found" && req.getMethod() == "POST")
                client.getResponse()->setResponse(400, req);
			else
				client.getResponse()->setResponse(client.getResponse()->getResponseCode(req, req.getServer(), req.getLocation()), req);
		}	
	}
}

int             sendResponseAndReturnCode(Socket &client)
{
	int result = 0;

    std::string response = client.getResponse()->makeResponse();
	result = send(client.getFd(), response.c_str(), response.length(), 0);
	if (result != -1 && result != 0)
		return (result);
	return (str_to_int(client.getResponse()->getCode()));   
}

void            clearClientAndSetPoll(Socket &client, std::vector<pollfd> &pollVec, int i)
{
    pollVec[i].events = POLLIN;
	client.setResponse(NULL);
	client.setRequest(NULL);
}

void            closeCgiFd(unsigned int i, std::vector<pollfd> &pollVec, Socket &client)
{
    close(client.getCgiFd());
    client.setCgiFd(-1);
    pollVec[i].fd = -1;
}

void            killZombieProcess(std::vector<pidStruct>::iterator &it, std::vector<pidStruct> &pidVec)
{
    close(it->fd);
    kill(it->pid, SIGKILL);
    it = pidVec.erase(it);
}

void            killTimeoutProcessAndDisconnectClient(Cluster &cluster, std::vector<pollfd> &pollVec, std::vector<pidStruct> &pidVec, std::vector<Socket> &sockVec, std::vector<pidStruct>::iterator &it)
{
    for (unsigned int j = 0; j < pollVec.size(); j++)
    {
        if (pollVec[j].fd == it->fd)
            cluster.closeConnection(j, pollVec, sockVec);
        else if (pollVec[j].fd == it->client->getFd())
            cluster.closeConnection(j, pollVec, sockVec);
    }
    close(it->fd);
    kill(it->pid, SIGKILL);
    waitpid(it->pid, NULL, 0);
    it = pidVec.erase(it);
}

void    deleteRequestAndResponse(Request *request, Response *response)
{
    delete request;
    delete response;
}