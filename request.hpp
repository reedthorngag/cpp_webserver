#include "server.hpp"

#ifndef _Request
#define _Request

struct header {
    char* name;
    char* value;
};

class Request {
    private:
        int conn;

    public:
        int status = -1;
        std::vector<header> headers;
        char* method;
        char* body = nullptr;
        char* path;

        Request(int conn);

    ~Request();
};

void* handle_request(void* data);

#endif

void* new_request(void* data);
