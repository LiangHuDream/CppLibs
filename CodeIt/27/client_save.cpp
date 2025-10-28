#include <exception>
#include <iostream>
#ifdef _WIN32
#include <fcntl.h>                // _O_WTEXT
#include <io.h>                   // _setmode
#include <stdio.h>                // _fileno/stdout
#endif
#include <stddef.h>               // size_t
#include <cpprest/filestream.h>   // concurrency::streams
#include <cpprest/http_client.h>  // web::http/...

using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;
using std::cerr;
using std::endl;

#ifdef _WIN32
#define tcout std::wcout
#else
#define tcout std::cout
#endif

auto get_headers(http_response resp)
{
    auto headers = resp.to_string();
    auto end = headers.find(U("\r\n\r\n"));
    if (end != string_t::npos) {
        headers.resize(end + 4);
    }

    return headers;
}

auto get_request(string_t uri)
{
    http_client client{uri};
    // 用 GET 方式发起一个客户端请求
    auto request =
        client.request(methods::GET).then([](http_response resp) {
            if (resp.status_code() == status_codes::OK) {
                // 正常的话
                tcout << U("Saving...\n");
                ostream fs;
                fstream::open_ostream(U("results.html"),
                                      std::ios_base::out |
                                          std::ios_base::trunc)
                    .then([&fs, resp](ostream os) {
                        fs = os;
                        // 读取网页内容到流
                        return resp.body().read_to_end(fs.streambuf());
                    })
                    .then([&fs](size_t size) {
                        // 然后关闭流
                        fs.close();
                        tcout << size << U(" bytes saved\n");
                    })
                    .wait();
            } else {
                // 否则显示当前响应信息
                auto headers = get_headers(resp);
                tcout << headers;
                tcout << resp.extract_string().get();
            }
        });
    return request;
}