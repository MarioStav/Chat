/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_client.h
*/

#include "includes.h"
#include <iostream>
#include "chat_client.h"
#include <string>

using namespace clipp;
using namespace std;

int main(int argc, char* argv[])
{
    short unsigned int port;
    bool help = false;
    bool admin = false;
    auto cli = (required("-p").doc("Client Port") & value("port", port),
                option("-h", "-- help").doc("help").set(help),
                option("-a", "-- admin").doc("admin").set(admin));

    if (!parse(argc, argv, cli))
    {
        cout << make_man_page(cli, argv[0]);
        exit(0);
    }
    else if (help)
    {
        cout << make_man_page(cli, argv[0]);
    }




    cout << "Enter Username: ";
    string name;
    cin >> name;
    cout << "Welcome " << name << "!" << endl;
    ChatClient cclient(port, name);
    return 0;
}
