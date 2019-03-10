/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_server/main.cpp
*/

#include "includes.h"
#include <iostream>
#include "chat_client.h"
#include "chat_server.h"
#include <fstream>

using namespace std;
using namespace asio;
using namespace clipp;

int main(int argc, char *argv[])
{
    
    bool help{false};
    bool conf{false};
    auto cli = (option("-h", "--help").set(help).doc("help"),
                option("-c", "--config").doc("configuration via JSON-file").set(conf));

    if (!parse(argc, argv, cli))
    {
        cout << make_man_page(cli, argv[0]);
        return 1;
    }
    else if (help)
    {
        cout << make_man_page(cli, argv[0]);
    }
    if (conf)
    {
        nlohmann::json config;
        ifstream i("../src/config.json");
        config = nlohmann::json::parse(i);
        ChatServer server(config);
    }
    else
    {

        ChatServer server();
    }
}
