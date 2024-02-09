#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "request.hpp"
#include "util.hpp"

void* handle_request(void* data) {

    struct conn_data* conn_data = reinterpret_cast<struct conn_data*>(data);

    Request* request = new Request(conn_data->conn);
    if (request->status!=0) {
        const char* string = "HTTP/2 400\r\nContent-Type: text/html; charset=utf-8\r\n\r\nMalformed request!";
        write(conn_data->conn,string,strlen(string));
    } else {
        if (conn_data->server->rootDir!=nullptr && validate(request->path)) {
            char* file = join(conn_data->server->rootDir,request->path);
            printf("returning file: %s\n",file);
            sendFile(conn_data->conn,file);
            delete file;
        } else {
            const char* string = "HTTP/2 404\r\nContent-Type: text/html; charset=utf-8\r\n\r\nFile not found!";
            write(conn_data->conn,string,strlen(string));
        }
    }

    delete request;

    close(conn_data->conn);

    delete conn_data;
    return nullptr;
}

Request::Request(int conn) {
    this->conn = conn;

    char buf[1024];

    long in = read(conn,buf,1024);
    printf("in is: %d\n",(int)in);

    char* ptr = buf;

    char* method = read_in(&ptr,' ',32);
    if (method==nullptr) {
        this->status=400;
        return;
    }

    this->method = method;

    //printf("request method: %s\n",method);

    char* path = read_in(&ptr,' ',2048);
    if (path==nullptr) {
        this->status=400;
        return;
    }
    printf("requested path: %s\n",path);
    this->path = path;

    char* protocol = read_in(&ptr,'\n',128);
    delete protocol;

    while (true) {
        if (*ptr=='\r' && *++ptr=='\n') break;

        char* header_name = read_in(&ptr,':',512);
        if (header_name==nullptr) {
            this->status=400;
            return;
        }
        ptr++; // skip the space character (TODO: check the http spec guarrentees this space exists)

        char* header_value = read_in(&ptr,'\r',4096);
        if (header_value==nullptr) {
            this->status=413;
            return;
        }
        ptr++; // skip over the \n because http uses \r\n endlines

        header header{name:header_name,value:header_value};

        printf("header name: %s\n",header_name);
        printf("header value: %s\n",header_value);

        this->headers.push_back(header);
    }

    in = in - ((ptr-buf)+1);

    if (in<=0) {
        this->status = 0;
        return;
    }

    char* body[18];
    char** bodyPtr = body;

    long total_read = in;

    char* new_buf = new char[in];
    memcpy(new_buf,ptr,in);
    *(bodyPtr++) = new_buf;
    *(bodyPtr++) = (char*)in;

    in = read(conn,buf,1024);

    while (in>0) {
        char* new_buf = new char[in];
        memcpy(new_buf,buf,in);
        *(bodyPtr++) = new_buf;
        *(bodyPtr++) = (char*)in;

        total_read += in;
        if (total_read>8196) {
            this->status = 413;
            return;
        }

        in = read(conn,buf,1024);
    }

    this->body = new char[total_read];

    long pos=0;
    for (int i=0;i<18;i+=2) {
        memcpy(this->body+pos,body[i],(long)body[i+1]);
        pos += (long)body[i+1];
        delete body[i];
    }
    
    this->status = 0;
}

Request::~Request() {
    delete this->method;
    for (header header : this->headers) {
        delete header.name;
        delete header.value;
    }
}

