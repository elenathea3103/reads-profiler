#include "TcpServer.h"
#include "CommandFactory.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

struct ThreadArgs
{
    int client_fd;
};

std::set<std::string> TcpServer::activeUsers;
std::mutex TcpServer::activeUsersMtx;

bool TcpServer::loginUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(activeUsersMtx); 
    
    if (activeUsers.count(username)) {
        return false; 
    }

    activeUsers.insert(username);
    return true;
}

void TcpServer::logoutUser(const std::string& username) {
    if (username.empty()) return;
    
    std::lock_guard<std::mutex> lock(activeUsersMtx);
    activeUsers.erase(username);
    cout << "[Session] User logged out: " << username << "\n";
}


TcpServer::TcpServer(int port) : server_fd(-1), port(port) {}

TcpServer::~TcpServer() {
    if (server_fd != -1) close(server_fd);
}

bool TcpServer::start()
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return false;
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (sockaddr *)&serv, sizeof(serv)) < 0) return false;
    if (listen(server_fd, 10) < 0) return false;
    cout << "Server running on port " << port << "...\n";
    return true;
}

void TcpServer::run()
{
    while (true)
    {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;
        ThreadArgs *args = new ThreadArgs{client_fd};
        pthread_t tid;
        pthread_create(&tid, nullptr, handleClient, args);
        pthread_detach(tid);
    }
}

void *TcpServer::handleClient(void *arg)
{
    ThreadArgs *t = (ThreadArgs *)arg;
    int fd = t->client_fd;
    delete t;

    string currentUser = "";

    cout << "[Thread] New client connected.\n";

    while (true)
    {
        int msg_len;
        int r = recv(fd, &msg_len, sizeof(msg_len), 0);
        
        if (r <= 0) break; 

        string msg(msg_len, '\0');
        recv(fd, msg.data(), msg_len, 0);

        if (msg == "quit") break; 

        Command *cmd = CommandFactory::create(msg, fd, &currentUser);
        string response = cmd ? cmd->execute() : "Error";
        
        int type = cmd ? cmd->getType() : 0;
        if (type == 1 && response.rfind("ERROR", 0) == 0) type = 0;

        int len = response.size();
        send(fd, &type, sizeof(type), 0);
        send(fd, &len, sizeof(len), 0);
        send(fd, response.c_str(), len, 0);

        delete cmd;
    }

    if (!currentUser.empty()) {
        TcpServer::logoutUser(currentUser);
    }

    close(fd);
    pthread_exit(nullptr);
}