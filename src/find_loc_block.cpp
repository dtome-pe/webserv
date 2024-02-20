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


const Locations *find_loc_block(const Server *serv, Request &req)
{
	const Locations *ret = NULL;

	std::string decoded_str = decode(req.request_line.target); // decodificamos posibles %xx de URI
	
	std::vector<Locations>loc_vec = serv->getLocations();
	std::vector<Locations>matches;

	for (std::vector<Locations>::const_iterator it = loc_vec.begin(); it != loc_vec.end(); it++)
	{
		int len = it->getLocation().length();
		cout << it->getLocation() << " len is " << len << endl;
		if (!req.request_line.target.compare(0, len, it->getLocation()))
			matches.push_back(*it);
	}
	for (std::vector<Locations>::const_iterator it = matches.begin(); it != matches.end(); it++)
	{
		if (!ret)
			ret = &(*it);
		else
		{
			if (it->getLocation().length() > ret->getLocation().length())
				ret = &(*it);
		}
	}
	if (!ret)
		cout << "No location was found" << endl;
	else
		cout << "Location " << ret->getLocation() << " was found." << endl;
	return (ret);
}
