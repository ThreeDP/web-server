#include "./Parser/Parser.hpp"

int main()
{
	Parser obj;

	char path_file[] = "./Parser/test_metodos/nginx.conf";

    std::vector<std::string> methods = obj.test_parseAllowMethods(path_file);

    std::cout << "Métodos permitidos: " << std::endl;
    for(std::vector<std::string>::size_type i = 0; i < methods.size(); ++i){
        std::cout << methods[i] << " ";
    }
    std::cout << std::endl;

    std::map<std::string, std::string> rewrites = obj.test_parseRewrites(path_file);

    std::cout << "Resultado: " << std::endl;
    for(std::map<std::string, std::string>::const_iterator it = rewrites.begin(); it != rewrites.end(); ++it){
        std::cout << "De: " << it->first << " Para: " << it->second << std::endl;
    }

	return(0);
}
