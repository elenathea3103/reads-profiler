#include "TcpServer.h"
#include "Database.h"
#include "DbSeeder.h" 
#include <iostream>

using namespace std;

int main()
{
    Database db;
    if (!db.open("books.db")) {
        cerr << "Eroare la deschiderea bazei de date.\n";
        return 1;
    }

    db.createTables();
    
    // DOAR LA PRIMA RULARE se decomenteaza linia de mai jos (adica daca server/books si books.db nu sunt create deja)
    // daca trebuie resetat se sterg cele vechi si se ruleaza make iar

    //seedDatabase(db);  

    db.close();

    TcpServer server(8080);
    if (server.start() == false) {
        cout << "Server failed to start.\n";
        return 1;
    } 

    server.run();
    return 0;
}