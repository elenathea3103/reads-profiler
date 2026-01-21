#include "Database.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sqlite3.h>
#include <unordered_set>

using namespace std;

Database::Database() : db(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open(const std::string& filename) {
    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open DB: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    return true;
}

void Database::close() {
    if (db) sqlite3_close(db);
    db = nullptr;
}

bool Database::createTables() {
    const char* sql = R"(

    CREATE TABLE IF NOT EXISTS books (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        author TEXT NOT NULL,
        year INTEGER,
        isbn TEXT UNIQUE NOT NULL,
        rating REAL
    );

    CREATE TABLE IF NOT EXISTS genres (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        parent_id INTEGER,
        FOREIGN KEY (parent_id) REFERENCES genres(id)
    );

    CREATE TABLE IF NOT EXISTS book_genres (
        book_id INTEGER NOT NULL,
        genre_id INTEGER NOT NULL,
        FOREIGN KEY (book_id) REFERENCES books(id),
        FOREIGN KEY (genre_id) REFERENCES genres(id)
    );

    CREATE TABLE IF NOT EXISTS authors (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS author_genres (
        author_id INTEGER,
        genre_id INTEGER,
        FOREIGN KEY (author_id) REFERENCES authors(id),
        FOREIGN KEY (genre_id) REFERENCES genres(id)
    );

    CREATE TABLE IF NOT EXISTS search_history (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        client_ip TEXT, 
        query TEXT,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    );

    CREATE TABLE IF NOT EXISTS downloads (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        client_ip TEXT,
        book_id INTEGER,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (book_id) REFERENCES books(id)
    );

    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT UNIQUE NOT NULL,
        password TEXT NOT NULL
    );

    )";

    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "SQL error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

// login system

bool Database::checkLogin(const std::string& user, const std::string& pass) {
    std::string sql = "SELECT id FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* st;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr) != SQLITE_OK) 
        return false;

    sqlite3_bind_text(st, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, pass.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = (sqlite3_step(st) == SQLITE_ROW);
    
    sqlite3_finalize(st);
    return exists;
}

void Database::createUser(const std::string& user, const std::string& pass) {
    std::string sql = "INSERT OR IGNORE INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt* st;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr);
    sqlite3_bind_text(st, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, pass.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(st);
    sqlite3_finalize(st);
}

// data insertion

int Database::insertBook(const std::string& title, const std::string& author,
                         int year, const std::string& isbn, double rating)
{
    const char* sql = "INSERT INTO books (title, author, year, isbn, rating) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* st;

    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(st, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 3, year);
    sqlite3_bind_text(st, 4, isbn.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(st, 5, rating);

    bool ok = (sqlite3_step(st) == SQLITE_DONE);
    sqlite3_finalize(st);

    if (!ok) return -1;
    return sqlite3_last_insert_rowid(db);
}

int Database::insertGenre(const std::string& name, int parent_id) {
    const char* sql = "INSERT INTO genres (name, parent_id) VALUES (?, ?);";
    sqlite3_stmt* st;

    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(st, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    if (parent_id == -1)
        sqlite3_bind_null(st, 2);
    else
        sqlite3_bind_int(st, 2, parent_id);

    bool ok = (sqlite3_step(st) == SQLITE_DONE);
    sqlite3_finalize(st);

    if (!ok) return -1;
    return sqlite3_last_insert_rowid(db);
}

bool Database::assignBookGenre(int book_id, int genre_id) {
    const char* sql = "INSERT INTO book_genres (book_id, genre_id) VALUES (?, ?);";
    sqlite3_stmt* st;

    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(st, 1, book_id);
    sqlite3_bind_int(st, 2, genre_id);

    bool ok = (sqlite3_step(st) == SQLITE_DONE);
    sqlite3_finalize(st);

    return ok;
}


void Database::logSearch(const std::string& userIdentifier, const std::string& query) {
    const char* sql = "INSERT INTO search_history (client_ip, query) VALUES (?, ?);";
    sqlite3_stmt* st;

    sqlite3_prepare_v2(db, sql, -1, &st, nullptr);
    sqlite3_bind_text(st, 1, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, query.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(st);
    sqlite3_finalize(st);
}

void Database::logDownload(const std::string& userIdentifier, int book_id) {
    const char* sql = "INSERT INTO downloads (client_ip, book_id) VALUES (?, ?);";
    sqlite3_stmt* st;

    sqlite3_prepare_v2(db, sql, -1, &st, nullptr);
    sqlite3_bind_text(st, 1, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, book_id);

    sqlite3_step(st);
    sqlite3_finalize(st);
}

// search

std::vector<Book> Database::searchBooks(const std::string& field, const std::string& value) {
    std::vector<Book> result;
    std::string sql = "SELECT id, title, author, year, isbn, rating FROM books WHERE " + field + " LIKE ?;";

    sqlite3_stmt* st;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr);
    std::string like = "%" + value + "%";
    sqlite3_bind_text(st, 1, like.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(st) == SQLITE_ROW) {
        Book b;
        b.id = sqlite3_column_int(st, 0);
        b.title = (const char*)sqlite3_column_text(st, 1);
        b.author = (const char*)sqlite3_column_text(st, 2);
        b.year = sqlite3_column_int(st, 3);
        b.isbn = (const char*)sqlite3_column_text(st, 4);
        b.rating = sqlite3_column_double(st, 5);
        result.push_back(b);
    }

    sqlite3_finalize(st);
    return result;
}

std::vector<Book> Database::searchByGenre(const std::string& genreName) {
    std::vector<Book> results;

    const char* sql = R"(
        WITH RECURSIVE subgenres(id) AS (
            SELECT id FROM genres WHERE name = ?
            UNION ALL
            SELECT g.id
            FROM genres g
            JOIN subgenres sg ON g.parent_id = sg.id
        )
        SELECT DISTINCT books.id, books.title, books.author, books.year, books.isbn, books.rating
        FROM books
        JOIN book_genres ON books.id = book_genres.book_id
        WHERE book_genres.genre_id IN (SELECT id FROM subgenres)
    )";

    sqlite3_stmt* st;

    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) {
        std::cerr << "SQL ERROR: " << sqlite3_errmsg(db) << "\n";
        return results;
    }

    sqlite3_bind_text(st, 1, genreName.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(st) == SQLITE_ROW) {
        Book b;
        b.id = sqlite3_column_int(st, 0);
        b.title = (const char*)sqlite3_column_text(st, 1);
        b.author = (const char*)sqlite3_column_text(st, 2);
        b.year = sqlite3_column_int(st, 3);
        b.isbn = (const char*)sqlite3_column_text(st, 4);
        b.rating = sqlite3_column_double(st, 5);
        results.push_back(b);
    }

    sqlite3_finalize(st);
    return results;
}

// recommend

std::vector<Book> Database::recommend(const std::string& userIdentifier)
{
    std::vector<Book> result;

    // ce carti a descarcat deja user ul
    std::vector<int> myBooks;
    {
        const char* sql = "SELECT book_id FROM downloads WHERE client_ip = ?;";
        sqlite3_stmt* st;
        sqlite3_prepare_v2(db, sql, -1, &st, nullptr);
        sqlite3_bind_text(st, 1, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(st) == SQLITE_ROW)
            myBooks.push_back(sqlite3_column_int(st, 0));

        sqlite3_finalize(st);
    }

    std::vector<std::string> topGenres;
    {
        const char* sql = R"(
            WITH dl AS (
                SELECT g.name AS genre_name, COUNT(*) AS dl_count
                FROM downloads d
                JOIN books b ON d.book_id = b.id
                JOIN book_genres bg ON b.id = bg.book_id
                JOIN genres g ON bg.genre_id = g.id
                WHERE d.client_ip = ?
                GROUP BY g.name
            ),
            sh AS (
                SELECT SUBSTR(query, 7) AS genre_name, COUNT(*) AS sh_count
                FROM search_history
                WHERE client_ip = ? AND query LIKE 'genre=%'
                GROUP BY genre_name
            )
            SELECT 
                COALESCE(dl.genre_name, sh.genre_name) AS genre,
                (COALESCE(dl.dl_count,0)*3 + COALESCE(sh.sh_count,0)) AS score
            FROM dl
            FULL OUTER JOIN sh ON dl.genre_name = sh.genre_name
            ORDER BY score DESC
            LIMIT 3;
        )";

        sqlite3_stmt* st;
        if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(st, 1, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 2, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);

            while (sqlite3_step(st) == SQLITE_ROW)
            {
                const unsigned char* g = sqlite3_column_text(st, 0);
                if (g) topGenres.push_back((const char*)g);
            }
        }
        sqlite3_finalize(st);
    }

    for (const auto& g : topGenres)
    {
        const char* sql = R"(
            SELECT DISTINCT b.id, b.title, b.author, b.year, b.isbn, b.rating
            FROM books b
            JOIN book_genres bg ON b.id = bg.book_id
            JOIN genres gr ON bg.genre_id = gr.id
            WHERE gr.name = ?
            AND b.id NOT IN (
                SELECT book_id FROM downloads WHERE client_ip = ?
            ) 
            ORDER BY b.rating DESC
            LIMIT 10;
        )";

        sqlite3_stmt* st;
        if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
            continue;

        sqlite3_bind_text(st, 1, g.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(st, 2, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(st) == SQLITE_ROW)
        {
            Book b;
            b.id = sqlite3_column_int(st, 0);
            b.title = (const char*)sqlite3_column_text(st, 1);
            b.author = (const char*)sqlite3_column_text(st, 2);
            b.year = sqlite3_column_int(st, 3);
            b.isbn = (const char*)sqlite3_column_text(st, 4);
            b.rating = sqlite3_column_double(st, 5);
            result.push_back(b);
        }

        sqlite3_finalize(st);
    }

    std::unordered_map<int, double> bonusScore;

    if (!myBooks.empty())
    {
        std::vector<std::string> similarUsers;
        {
            std::string sql =
                "SELECT DISTINCT client_ip FROM downloads WHERE client_ip != ? AND book_id IN (";

            for (size_t i = 0; i < myBooks.size(); i++)
            {
                sql += std::to_string(myBooks[i]);
                if (i + 1 < myBooks.size()) sql += ",";
            }
            sql += ");";

            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr);
            sqlite3_bind_text(st, 1, userIdentifier.c_str(), -1, SQLITE_TRANSIENT);

            while (sqlite3_step(st) == SQLITE_ROW)
                similarUsers.push_back((const char*)sqlite3_column_text(st, 0));

            sqlite3_finalize(st);
        }

        for (auto& other : similarUsers)
        {
            const char* sql = "SELECT book_id FROM downloads WHERE client_ip = ?;";
            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sql, -1, &st, nullptr);
            sqlite3_bind_text(st, 1, other.c_str(), -1, SQLITE_TRANSIENT);

            while (sqlite3_step(st) == SQLITE_ROW)
            {
                int bookId = sqlite3_column_int(st, 0);
                if (std::find(myBooks.begin(), myBooks.end(), bookId) != myBooks.end())
                    continue;

                bonusScore[bookId] += 3;
            }
            sqlite3_finalize(st);
        }
    }

    for (auto& b : result)
        if (bonusScore.count(b.id))
            b.rating += bonusScore[b.id];

    
    std::sort(result.begin(), result.end(),
        [](const Book& a, const Book& b){
            return a.rating > b.rating;
        }
    );

    std::unordered_set<int> seen;
    std::vector<Book> unique;
    for (const auto& b : result)
    {
        if (!seen.count(b.id))
        {
            unique.push_back(b);
            seen.insert(b.id);
        }
    }

    return unique;
}

bool Database::registerUser(const std::string& user, const std::string& pass) {
    const char* sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt* st;

    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) 
        return false; 

    sqlite3_bind_text(st, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, pass.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(st) == SQLITE_DONE);

    sqlite3_finalize(st);
    return success;
}