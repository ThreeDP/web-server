#include <gtest/gtest.h>
#include <iostream>
#include "Parser.hpp"

// Path for the config files. 
const std::string PWD = "../../../config/";

TEST(ParserAllowMethods, BasicAssertion) {
  Http http;
  std::string file = PWD + std::string("nginx.conf");
  std::string               server_name = "www.example.com";
  std::string               route = "<path>";
  std::vector<std::string>  expected = {"GET", "POST", "HEAD"};

  try {
    Parser obj(file.c_str());
    obj.ParserServer(http);
  } catch (const std::exception &e) {}

  std::vector<std::string> &actual = (http.GetServer(server_name)->routes[route])->GetAllowMethods();
  for(std::vector<std::string>::size_type i = 0; i < actual.size(); ++i){
      EXPECT_STREQ(actual[i].c_str(), expected[i].c_str());
  }
}

// TEST(ParserReWriteRoute, BasicAssertion) {
//   Http http;
//   std::string file = PWD + std::string("nginx.conf");
//   std::string               server_name = "example.com";
//   std::vector<std::string>  routes = {"/blog"};
//   std::vector<std::string>  expected = {"/"};

//   try {
//     Parser obj(file.c_str());
//     obj.ParserServer(http);
//   } catch (const std::exception &e) {}

//   for (std::vector<std::string>::size_type i = 0; i < routes.size(); ++i) {
//     std::map<std::string, std::string> &actual = (http.GetServer(server_name)->routes[routes[i]])->GetRedirectPath();
//     EXPECT_STREQ(actual[routes[i]].c_str(), expected[i].c_str());
//   }
// }

TEST(ParserServerName, BasicAssertion) {
  Http http;
  std::string file = PWD + std::string("nginx.conf");
  std::vector<std::string>  expected = {"example.com", "www.example.com"};

  try {
    Parser obj(file.c_str());
    obj.ParserServer(http);
  } catch (const std::exception &e) {}

  EXPECT_NE(http.GetServer(expected[0]), nullptr);
  EXPECT_NE(http.GetServer(expected[1]), nullptr);
  EXPECT_EQ(http.GetServer(expected[0]), http.GetServer(expected[1]));
  std::vector<std::string> server_names = http.GetServer(expected[0])->server_names;
  for (std::vector<std::string>::size_type i = 0; i < server_names.size(); i++) {
    EXPECT_STREQ(server_names[i].c_str(), expected[i].c_str());
  }
}

TEST(ParserServerErrorPage, Tests_Whether_Error_Pages_Are_Being_Set_On_The_Server) {
  Http http;
  std::string file = PWD + std::string("nginx.conf");
  std::string                 server_name = "example.com";
  std::map<int, std::string>  expected = {
    {404, "/404.html"}, 
    {500, "/50x.html"},
    {502, "/50x.html"},
    {503, "/50x.html"},
    {504, "/50x.html"}
  };

  try {
    Parser obj(file.c_str());
    obj.ParserServer(http);
  } catch (const std::exception &e) {}

  Server *server = http.GetServer(server_name);
  for (std::map<int, std::string>::iterator it = expected.begin(); it != expected.end(); ++it) {
    EXPECT_STREQ(it->second.c_str(), server->default_error_page[it->first].c_str());
  }
}

TEST(ParserLocationErrorPage, Tests_Whether_The_Route_Is_Inheriting_The_Servers_Error_Pages) {
  Http http;
  std::string file = PWD + std::string("nginx.conf");
  std::string                 server_name = "example.com";
  std::map<int, std::string>  expected = {
    {404, "/404.html"}, 
    {500, "/50x.html"},
    {502, "/50x.html"},
    {503, "/50x.html"},
    {504, "/50x.html"}
  };

  try {
    Parser obj(file.c_str());
    obj.ParserServer(http);
  } catch (const std::exception &e) {}

  Route *route = http.GetServer(server_name)->routes["/"];
  for (std::map<int, std::string>::iterator it = expected.begin(); it != expected.end(); ++it) {
    EXPECT_STREQ(it->second.c_str(), route->_error_page[it->first].c_str());
  }
}

TEST(ParserLocationErrorPage, Tests_Whether_Error_Pages_Are_Being_Set_On_The_Location) {
  Http http;
  std::string file = PWD + std::string("nginx.conf");
  std::string                 server_name = "mysite.com";
  std::map<int, std::string>  expected = {
    {404, "/static/404.html"}, 
    {500, "/static/50x.html"},
    {502, "/static/50x.html"},
    {503, "/static/50x.html"},
    {504, "/static/50x.html"}
  };

  try {
    Parser obj(file.c_str());
    obj.ParserServer(http);
  } catch (const std::exception &e) {}

  Route *route = http.GetServer(server_name)->routes["/"];
  for (std::map<int, std::string>::iterator it = expected.begin(); it != expected.end(); ++it) {
    EXPECT_STREQ(it->second.c_str(), route->_error_page[it->first].c_str());
  }
}

// TEST ERRORS EXCEPTIONS

TEST(ParserExceptionOpenBraces, BasicAssertion) {
  Http http;
  std::string file = PWD + std::string("errors/server_syntax_error.conf");
  std::string   expected_exception = "Error: Server expected '{'.";
  std::string   actual_exception = "";

  try {
    Parser obj(file.c_str());
    obj.ParserServer(http);
  } catch (const std::exception &e) {
    actual_exception = e.what();
  }

  EXPECT_STREQ(expected_exception.c_str(), actual_exception.c_str());
}
