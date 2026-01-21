#pragma once
#include "Command.h"
#include <string>

class LoginCommand : public Command {
private:
    std::string username;
    std::string password;
    std::string* serverUserLink; 

public:
    LoginCommand(const std::string& input, std::string* currentUser);
    std::string execute() override;
};