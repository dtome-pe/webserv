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
        pollVec[findPoll(pollVec, client)].events = POLLIN | POLLOUT; // vamos anadiendo a request, si request ha acabado, pondriamos fd en pollout
        client.getRequest()->otherInit();
    }
}

void           set400AndCloseConnection(Cluster &cluster, Socket &client, Request &req, int i, std::vector<pollfd> &pollVec,  std::vector<Socket> &sockVec, unsigned int *size)
{
    client.getResponse()->setResponse(400, req);
    cluster.closeConnection(i, pollVec, sockVec, size);

}

void            setResponse(Cluster &cluster, Socket &client, Request &req, int i, std::vector<pollfd> &pollVec,  std::vector<Socket> &sockVec, unsigned int *size)
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
				set400AndCloseConnection(cluster, client, req, i, pollVec, sockVec, size);
			else if (req.getMethod() != "GET" && req.getMethod() != "PUT" && req.getMethod() != "DELETE" && req.getMethod() != "POST")
				client.getResponse()->setResponse(501, req);
			else
				client.getResponse()->setResponse(client.getResponse()->getResponseCode(req, req.getServer(), req.getLocation()), req);
		}	
	}
}

int             sendResponseAndReturnCode(Socket &client, Request &req)
{
    std::string response = client.getResponse()->makeResponse();
	send(req.getClient().getFd(), response.c_str(), response.length(), 0);
	return (str_to_int(client.getResponse()->getCode()));   
}

void            clearClientAndSetPoll(Socket &client, std::vector<pollfd> &pollVec, int i)
{
    pollVec[i].events = POLLIN;
	client.setResponse(NULL);
	client.setRequest(NULL);
}

void            removeCgiFdFromPollAndClose(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, Request &req)
{
    remove_pollfd(pollVec, sockVec, req.getClient().getCgiFd());
    close(req.getClient().getCgiFd());
    req.getClient().setCgiFd(-1);
}

void            killZombieProcess(std::vector<struct pidStruct> &pidVec, int i)
{
    close(pidVec[i].fd);
    kill(pidVec[i].pid, SIGKILL);
    pidVec.erase(pidVec.begin() + i);
}

void            killTimeoutProcessAndDisconnectClient(Cluster &cluster, std::vector<struct pidStruct> &pidVec, int i, 
                                                            std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, unsigned int *size)
{
    for (unsigned int j = 0; j < pollVec.size(); j++)
    {
        if (pollVec[j].fd == pidVec[i].fd)
            cluster.closeConnection(j, pollVec, sockVec, size);
        else if (pollVec[j].fd == pidVec[i].client->getFd())
            cluster.closeConnection(j, pollVec, sockVec, size);
    }
    close(pidVec[i].fd);
    kill(pidVec[i].pid, SIGKILL);
    waitpid(pidVec[i].pid, NULL, 0);
    pidVec.erase(pidVec.begin() + i);
}