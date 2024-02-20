#ifndef LIB_HPP
#define LIB_HPP

// defines
#define _XOPEN_SOURCE_EXTENDED 1

// Our classes
# include <Server.hpp>
# include <Socket.hpp>
# include <Request.hpp>
# include <Response.hpp>
# include <HeaderHTTP.hpp>
# include <ConfFile.hpp>

// Standard class
#include <iostream>
#include <cstddef>
#include <cstdio>

// Element headers
#include <sys/socket.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

// String headers
#include <cstring>
#include <string.h>
#include <string>

// Memory headers
#include <cstdlib>
#include <stdlib.h>

// Stream headers
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

// Net headers
# include <netinet/in.h>
#include <netdb.h>

// Error headers
#include <errno.h>

//#include <bits/stdc++.h> // solo va con gcc

#endif