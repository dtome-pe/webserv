#include<webserv.hpp>

static std::string decode(const std::string& encoded) // decodifica caracteres escapados con hexadecimales en URL encoding
{  
	std::ostringstream decoded;

    for (std::size_t i = 0; i < encoded.length(); ++i) 
	{
        if (encoded[i] == '%' && i + 2 < encoded.length()) 
		{
            // Extract the two characters following '%'
            char hex1 = encoded[i + 1];
            char hex2 = encoded[i + 2];

            // Convert hex characters to an integer
            std::istringstream hexStream(std::string("0x") + hex1 + hex2);
            int decodedChar;
            hexStream >> std::hex >> decodedChar;

            // Append the decoded character to the result
            decoded << static_cast<char>(decodedChar);

            // Move the index two characters forward
            i += 2;
        } 
		else 
		{
            // Append non-% characters directly to the result
            decoded << encoded[i];
        }
    }
    return decoded.str();
}


const Location *find_loc_block(const Server *serv, Request &req)
{
	const Location *ret = NULL;

	std::string decoded_str = decode(req.getTarget()); // decodificamos posibles %xx de URI
	
	const std::vector<Location>&locations = serv->getLocations();
	
	std::vector<const Location*>matches;

	for (unsigned int i = 0; i < locations.size(); i++)
	{
		int len = locations[i].getLocation().length();
		if (!req.getTarget().compare(0, len, locations[i].getLocation()))
		{
			matches.push_back(&locations[i]);
		}
	}
	for (unsigned int i = 0; i < matches.size(); i++)
	{
		if (!ret)
			ret = matches[i];
		else
		{
			if (matches[i]->getLocation().length() > ret->getLocation().length())
				ret = matches[i];
		}
	}
	req.setLocation(ret);
	return (ret);
}
