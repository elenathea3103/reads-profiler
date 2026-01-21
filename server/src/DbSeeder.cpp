#include "DbSeeder.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h> 

using namespace std;

void createDummyFile(const string& isbn, const string& title, const string& contentSnippet) {
    string path = "books/" + isbn + ".txt";
    ofstream f(path);
    if (f.is_open()) {
        f << "TITLU: " << title << "\n";
        f << "------------------------\n";
        f << contentSnippet << "\n\n";
        f << "(Acesta este un fisier generat automat pentru testarea download-ului.)";
        f.close();
    }
}

void seedDatabase(Database& db) {
    cout << "--- Incepem popularea bazei de date (Seeding) ---\n";

    mkdir("books", 0777);

    int gFictiune = db.insertGenre("Fictiune", -1);
    int gSF = db.insertGenre("SF", gFictiune);
    int gFantasy = db.insertGenre("Fantasy", gFictiune);
    
    int gEdu = db.insertGenre("Educational", -1);
    int gIT = db.insertGenre("Programare", gEdu);
    int gIstorie = db.insertGenre("Istorie", gEdu);

    
    int b1 = db.insertBook("Dune", "Frank Herbert", 1965, "1111", 4.9);
    if (b1 != -1) {
        db.assignBookGenre(b1, gSF);
        createDummyFile("1111", "Dune", "Arrakis... planeta desertului.");
    }

    int b2 = db.insertBook("Dune Messiah", "Frank Herbert", 1969, "1112", 4.5);
    if (b2 != -1) {
        db.assignBookGenre(b2, gSF);
        createDummyFile("1112", "Dune Messiah", "Paul Atreides este imparat.");
    }

    int b3 = db.insertBook("The C++ Programming Language", "Bjarne Stroustrup", 2013, "2222", 4.8);
    if (b3 != -1) {
        db.assignBookGenre(b3, gIT);
        createDummyFile("2222", "The C++ Lang", "Biblia C++ scrisa de creatorul sau.");
    }

    int b4 = db.insertBook("Clean Code", "Robert C. Martin", 2008, "2223", 4.7);
    if (b4 != -1) {
        db.assignBookGenre(b4, gIT);
        createDummyFile("2223", "Clean Code", "Chiar daca codul merge, daca e urat...");
    }

    int b5 = db.insertBook("Harry Potter and the Philosopher's Stone", "J.K. Rowling", 1997, "3333", 4.9);
    if (b5 != -1) {
        db.assignBookGenre(b5, gFantasy);
        createDummyFile("3333", "Harry Potter 1", "You're a wizard, Harry!");
    }

    int b6 = db.insertBook("The Hobbit", "J.R.R. Tolkien", 1937, "3334", 5.0);
    if (b6 != -1) {
        db.assignBookGenre(b6, gFantasy);
        createDummyFile("3334", "The Hobbit", "Intr-o gaura in pamant traia un hobbit.");
    }

    int b7 = db.insertBook("Sapiens", "Yuval Noah Harari", 2011, "4444", 4.6);
    if (b7 != -1) {
        db.assignBookGenre(b7, gIstorie);
        createDummyFile("4444", "Sapiens", "O scurta istorie a omenirii.");
    }

    // 2 useri de baza pentru teste
    db.createUser("profesor", "admin");
    db.createUser("student", "1234");

    cout << "--- Populare completa! Cartile si fisierele au fost create. ---\n";
}