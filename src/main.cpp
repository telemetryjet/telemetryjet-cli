#include "server.h"
#include <iostream>

Server server;

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    server.triggerStop();
    exit(signum);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    server.start();

    return 0;
}