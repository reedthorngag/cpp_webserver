#include <vector>

#include "request.hpp"

#ifndef _Response
#define _Response

class Response {
    private:
        int conn;
        int _status;
        Request* request;
        std::vector<header> headers;

    public:
        Response* status(int status);
        Response* send(char* data, int len);
        Response* setHeader(char* name, char* value);
        Response* setHeader(header);
        Response* contentType(char* type);
        Response* sendFile(char* file);

        Response(int conn, Request* request);
        ~Response();
};

#endif
