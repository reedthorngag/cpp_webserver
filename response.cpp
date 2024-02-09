#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "response.hpp"
#include "util.hpp"


Response* Response::status(int status) {
    this->_status = status;

    return this;
}

Response* Response::setHeader(header header) {
    this->headers.push_back(header);

    return this;
}

Response* Response::setHeader(char* name, char* value) {
    this->headers.push_back(header{name:name,value:value});

    return this;
}

Response* Response::send(char* data, int len) {
    write(this->conn,data,len);

    return this;
}

Response* Response::contentType(char* type) {
    bool found = false;
    for (header header : this->headers) {
        if (str_equals(header.name,"Content-Type\x00")) {
            header.value = type;
            found = true;
            break;
        }
    }

    if (!found) {
        char* name = new char[13]{'C','o','n','t','e','n','t','-','T','y','p','e',0};
        this->headers.push_back(header{name:name,value:type});
    }

    return this;
}

Response* Response::sendFile(char* file) {
    if (FILE* fp = fopen(file,"r")) {
        const char* string = "HTTP/2 200\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
        write(this->conn,string,strlen(string));

        char buf[1024];
        int in;
        while ((in=fread(buf,1,1024,fp))>0) {
            write(this->conn,buf,in);
        }
        fclose(fp);
    } else {
        const char* string = "HTTP/2 404\r\nContent-Type: text/html; charset=utf-8\r\n\r\nFile not found!";
        write(this->conn,string,strlen(string));
    }
    return this;
}

Response::Response(int conn, Request* request) {
    this->conn = conn;
    this->request = request;
}

Response::~Response() {
    for (header header : this->headers) {
        delete header.name;
        delete header.value;
    }

    this->headers.clear();
}

