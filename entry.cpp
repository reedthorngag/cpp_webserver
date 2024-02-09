#include <stdio.h>
#include <signal.h>
#include <csignal>
#include <string.h>

#include "server.hpp"


Server server;

void handle_sigint([[maybe_unused]] int s) {
    printf("Recived SIGINT, killing...\n");
    server.stop();
}

int main() {

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = handle_sigint;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    
    const char* dir = "/home/reedt/code/cpp_webserver/webserver";
    char* rootDir = new char[strlen(dir)+1];
    memcpy(rootDir,dir,strlen(dir));
    server.rootDir = rootDir;
    printf("root dir: %s\n",server.rootDir);

    return server.start();
}