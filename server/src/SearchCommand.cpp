#include "SearchCommand.h"
#include "Database.h"
#include <sstream>
#include <vector>

SearchCommand::SearchCommand(int fd, const std::string &input, const std::string& user)
    : client_fd(fd), username(user)
{
    std::stringstream ss(input);
    std::string cmd;
    ss >> cmd;   
    ss >> field; 
    std::getline(ss, value);
    if (!value.empty() && value[0] == ' ')
        value.erase(0, 1);
}

std::string SearchCommand::execute()
{
    if (username.empty()) {
        return "ACCESS DENIED: You must login first!\n"
               "   Use: login <username> <password>";
    }

    if (field.empty() || value.empty())
        return "Usage: search <field> <value>";

    Database db;
    db.open("books.db");

    db.logSearch(username, field + "=" + value);

    std::vector<Book> results;
    if (field == "genre") {
        results = db.searchByGenre(value);
    } else {
        results = db.searchBooks(field, value);
    }
    db.close();

    if (results.empty())
        return "No books found for: " + field + " = " + value;

    std::string out = "Results:\n";
    for (const auto &b : results)
    {
        out += "- " + b.title + " | " + b.author +
               " | ISBN: " + b.isbn + "\n";
    }
    return out;
}