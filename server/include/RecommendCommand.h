#pragma once
#include "Command.h"
#include <string>

class RecommendCommand : public Command {
private:
    std::string username; 
public:
    RecommendCommand(const std::string& user);
    std::string execute() override;
};