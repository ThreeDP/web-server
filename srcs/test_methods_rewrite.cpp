#include "./Parser/Parser.hpp"

int main()
{
    std::vector<Server>     servers;
	char path_file[] = "./Parser/test_metodos/nginx.conf";

    try {
	    Parser obj(path_file);
        servers = obj.ParserServer();

        std::cout << "Métodos permitidos: " << std::endl;
        std::vector<std::string> &methods = (servers.back().routes["<path>"])->GetAllowMethods();
        for(std::vector<std::string>::size_type i = 0; i < methods.size(); ++i){
            std::cout << methods[i] << " ";
        }
        std::cout << std::endl;

        std::map<std::string, std::string> &it = servers.back().routes["/blog"]->GetRedirectPath();
        std::cout << "De: " << "/blog" << " Para: " << it["/blog"] << std::endl;
        
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }



    // std::map<std::string, std::string> rewrites = obj.test_parseRewrites();

    // std::cout << "Resultado: " << std::endl;
    // for(std::map<std::string, std::string>::const_iterator it = rewrites.begin(); it != rewrites.end(); ++it){
    //     std::cout << "De: " << it->first << " Para: " << it->second << std::endl;
    // }

	return(0);
}
