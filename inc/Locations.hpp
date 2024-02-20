

#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include <iostream>

class Locations {
	private:
		bool autoindex;
		std::string location;
		std::string index;
		int methods[3]; //[0] = GET, [1] = POST, [2] = DELETE. Si es 0 no permitido si es 1 permitido.
		std::string redirection;
		std::string root;
		std::string allurl;
	public:
		Locations();
		~Locations();
		void setAutoindex(bool autoin);
		void setIndex(std::string idx);
		void setMethods(int met[3]);
		void setLocation(std::string loc);
		void setRedirection(std::string red);
		void setRoot(std::string rt);
		void setAllUrl(std::string url);
		bool getAutoindex() const;
		std::string getIndex();
		const int *getMethods() const;
		const std::string getLocation() const;
		std::string getRedirection();
		const std::string getRoot() const;
		std::string getAllUrl();
};

#endif
