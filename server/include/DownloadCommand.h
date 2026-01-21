#pragma once
#include "Command.h"
#include <string>

class DownloadCommand : public Command {
public:
    DownloadCommand(const std::string& input, const std::string& user);
    std::string execute() override;
    int getType() override { return 1; }

private:
    std::string isbn;
    std::string username; 
};