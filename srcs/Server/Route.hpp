#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"
# include "Utils.hpp"
# include "CommonParameters.hpp"
# include "AHttpResponse.hpp"
# include "HttpRequest.hpp"

# include "ResponseOK.hpp"
//# include "HttpResponse.hpp"


enum RouteStages {
    R_START,
    R_REQUEST
};

class Route {
    private:
        std::string                                 _route_name;
        std::set<std::string>                       *_allow_methods;
        std::map<int, std::string>                  _error_page;
        int                                         _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _directory;
        bool                                        _autoIndex;
        std::set<std::string>                       &_index;
        
        AHttpResponse               *_fondStatusResponse(int statusCode, std::string extension);

    public:
        RouteStages                                 _stage;

        // Route Methods

        std::set<std::string>       *CatDirectorysFiles(std::string path, std::vector<struct dirent *> &dirs);
        AHttpResponse               *ProcessRoute(HttpRequest &httpReq);
        std::string                 ReturnFileRequest(std::string path);
        mode_t                      CatFileMode(std::string &path, int &statusCode);
        bool                        FindFilePattern(std::string &path, std::set<std::string> *dirs);
        AHttpResponse               *DetermineOutputFile(HttpRequest &httpReq);
        std::string                 GenerateAutoindex(std::vector<struct dirent *> dirs, std::string path);
        AHttpResponse               *checkFilePermission(HttpRequest &httpReq, int &statusCode);
        
        // Geters

        std::string                 GetRedirectPath(void);
        std::set<std::string>       *GetAllowMethods(void);
        std::map<int, std::string>  GetErrorPage(void);
        void                        pathReset(std::string &path);
        std::string                 GetRouteName(void) const;

        // Seters

        void                        SetAllowMethods(std::set<std::string> *methods);
        void                        SetRedirectPath(std::string redirect);
        void                        SetErrorPageRoute(std::pair<int, std::string> error_page);

        // Base methods

        Route(CommonParameters *server, std::string server_name);

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
};

std::ostream &operator<<(std::ostream &os, Route const &route);

#endif
