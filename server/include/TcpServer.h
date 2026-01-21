#pragma once
#include <string>
#include <set>      
#include <mutex>    

class TcpServer {
private:
    int server_fd;
    int port;

    static void* handleClient(void* arg);

public:
    TcpServer(int port);
    ~TcpServer();

    bool start();
    void run();

    static std::set<std::string> activeUsers;
    static std::mutex activeUsersMtx;

    static bool loginUser(const std::string& username);
    static void logoutUser(const std::string& username);
};