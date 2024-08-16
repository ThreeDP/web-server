#ifndef __BUILDER_SERVER_HPP__
# define __BUILDER_SERVER_HPP__

class BuilderServer {
    IHandler        *_handler;
    IServer         *_server;

    BuilderServer(IHandler handler) {
        _handler = handler;
        _server = NULL;
    }
    ~BuilderServer(void) {}

    virtual IBuilderServer      &SetupServer(void) {
        if (_server != NULL) {
            _server = NULL;
        }
        _server = Server();
    }
    virtual IBuilderServer      &WithRoute(IRoute *route) {
        _server->SetRoute(route->GetRouteName(), route);
    }

    virtual IBuilderServer      &WithHosts(std::vector<std::string> hosts) {

    }ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQCs8ZiwF+fyUoYB1I6HWNSdvYEi4Cz0UiEhe9MUb6Fx56yBGNBujMp81p2NylAMhCvAYGSJI2xUvP4hrjk8ZpLaYU80ywLV/k3m1dxPjR1tfY3bfnC8A7JBDPjEH1ATTBk+0ZYUSS6j0Eo7tVIbaVHJg3Ag7GnKKIar5nkPZTWZQb3ebPpE5G80JK0M+VFNsFiZVOJh/Zf9tKRy83NlnLJYGeMkuiYV5vda47XWeWkVbHksaOM2tvquBUbjEBnPm9wsKX0jDipMsv3TfYVaSpKJ6oOcAKycuA1mwX8IKIn0HvU/okkDKwLFDyFQlkHuAUMv5O51tc+2ssX34HRmWSD6ZYO2GiVl7RAejBa5pesIwtn4yzdUDcvLzAsINEKMVPdKZamVaxCsIwJSRj+saSVwG0yAzFaBaIBkX32Fj4SLp58P44L68pOX0K3LfRDBRtj7ziCtEbjb8gC5TlFLj6A1FU54lLKKEPN6uADJ533eSIKafJVqiuowUab4UtbwjNU= codespace@codespaces-ee6fb8

    virtual IBuilderServer      &WithPort(std::string port) = 0;

    virtual IBuilderServer      &WithAllowMethods(std::set<std::string> methods) = 0;
    virtual IBuilderServer      &WithErrorPages(std::set<HttpStatusCode::Code> statusCode, std::string filePath) = 0;
    virtual IBuilderServer      &WithbodyLimit(int size) = 0;
    virtual IBuilderServer      &WithRedirectPath(std::pair<std::string, std::string> pair) = 0;
    virtual IBuilderServer      &WithRootDirectory(std::string root) = 0;
    virtual IBuilderServer      &WithAutoIndex(bool flag) = 0;

    virtual IServer             *GetResult(void) = 0;

}

#endif 