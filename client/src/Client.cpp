#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>

using namespace std;

Client::Client(const string &ip, int port)
    : serverIp(ip), serverPort(port), sockfd(-1) {}

bool Client::connectToServer()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return false;
    }

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIp.c_str(), &serv.sin_addr) <= 0)
    {
        cout << "Invalid IP address\n";
        return false;
    }

    if (connect(sockfd, (sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("connect FAILED");
        close(sockfd);
        sockfd = -1;
        return false;
    }

    cout << "Connected to server.\n";
    return true;
}

void Client::sendMessage(const string &msg)
{
    int len = msg.size();
    send(sockfd, &len, sizeof(len), 0);
    send(sockfd, msg.c_str(), len, 0);
}

string Client::receiveMessage()
{
    int type;
    int r1 = recv(sockfd, &type, sizeof(type), 0);
    if (r1 <= 0)
        return "__ERROR__";

    int len;
    int r2 = recv(sockfd, &len, sizeof(len), 0);
    if (r2 <= 0)
        return "__ERROR__";

    if (len < 0 || len > 10'000'000)
        return "__ERROR__";

    string buff(len, '\0');
    int r3 = recv(sockfd, buff.data(), len, 0);
    if (r3 <= 0)
        return "__ERROR__";

    return buff;
}

void Client::downloadFile(const std::string &isbn)
{
    int type, len;

    if (recv(sockfd, &type, sizeof(type), 0) <= 0)
    {
        cout << "Download failed.\n";
        return;
    }

    if (recv(sockfd, &len, sizeof(len), 0) <= 0)
    {
        cout << "Download failed.\n";
        return;
    }

    if (len < 0 || len > 100'000'000)
    {
        cout << "Download failed (invalid length).\n";
        return;
    }

    string data(len, '\0');
    if (recv(sockfd, data.data(), len, 0) <= 0)
    {
        cout << "Download failed.\n";
        return;
    }

    if (type != 1)
    {
        cout << "SERVER ERROR: " << data << "\n";
        return;
    }

    string home = getenv("HOME");
    string outPath = home + "/Downloads/" + isbn + ".txt";

    ofstream out(outPath, ios::binary);
    out.write(data.c_str(), data.size());
    out.close();

    cout << "File downloaded to: " << outPath << "\n";
}

void Client::run()
{
    cout << "\n";
    cout << "=================================================================\n";
    cout << "             WELCOME TO THE VIP ONLINE LIBRARY \n";
    cout << "=================================================================\n";
    cout << "AVAILABLE COMMANDS:\n\n";
    
    cout << "1. AUTHENTICATION (Start here):\n";
    cout << "   > signup <user> <pass>      -> Create a new VIP account\n";
    cout << "   > login <user> <pass>       -> Access your profile\n\n";
    
    cout << "2. LIBRARY ACTIONS (Require Login):\n";
    cout << "   > search title <name>       -> Search books by title\n";
    cout << "   > search author <name>      -> Search books by author\n";
    cout << "   > search genre <name>       -> Search by genre (e.g. SF)\n";
    cout << "   > download <isbn>           -> Download book file\n";
    cout << "   > recommend                 -> Get smart AI suggestions\n\n";
    
    cout << "3. SYSTEM:\n";
    cout << "   > quit                      -> Exit the application\n";
    cout << "=================================================================\n\n";

    while (true)
    {
        cout << "VIP-Library> "; 
        string cmd;
        getline(cin, cmd);

        //if (cmd.empty())
            //continue;

        sendMessage(cmd);

        if (cmd == "quit")
        {
            cout << "Closing client...\n";
            break;
        }

        if (cmd.rfind("download ", 0) == 0)
        {
            string isbn = cmd.substr(9);
            downloadFile(isbn);
            continue;
        }

        string response = receiveMessage();

        if (response == "__ERROR__")
        {
            cout << "Lost connection to server.\n";
            break;
        }

        cout << "\nSERVER RESPONSE: " << response << "\n\n";
    }

    close(sockfd);
}
