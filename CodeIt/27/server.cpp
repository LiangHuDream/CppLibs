#include <exception>                // std::exception
#include <iostream>                 // std::cout/cerr/endl/wcout
#include <map>                      // std::map
#include <string>                   // std::string
#ifdef _WIN32
#include <fcntl.h>                  // _O_WTEXT
#include <io.h>                     // _setmode
#include <stdio.h>                  // _fileno/stdout
#endif
#include <cpprest/http_listener.h>  // web::http::experimental::listener/...
#include <cpprest/json.h>           // web::json

using namespace std;
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::
  experimental::listener;

#ifdef _WIN32
#define tcout std::wcout
#else
#define tcout std::cout
#endif

void handle_get(http_request request)
{
    auto& uri = req.request_uri();

    if (uri.path() != U("/sayHi")) {
        req.reply(status_codes::NotFound);
        return;
    }

    tcout << uri::decode(uri.query()) << endl;

    auto query = uri::split_query(uri.query());
    auto it = query.find(U("name"));
    if (it == query.end()) {
        req.reply(status_codes::BadRequest, U("Missing query info"));
        return;
    }

    auto answer = json::value::object(true);
    answer[U("msg")] =
        json::value(string_t(U("Hi, ")) + uri::decode(it->second) + U("!"));

    req.reply(status_codes::OK, answer);
}