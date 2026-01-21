#include "Client.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    
    string ipAddress = "127.0.0.1"; 
    int port = 8080;

    // ex: ./client 192.168.1.50
    if (argc >= 2) {
        ipAddress = argv[1];
    }

    cout << "Connecting to server at " << ipAddress << ":" << port << "...\n";

    Client c(ipAddress, port);

    if (!c.connectToServer()) {
        cout << "Failed to connect to server. Check IP and Firewall.\n";
        return 1;
    }

    c.run();
    return 0;
}