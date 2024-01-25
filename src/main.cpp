#include <iostream>
#include "../inc/Server.hpp"

using namespace std;

int	main()
{
	cout << "webserv!" << endl;
	Server s;

	s.parse_config();
	s.start();
	s.loop();
}
