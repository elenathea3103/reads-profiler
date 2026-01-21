#include "CommandFactory.h"
#include "SearchCommand.h"
#include "DownloadCommand.h"
#include "RecommendCommand.h"
#include "LoginCommand.h"
#include "SignUpCommand.h"

using namespace std;

Command *CommandFactory::create(const std::string &input, int client_fd, std::string* currentUser)
{
    string actualUser = (currentUser) ? *currentUser : "";

    if (input.rfind("login ", 0) == 0)
        return new LoginCommand(input, currentUser);

    if (input.rfind("signup ", 0) == 0)
        return new SignupCommand(input);

    if (input.rfind("search ", 0) == 0)
        return new SearchCommand(client_fd, input, actualUser); 

    if (input.rfind("download ", 0) == 0)
        return new DownloadCommand(input, actualUser); 

    if (input == "recommend")
        return new RecommendCommand(actualUser); 

    class UnknownCommand : public Command {
    public:
        string execute() override { return "Unknown command!"; }
    };
    return new UnknownCommand();
}