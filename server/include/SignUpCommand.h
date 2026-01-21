#pragma once
#include "Command.h"
#include <string>

class SignupCommand : public Command {
private:
    std::string username;
    std::string password;

public:
    SignupCommand(const std::string& input);
    std::string execute() override;
};