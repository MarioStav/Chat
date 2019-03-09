/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_client.cpp
*/

#include "chat_client.h"

using namespace std;
using namespace asio;

void ChatClient::handleUserInput(ip::tcp::socket& _socket)
{

    string line;
    while (getline(cin, line))
    {
        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf), end;

        std::vector<std::string> tokens(beg, end);
        if ((tokens.size() == 2) && (tokens.at(0) == "/join" ))
        {
            cout << "joining channel " << tokens.at(1) << endl;
        }
        else if ((tokens.size() == 1) && (tokens.at(0) == "/signout"))
        {
            cout << "signing out" << endl;
            Chat::SignOut signOutMessage;
            networking::sendProto(_socket, signOutMessage);
        }
    }
}

ChatClient::ChatClient(short unsigned int _port, string _name)
{
    try
    {
        io_context ctx;
        ip::tcp::resolver resolver{ctx};
        auto results = resolver.resolve("127.0.0.1", "7777");
        ip::tcp::socket socket{ctx};
        asio::connect(socket, results);

        Chat::SignIn signInMessage;
        signInMessage.set_name(_name);
        signInMessage.set_ip(socket.local_endpoint().address().to_string());
        signInMessage.set_port(_port);
        networking::sendProto(socket, signInMessage);

        ip::tcp::endpoint ep{ip::tcp::v4(), _port};
        ip::tcp::acceptor acc{ctx, ep};
        while (true)
        {
            std::thread t(&ChatClient::handleUserInput, this, ref(socket));
            t.join();
            try
            {
                acc.listen();
                ip::tcp::socket socketLi{acc.accept()};
                networking::MessageType mType;
                networking::receiveProtoMessageType(socketLi, mType);
                if (mType == networking::MessageType::ChatMessage)
                {
                    Chat::ChatMessage cm;
                    networking::receiveProtoMessage(socketLi, cm);
                    cout << "\n[" + cm.channel() + "]: " + cm.text() << endl;
                }
                else
                {
                    spdlog::error("Reiceved invalid message.");
                }
            }
            catch (const asio::system_error &e)
            {
                spdlog::error("Could not connect to server.");
                spdlog::error(e.what());
            }
        }
    }
    catch (const asio::system_error &e)
    {
        spdlog::error("Can't connect to server.");
        spdlog::error(e.what());
    }
}