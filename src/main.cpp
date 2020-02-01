#include "server.h"
#include <iostream>

Server server;

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    server.stop();
    exit(signum);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    server.start();

    while (server.shouldUpdate()) {
        server.update();
    }

    return 0;
}