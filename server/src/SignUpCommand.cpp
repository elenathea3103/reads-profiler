#include "SignUpCommand.h"
#include "Database.h"
#include <sstream>

using namespace std;

SignupCommand::SignupCommand(const std::string& input) {
    stringstream ss(input);
    string cmd;
    ss >> cmd >> username >> password;
}

std::string SignupCommand::execute() {
    if (username.empty() || password.empty()) {
        return "Usage: signup <username> <password>";
    }

    Database db;
    if (!db.open("books.db")) {
        return "Server Error: Could not open database.";
    }

    bool success = db.registerUser(username, password);
    db.close();

    if (success) {
        return "Account created successfully! You can now login.";
    } else {
        return "Error: Username '" + username + "' is already taken.";
    }
}