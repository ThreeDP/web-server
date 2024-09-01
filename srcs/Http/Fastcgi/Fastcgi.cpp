# include "Fastcgi.hpp"
#include <sys/wait.h>

void Fastcgi::newProcessCGI(HttpRequest &req, int epollFD) {
// 	std::map<std::string, std::string> expectedHeaders;
// 	expectedHeaders["Host:"] = "www.example.com";
// 	expectedHeaders["User-Agent:"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
// 	expectedHeaders["Accept:"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
// 	expectedHeaders["Accept-Language:"] = "en-US,en;q=0.5";
// 	expectedHeaders["Accept-Encoding:"] = "gzip, deflate, br";
// 	expectedHeaders["Connection:"] = "keep-alive";
// 	expectedHeaders["Upgrade-Insecure-Requests:"] = "1";
// 	std::stringstream requestMessage;
// 	requestMessage << "POST /create-user HTTP/1.1\r\n";
// 	std::map<std::string, std::string>::iterator it = expectedHeaders.begin();
// 	for ( ; it != expectedHeaders.end(); ++it) {
// 		requestMessage << it->first << it->second << "\r\n";
// 	}
// 	requestMessage << "\r\n";
// 	requestMessage << "QUERY_STRING=Rannnnna\n" << "\r\n";

// 	req.ParserRequest(requestMessage.str());


// 	std::vector<std::string> ev = req.GetEnvp();
// 	char **envp = new char*[ev.size() + 1];

// 	for (size_t i = 0; i < ev.size(); ++i) {
// 		envp[i] = new char[ev[i].size() + 1];  // Aloca memória para a string e o caractere nulo
// 		std::strcpy(envp[i], ev[i].c_str());   // Copia a string para o array de char
// 	}

// 	// Adicionar o ponteiro nulo para indicar o final do array
	
// 	envp[ev.size()] = NULL;
// 	for (size_t i = 0; envp[i] != NULL; ++i) {
// 		std::cout << envp[i] << std::endl;
// 	}
// 	const char *phpInterpreter = "/usr/bin/php";
// 	// entra nessa pasta "/nfs/homes/dapaulin/Documents/web/web-server/home/ranna-site/"
// 	const char *scriptPath = "/nfs/homes/dapaulin/Documents/web/web-server/home/ranna-site/index.php";
// 	const char *argv[] = {phpInterpreter, scriptPath, NULL};
// 	int sv[2];  // Array para armazenar os descritores dos sockets

//     if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
//         std::cerr << "Erro ao criar socket pair: " << strerror(errno) << std::endl;
//         return;
//     }

// 	pid_t pid = fork();
// 	if (pid == 0) {

//         close(sv[0]);

//         dup2(sv[1], STDOUT_FILENO);
//         dup2(sv[1], STDERR_FILENO);
//         close(sv[1]);

// 		execve("/usr/bin/php", (char**)argv, envp);
// 	} else {
// 		int status;
		
// 		pid_t result = waitpid(pid, &status, 0);
// 		close(sv[1]);  // Fecha o socket não utilizado no processo pai
// 		struct epoll_event  event;
// 		memset(&event, 0, sizeof(struct epoll_event));
// 		if (epoll_ctl(epollFD, EPOLL_CTL_ADD, sv[0], &event) == -1)
//             throw std::invalid_argument("");
// 		return;

// 	}

// 	char buf[1024];
// 	memset(buf, '\0', 1024);
// 	ssize_t n = read(sv[0], buf, sizeof(buf) - 1);
// 	if (n == -1) {
// 		std::cerr << "Erro ao ler do socket: " << strerror(errno) << std::endl;
// 	} else {
// 		buf[n] = '\0';  // Adiciona o terminador nulo
// 		std::cout << "Saída do CGI:\n" << buf << std::endl;
// 	}
// 	close(sv[0]);

// 	// std::cout << "Este é o processo pai, PID = " << getpid() << ", Filho PID = " << pid << std::endl;
//   // Espera pelo processo filho específico
// 	if (result == -1) {
// 		std::cerr << "Erro no waitpid" << std::endl;
// 		return;
// 	}

// 	if (WIFEXITED(status)) {
// 		std::cout << "O processo filho terminou normalmente com status " << WEXITSTATUS(status) << std::endl;
// 	} else if (WIFSIGNALED(status)) {
// 		std::cout << "O processo filho terminou devido a um sinal com número " << WTERMSIG(status) << std::endl;
// 	}
}