#include "RecommendCommand.h"
#include "Database.h"
#include <sstream>

using namespace std;

RecommendCommand::RecommendCommand(const std::string& user)
    : username(user) {}

std::string RecommendCommand::execute()
{
    if (username.empty())
    {
        return "ERROR: You must LOGIN to receive recommendations.\n"
               "Usage: login <username> <password>";
    }

    Database db;
    if (!db.open("books.db")) {
        return "ERROR: Could not access database.";
    }

    auto recs = db.recommend(username);

    db.close();

    if (recs.empty())
        return "No recommendations available yet. Try searching or downloading more books first!\n";

    std::stringstream out;
    out << "Top Recommendations for [" << username << "]:\n";
    out << "------------------------------------------------\n";

    for (auto &b : recs)
    {
        out << "[ISBN: " << b.isbn << "] " 
            << "\"" << b.title << "\" by " << b.author
            << " (Score: " << b.rating << ")\n";
    }
    out << "------------------------------------------------\n";

    return out.str();
}