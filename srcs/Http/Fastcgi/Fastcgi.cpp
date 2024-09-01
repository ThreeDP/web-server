# include "Fastcgi.hpp"
#include <sys/wait.h>

int Fastcgi::newProcessCGI(HttpRequest &req, int epollFD) {
	std::vector<std::string> ev = req.GetEnvp();
	char **envp = new char*[ev.size() + 1];

	for (size_t i = 0; i < ev.size(); ++i) {
		envp[i] = new char[ev[i].size() + 1];  
		std::strcpy(envp[i], ev[i].c_str());
	}

	envp[ev.size()] = NULL;

	const char *phpInterpreter = "/usr/bin/php";
	const char *scriptPath = "/nfs/homes/rleslie-/web-server/home/ranna-site/index.php";
	const char *argv[] = {phpInterpreter, scriptPath, NULL};
	int sv[2]; 

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        std::cerr << "Erro ao criar socket pair: " << strerror(errno) << std::endl;
        return -1;
    }
	pid_t pid = fork();
	if (pid == 0) {
        close(sv[0]);

        dup2(sv[1], STDOUT_FILENO);
        dup2(sv[1], STDERR_FILENO);

        close(sv[1]);

		execve("/usr/bin/php", (char**)argv, envp);
	} else {
		int status;
		waitpid(pid, &status, 0);
		close(sv[1]);  // Fecha o socket não utilizado no processo pai
		struct epoll_event  event;

		memset(&event, 0, sizeof(struct epoll_event));
        event.events = EPOLLIN;
        event.data.fd = sv[0];
		if (epoll_ctl(epollFD, EPOLL_CTL_ADD, sv[0], &event) == -1)
            throw std::invalid_argument("");
		return sv[0];

	}

	// char buf[1024];
	// memset(buf, '\0', 1024);
	// ssize_t n = read(sv[0], buf, sizeof(buf) - 1);
	// if (n == -1) {
	// 	std::cerr << "Erro ao ler do socket: " << strerror(errno) << std::endl;
	// } else {
	// 	buf[n] = '\0';  // Adiciona o terminador nulo
	// 	std::cout << "Saída do CGI:\n" << buf << std::endl;
	// }
	// close(sv[0]);

	// std::cout << "Este é o processo pai, PID = " << getpid() << ", Filho PID = " << pid << std::endl;
//   Espera pelo processo filho específico
	// if (result == -1) {
	// 	std::cerr << "Erro no waitpid" << std::endl;
	// 	return;
	// }

	// if (WIFEXITED(status)) {
	// 	std::cout << "O processo filho terminou normalmente com status " << WEXITSTATUS(status) << std::endl;
	// } else if (WIFSIGNALED(status)) {
	// 	std::cout << "O processo filho terminou devido a um sinal com número " << WTERMSIG(status) << std::endl;
	// }
}