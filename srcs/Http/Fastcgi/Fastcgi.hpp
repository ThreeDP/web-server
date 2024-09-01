#ifndef __FASTCGI_HPP__
# define __FASTCGI_HPP__

# include "Handler.hpp"
# include "HttpRequest.hpp"

/**!
 * 
 * 
 * 1. Criar uma função para pegar parametros do request e colocar na variavel de ambiente. (envp)
 * 2. Criar processo para executar o execve
 * 7. Achar forma de enviar o script path para o processo filho.  
 * 
 */


class Fastcgi {
	IHandler *_handler;

	public:
		Fastcgi(void) {
		}

		void	Process(HttpRequest &request) {

		}

		void newProcessCGI(HttpRequest &req, int epollFD);
};

#endif