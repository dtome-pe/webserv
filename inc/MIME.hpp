#include <webserv.hpp>

class MIME
{
    public:
        static std::map<std::string, std::vector<std::string> > mime;
        static void         initializeMIME();
        static std::string getMIMEType(const std::string &extension);
};