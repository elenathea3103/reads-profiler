#pragma once
#include <string>

class Client {
public:
    Client(const std::string &ip, int port);
    bool connectToServer();
    void run();

private:
    std::string serverIp;
    int serverPort;
    int sockfd;

    void sendMessage(const std::string &msg);
    std::string receiveMessage();
    void downloadFile(const std::string& isbn); 
};
