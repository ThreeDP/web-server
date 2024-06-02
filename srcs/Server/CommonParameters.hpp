#ifndef __COMMON_PARAMETERS_HPP__
# define __COMMON_PARAMETERS_HPP__

# include <iostream>
# include <set>
# include <map>
# include <vector>

class CommonParameters {
	protected:
		std::string                             _listen_host;
        unsigned short                          _listen_port;
        std::vector<std::string>                _server_names;
        std::set<std::string>                   _default_allow_methods;
        std::map<int, std::string>              _default_error_page;
        int                                     _limit_client_body_size;
        std::map<std::string, std::string>      _rewrites;
        std::string                             _root;
        std::set<std::string>                   _index;
        bool                                    _autoindex;
		
	public:

		// Seters
		void		SetServerName(std::string server_name);
		void		SetRewrites(std::pair<std::string, std::string> rewrite);
		void		SetErrorPage(std::pair<std::string, std::string> rewrite);

		// Geters
		std::set<std::string>				*GetDefaultAllowMethods(void);
		std::map<int, std::string>			&GetDefaultErrorPage(void);
		int 								GetLimitClientBodySize(void);
		std::map<std::string, std::string>	&GetReWrites(void);
		std::string 						GetRoot(void);
		std::set<std::string> 				&GetIndex(void);
		bool 								GetAutoIndex(void);
		std::vector<std::string>			GetServerNames(void);
		// Base Methods
		CommonParameters(void);
		CommonParameters(std::vector<std::string> serv, unsigned short port);
};

#endif