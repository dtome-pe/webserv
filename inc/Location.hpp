

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

class Location {
	private:
		bool 									autoindex;
		std::string 							location;

		int 									methods[4]; //[0] = GET, [1] = POST, [2] = DELETE. [3] = PUT. Si es 0 no permitido si es 1 permitido.
		std::string 							redirection;
		int										redirectNum;
		std::string 							root;
		std::string 							allurl;
		std::map<std::string, std::string> 		cgi;
		std::vector<std::string> 				index;
	public:
		Location();
		~Location();
		void 									setAutoindex(bool autoin);
		void									setVIndex(std::vector<std::string> idx);
		void 									setMethods(int met[4]);
		void 									setLocation(std::string loc);
		void 									setRedirection(std::string red);
		void 									setRoot(std::string rt);
		void 									setAllUrl(std::string url);
		void 									setCGI(std::string ext, std::string path);
		std::map<std::string, std::string> 		getCGI() const;
		bool 									getAutoindex() const;
		std::vector<std::string> 				getIndex() const;
		const int 								*getMethods() const;
		const std::string 						getLocation() const;
		const std::string 						getRedirection() const;
		int 									getRedirectionNumber() const;
		const std::string 						getRoot() const;
		std::string 							getAllUrl();
};

#endif
