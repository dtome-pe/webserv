#include <webserv.hpp>

std::map<std::string, std::vector<std::string> > MIME::mime;

void        MIME::initializeMIME()
{
    mime["text/html"].push_back("html");
    mime["text/html"].push_back("htm");
    mime["text/html"].push_back("shtml");
    mime["text/css"].push_back("css");
    mime["text/xml"].push_back("xml");

    mime["image/gif"].push_back("gif");
    mime["image/jpeg"].push_back("jpeg");
    mime["image/jpeg"].push_back("jpg");

    mime["application/x-javascript"].push_back("js");
    mime["application/atom+xml"].push_back("atom");
    mime["application/rss+xml"].push_back("rss");
   
    mime["text/mathml"].push_back("mml");
    mime["text/plain"].push_back("txt");
    mime["text/vnd.sun.j2me.app-descriptor"].push_back("jad");
    mime["text/vnd.wap.wml"].push_back("wml");
    mime["text/x-component"].push_back("htc");

    mime["image/png"].push_back("png");
    mime["image/tiff"].push_back("tif");
    mime["image/tiff"].push_back("tiff");
    mime["image/vnd.wap.wbmp"].push_back("wbmp");
    mime["image/x-icon"].push_back("ico");
    mime["image/x-jng"].push_back("jng");
    mime["image/x-ms-bmp"].push_back("bmp");
    mime["image/svg+xml"].push_back("svg");
    mime["image/webp"].push_back("webp");

    mime["application/java-archive"].push_back("jar");
    mime["application/java-archive"].push_back("war");
    mime["application/java-archive"].push_back("ear");
    mime["application/mac-binhex40"].push_back("hqx");
    mime["application/msword"].push_back("doc");
    mime["application/pdf"].push_back("pdf");
    mime["application/postscript"].push_back("ps");
    mime["application/postscript"].push_back("eps");
    mime["application/postscript"].push_back("ai");
    mime["application/rtf"].push_back("rtf");
    mime["application/vnd.ms-excel"].push_back("xls");
    mime["application/vnd.ms-powerpoint"].push_back("ppt");
    mime["application/vnd.wap.wmlc"].push_back("wmlc");
    mime["application/vnd.google-earth.kml+xml"].push_back("kml");
    mime["application/vnd.google-earth.kmz"].push_back("kmz");
    mime["application/x-7z-compressed"].push_back("7z");
    mime["application/x-cocoa"].push_back("cco");
    mime["application/x-java-archive-diff"].push_back("jardiff");
    mime["application/x-java-jnlp-file"].push_back("jnlp");
    mime["application/x-makeself"].push_back("run");
    mime["application/x-perl"].push_back("pl");
    mime["application/x-perl"].push_back("pm");
    mime["application/x-pilot"].push_back("prc");
    mime["application/x-pilot"].push_back("pdb");
    mime["application/x-rar-compressed"].push_back("rar");
    mime["application/x-redhat-package-manager"].push_back("rpm");
    mime["application/x-sea"].push_back("sea");
    mime["application/x-shockwave-flash"].push_back("swf");
    mime["application/x-stuffit"].push_back("sit");
    mime["application/x-tcl"].push_back("tcl");
    mime["application/x-tcl"].push_back("tk");
    mime["application/x-x509-ca-cert"].push_back("der");
    mime["application/x-x509-ca-cert"].push_back("pem");
    mime["application/x-x509-ca-cert"].push_back("crt");
    mime["application/x-xpinstall"].push_back("xpi");
    mime["application/xhtml+xml"].push_back("xhtml");
    mime["application/zip"].push_back("zip");

    mime["application/octet-stream"].push_back("bin");
    mime["application/octet-stream"].push_back("exe");
    mime["application/octet-stream"].push_back("dll");
    mime["application/octet-stream"].push_back("deb");
    mime["application/octet-stream"].push_back("dmg");
    mime["application/octet-stream"].push_back("eot");
    mime["application/octet-stream"].push_back("iso");
    mime["application/octet-stream"].push_back("img");
    mime["application/octet-stream"].push_back("msi");
    mime["application/octet-stream"].push_back("msp");
    mime["application/octet-stream"].push_back("msm");
    
    mime["audio/midi"].push_back("mid");
    mime["audio/midi"].push_back("midi");
    mime["audio/midi"].push_back("kar");
    mime["audio/mpeg"].push_back("mp3");
    mime["audio/ogg"].push_back("ogg");
    mime["audio/x-realaudio"].push_back("ra");
   
    mime["video/3gpp"].push_back("3gpp");
    mime["video/3gpp"].push_back("3gp");
    mime["video/mpeg"].push_back("mpeg");
    mime["video/mpeg"].push_back("mpg");
    mime["video/quicktime"].push_back("mov");
    mime["video/x-flv"].push_back("flv");
    mime["video/x-mng"].push_back("mng");
    mime["video/x-ms-asf"].push_back("asx");
    mime["video/x-ms-asf"].push_back("asxf");
    mime["video/x-ms-wmv"].push_back("wmv");
    mime["video/x-msvideo"].push_back("avi");
    mime["video/mp4"].push_back("m4v");
    mime["video/mp4"].push_back("mp4");
}

std::string MIME::getMIMEType(const std::string &extension)
{
    // Iterate through the map
    std::map<std::string, std::vector<std::string> >::const_iterator it;
    for (it = mime.begin(); it != mime.end(); ++it) {
        // Iterate through the vector of extensions for the current MIME type
        const std::vector<std::string>& extensions = it->second;
        for (size_t i = 0; i < extensions.size(); ++i) {
            // Check if the given extension matches any in the vector
             if (extensions[i] == extension) {
                return it->first; // Return the MIME type if found
            }
        }
    }
    // si no encuentra tipo, devolvemos text/plain
    return "text/plain";
}