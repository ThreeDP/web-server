#include <gtest/gtest.h>
#include <iostream>
#include "Parser.hpp"

// Path for the config files. 
const std::string PWD = "../../../config/";

// TEST(ParserAllowMethods, BasicAssertion) {
//   Http http;
//   std::string file = PWD + std::string("nginx.conf");
//   std::string               server_name = "www.example.com";
//   std::string               route = "<path>";
//   std::vector<std::string>  expected = {"GET", "POST", "HEAD"};

//   try {
//     Parser obj(file.c_str());
//     obj.ParserServer(http);
//   } catch (const std::exception &e) {}

//   std::set<std::string> &actual = (http.GetServer(server_name)->routes[route])->GetAllowMethods();
//   for(std::vector<std::string>::size_type i = 0; i < actual.size(); ++i){
//       EXPECT_STREQ(actual[i].c_str(), expected[i].c_str());
//   }
// }

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

// TEST(ParserServerName, BasicAssertion) {
//   Http http;
//   std::string file = PWD + std::string("nginx.conf");
//   std::vector<std::string>  expected = {"example.com", "www.example.com"};

//   try {
//     Parser obj(file.c_str());
//     obj.ParserServer(http);
//   } catch (const std::exception &e) {}

//   EXPECT_NE(http.GetServer(expected[0]), nullptr);
//   EXPECT_NE(http.GetServer(expected[1]), nullptr);
//   EXPECT_EQ(http.GetServer(expected[0]), http.GetServer(expected[1]));
//   std::vector<std::string> server_names = http.GetServer(expected[0])->server_names;
//   for (std::vector<std::string>::size_type i = 0; i < server_names.size(); i++) {
//     EXPECT_STREQ(server_names[i].c_str(), expected[i].c_str());
//   }
// }
