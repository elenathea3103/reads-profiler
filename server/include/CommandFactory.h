#pragma once
#include "Command.h"
#include <string>

class CommandFactory {
public:
    static Command* create(const std::string& input, int client_fd, std::string* currentUser);
};