#include<webserv.hpp>

int doneNothing(Socket &client, std::string &textRead)
{	
	cout << "entra en doneNothing" << endl;
	cout << "textRead: " << textRead << endl;
	cout << "textRead length: " << textRead.size() << endl;
    client.getResponse()->setBody(textRead);
    client.setTextRead("");
    client.getResponse()->waitingForBody = false;
    return (DONE);
}

int doneNoBody(Socket &client)
{
    client.setTextRead("");
	client.getRequest()->setWaitingForBody(false);
	return (DONE);
}

int contentLengthRequestDone(Socket &client, Request *request, std::string &textRead)
{
    if (textRead.length() >= str_to_int(request->getHeader("Content-Length")))
    {
        request->setBody(textRead.substr(0, str_to_int(request->getHeader("Content-Length"))));
        client.setTextRead("");
        client.getRequest()->setWaitingForBody(false);
        return (DONE);
    }
    return (NOT_DONE);
}

int contentLengthCgiOutputDone(Socket &client, Response *response, std::string &textRead, int *flag)
{
    if (textRead.length() >= str_to_int(response->getCgiHeader("Content-Length")))
    {
        response->setBody(textRead.substr(0, str_to_int(response->getCgiHeader("Content-Length"))));
        client.setTextRead("");
        client.getResponse()->waitingForBody = false;
        *flag = 0;
        return (DONE);
    }
    return (NOT_DONE);
}

void parseRequestTillBody(Socket &client, std::string &textRead)
{
    size_t  i = 0;

    while (i < textRead.length())
    {
        if (!client.getRequest()->getWaitingForBody())
        {
            if (textRead[i] == '\r' && (i + 1) < textRead.length() && textRead[i + 1] == '\n')
            {
                client.getRequest()->parseRequest(textRead.substr(0, i + 2));
                textRead = textRead.substr(i + 2, textRead.length());
                i = 0;
                continue ;
            }
        }
        else
            return ;
        i++;
    } 
}

int parseCgiOutputTillBody(Socket &client, std::string &textRead, std::string text, int *flag)
{
    size_t  i = 0;

	cout << "textRead: " << textRead << " length: " << textRead.length() << endl;
    while (i < textRead.length())
    {
        if (!client.getResponse()->waitingForBody)
        {
            if (textRead[i] == '\n')
            {
                if (!*flag)
                {
                    if (!checkIfHeader(text)) // si la primera linea del output no es ninguno de los tres headers esenciales, damos 500. nos aseguramos de que haya algun header.
                    {
                        client.getResponse()->setCode("500");
                        return (DONE_ERROR);
                    }
                    *flag = 1;
                }
                client.getResponse()->parseCgi(textRead.substr(0, i + 1));
                textRead = textRead.substr(i + 1, textRead.length());
                i = 0;
                continue ;
            }
        }
        else
            break ;
        i++;
    }
    return (NOT_DONE);
}