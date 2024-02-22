#include <webserv.hpp>

static bool caseInsensitiveCompare(char c1, char c2) {
    return std::toupper(static_cast<unsigned char>(c1)) == std::toupper(static_cast<unsigned char>(c2));
}

std::string getCgiHeader(const std::string& content, const std::string &header) 
{
    std::string::const_iterator pos = content.begin();

    while (true) 
	{
        pos = std::search(pos, content.end(), header.begin(), header.end(), caseInsensitiveCompare);

        if (pos == content.end()) {
            break;  // no se encontró header
        }
        // Check if "Content-Type:" is at the beginning of a line or the start of the content
        if (pos == content.begin() || *(pos - 1) == '\n' || *(pos - 1) == '\r') 
		{
            break;
        }
        // Iteramos para seguir buscando
        ++pos;
    }

    if (pos != content.end()) {
        // Extract the value of the "Content-Type" header
        std::string::const_iterator start = pos + header.size(); // Move past "Content-Type:"
        std::string::const_iterator end = std::find_if(start, content.end(), std::bind2nd(std::equal_to<int>(), '\n'));

        // Trim leading and trailing whitespace
        std::string::const_iterator first = std::find_if(start, end, std::not1(std::ptr_fun<int, int>(std::isspace)));
        std::string::const_iterator last = std::find_if(std::reverse_iterator<std::string::const_iterator>(end), std::reverse_iterator<std::string::const_iterator>(first), std::not1(std::ptr_fun<int, int>(std::isspace))).base();

        // Return the trimmed value
        return std::string(first, last);
    }

    // No se encontró header
    return "";
}

std::string removeHeaders(std::string& content) 
{
    std::string target[3] = {"Content-Type:", "Location:", "Status:"};
	for (unsigned int i = 0; i < 3; i++)
	{
		std::string::iterator pos = content.begin();

    	while (true) 
		{
			pos = std::search(pos, content.end(), target[i].begin(), target[i].end(), caseInsensitiveCompare);

			if (pos == content.end()) {
				break;  // Header not found
			}

			// Check if "Content-Type:" is at the beginning of a line or the start of the content
			if (pos == content.begin() || *(pos - 1) == '\n' || *(pos - 1) == '\r') {
				break;
			}

			// Move to the next character to continue the search
			++pos;
   		}

    	if (pos != content.end()) 
		{
			// Find the end of the line or content after the header
			std::string::iterator end = std::find_if(pos, content.end(), std::bind2nd(std::equal_to<int>(), '\n'));

			// Erase the header and the following line break
			content.erase(pos, end);

			// Remove any leading whitespace after erasing the header
			content.erase(std::find_if(content.begin(), content.end(), std::not1(std::ptr_fun<int, int>(std::isspace))), content.begin());
    	}
	}
	return content;
}