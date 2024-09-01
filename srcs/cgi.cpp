# include "Fastcgi.hpp"
# include "HttpRequest.hpp"

int main() {
	Fastcgi cgi;
	HttpRequest n;

	cgi.newProcessCGI(n);
}