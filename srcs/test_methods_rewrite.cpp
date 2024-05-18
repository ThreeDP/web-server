#include "./Parser/Parser.hpp"

int main()
{
    std::vector<Server>     servers;
    Http http;
	char path_file[] = "./Parser/test_metodos/nginx.conf";

    try {
	    Parser obj(path_file);
        obj.ParserServer(http);

        std::cout << "Métodos permitidos: " << std::endl;
        if (http.GetServer("www.www.example.com") == NULL) {
            return 1;
        }
        std::vector<std::string> &methods = (http.GetServer("www.example.com")->routes["<path>"])->GetAllowMethods();
        for(std::vector<std::string>::size_type i = 0; i < methods.size(); ++i){
            std::cout << methods[i] << " ";
        }
        std::cout << std::endl;

        std::map<std::string, std::string> &it = http.GetServer("www.www.example.com")->routes["/blog"]->GetRedirectPath();
        std::cout << "De: " << "/blog" << " Para: " << it["/blog"] << std::endl;
        
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

	return(0);
}
