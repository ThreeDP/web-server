#include <gtest/gtest.h>
#include <iostream>
#include "Parser.hpp"

// Path for the config files. 
const std::string PWD = "../../../config/";

TEST(ParserAllowMethods, BasicAssertion) {
  std::string file = PWD + std::string("nginx.conf");
  try {
    Parser obj(file.c_str());
    std::vector<Server>  servers = obj.ParserServer();

    std::string route = "<path>";
    std::vector<std::string> expected = {"GET", "POST", "HEAD"};

    std::vector<std::string> &actual = (servers.back().routes[route])->GetAllowMethods();
    for(std::vector<std::string>::size_type i = 0; i < actual.size(); ++i){
        EXPECT_STREQ(actual[i].c_str(), expected[i].c_str());
    }
  } catch (const std::exception &e) {

  }
}

TEST(ParserReWriteRoute, BasicAssertion) {
  std::string file = PWD + std::string("nginx.conf");
  try {
    Parser obj(file.c_str());
    std::vector<Server>  servers = obj.ParserServer();

    std::vector<std::string> routes = {"/blog"};
    std::vector<std::string> expected = {"/"};

    for (std::vector<std::string>::size_type i = 0; i < routes.size(); ++i) {
      std::map<std::string, std::string> &actual = (servers.back().routes[routes[i]])->GetRedirectPath();
      EXPECT_STREQ(actual[routes[i]].c_str(), expected[i].c_str());
    }
  } catch (const std::exception &e) {

  }
}
