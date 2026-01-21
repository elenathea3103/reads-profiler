#include "DownloadCommand.h"
#include "Database.h"
#include <fstream>

using namespace std;

DownloadCommand::DownloadCommand(const std::string &input, const std::string& user)
    : username(user)
{
    size_t pos = input.find(' ');
    if (pos != string::npos)
        isbn = input.substr(pos + 1);
}

std::string DownloadCommand::execute()
{
    if (username.empty()) {
        return "ACCESS DENIED: You must login first to download files!";
    }

    std::string path = "books/" + isbn + ".txt";
    ifstream f(path, ios::binary);
    if (!f) return "ERROR: File not found (" + isbn + ")";

    Database db;
    if (db.open("books.db")) {

        auto books = db.searchBooks("isbn", isbn);
        if (!books.empty()) {
            db.logDownload(username, books[0].id);
        }
        db.close();
    }

    string content((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());
    return content;
}