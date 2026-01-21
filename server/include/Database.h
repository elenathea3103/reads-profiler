#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

struct Book
{
    int id;
    std::string title;
    std::string author;
    int year;
    std::string isbn;
    double rating;
};

class Database
{
public:
    Database();
    ~Database();

    bool open(const std::string &filename);
    void close();

    bool createTables();

    int insertBook(const std::string &title, const std::string &author,
                   int year, const std::string &isbn, double rating);

    int insertGenre(const std::string &name, int parent_id);
    bool assignBookGenre(int book_id, int genre_id);

    void logSearch(const std::string &client_ip, const std::string &query);
    void logDownload(const std::string &client_ip, int book_id);


    std::vector<Book> searchBooks(const std::string &field, const std::string &value);

    std::vector<Book> recommend(const std::string &client_ip);

    std::vector<Book> searchByGenre(const std::string &genreName);

    bool checkLogin(const std::string &user, const std::string &pass);
    void createUser(const std::string &user, const std::string &pass); 
    
    bool registerUser(const std::string& user, const std::string& pass);

private:
    sqlite3 *db;
};
