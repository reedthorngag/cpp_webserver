#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <atomic>

#ifndef _Server
#define _Server

enum server_status {
    STARTING,
    STARTED,
    STOPPING,
    STOPPED
};

class Server {
    private:
        std::atomic<server_status> status = STARTING;
        int sock;
    
    public:
        char* rootDir;
        int start();
        void stop();
        void setRoot(const char* dir);
};

struct conn_data {
    int conn;
    struct sockaddr_in addr;
    socklen_t addr_len;
    Server* server;
};

#endif

