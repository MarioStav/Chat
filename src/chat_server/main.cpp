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

using namespace std;
using namespace asio;
using namespace clipp;

int main(int argc, char* argv[])
{
    short unsigned int port;
    bool help = false;
    auto cli = (option("-p", "--port").doc("port to connect to") & value("port", port),
                 option("-h", "--help").set(help).doc("help")
    );

    if (!parse(argc, argv, cli))
    {
        cout << make_man_page(cli, argv[0]);
        return 1;
    }
    else if (help)
    {
        cout << make_man_page(cli, argv[0]);
    }

    ChatServer server(port);

}
