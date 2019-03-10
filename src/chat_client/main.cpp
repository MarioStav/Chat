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
#include <fstream>

using namespace clipp;
using namespace std;

int main(int argc, char *argv[])
{
    short unsigned int port;
    bool help{false};
    bool admin{false};
    bool config{false};
    auto cli = (required("-p").doc("Client Port") & value("port", port),
                option("-h", "-- help").doc("help").set(help),
                option("-a", "-- admin").doc("admin").set(admin),
                option("-c", "--config").doc("config").set(config));

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
    if (config)
    {
        nlohmann::json config;
        ifstream i("../src/config.json");
        config = nlohmann::json::parse(i);
        ChatClient cclient(port, name, admin, config);
    }else{
        ChatClient cclient(port, name, admin);
    }

    
    return 0;
}
