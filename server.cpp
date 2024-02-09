#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "server.hpp"
#include "request.hpp"

void Server::stop() {
    this->status = STOPPING;
    
    return;
}

int Server::start() {
    struct sockaddr_in addr;

    this->sock = socket(AF_INET,SOCK_STREAM,0);

    memset(&addr,0,sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0

    if (bind(this->sock,(struct sockaddr *)&addr,sizeof(addr))<0) {
        printf("bind call failed!\n");
        return 1;
    }

    if (listen(this->sock,200)<0) {   
        printf("call to listen failed!\n");
        return 1;
    }

    this->status = STARTED;

    while (this->status==STARTED) {

        struct conn_data* data = new conn_data;

        int conn = accept(sock,(struct sockaddr*)&data->addr,&data->addr_len);
        if (conn<0) {
            printf("accept call failed!\n");
            delete data;
            continue;
        }
        data->conn = conn;
        data->server = this;

        pthread_t thread;

        pthread_create(&thread,NULL,&handle_request,data);
        pthread_detach(thread);

    }

    close(this->sock);

    printf("No longer accepting connections!\nWaiting for existing connections to terminate...\n");

    printf("All connections terminated!\n");

    this->status = STOPPED;

    return 0;
}

void Server::setRoot(const char* dir) {
    struct stat st;
    if (stat(dir,&st)<0 || (st.st_mode & S_IFDIR) != 0) {
        printf("Stat failed, root dir invalid! (%s)",dir);
        return;
    }

    int len = strlen(dir);
    this->rootDir = new char[len+1]{0};
    memcpy(this->rootDir,dir,len);
}

