#pragma once
#include "Command.h"
#include <string>

class SearchCommand : public Command {
private:
    int client_fd;
    std::string field;
    std::string value;
    std::string username;

public:
    SearchCommand(int client_fd, const std::string& input, const std::string& user); 
    std::string execute() override;
};