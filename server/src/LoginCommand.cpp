#include "LoginCommand.h"
#include "Database.h"
#include "TcpServer.h" 
#include <sstream>

using namespace std;

LoginCommand::LoginCommand(const std::string& input, std::string* currentUser) 
    : serverUserLink(currentUser)
{
    stringstream ss(input);
    string cmd;
    ss >> cmd >> username >> password;
}

std::string LoginCommand::execute()
{
    if (!serverUserLink->empty()) {
        return "Error: You are already logged in as " + *serverUserLink + ". Please restart client or logout.";
    }

    Database db;
    db.open("books.db");
    
    bool credentialsOK = db.checkLogin(username, password);
    db.close();

    if (!credentialsOK) {
        return "Login failed! Check username/password.";
    }

    if (!TcpServer::loginUser(username)) {
        return "Login failed! User '" + username + "' is already connected on another device.";
    }

    *serverUserLink = username; 
    return "Login successful! Welcome " + username;
}