/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_server.cpp
*/

#include "../include/chat_server.h"

using namespace asio;
using namespace std;

void ChatServer::broadcastMessage(Client &_client, string _text, string _channel)
{
    for (auto client : this->clients)
    {
        if (client.getId() != _client.getId())
        {
            try
            {
                io_context ctx;
                ip::tcp::resolver resolver{ctx};
                auto results = resolver.resolve(client.getIp(), to_string(client.getPort()));
                ip::tcp::socket socket(ctx);
                asio::connect(socket, results);

                Chat::ChatMessage cm;
                cm.set_text(_text);
                cm.set_channel(_channel);
                networking::sendProto(socket, cm);
            }catch (asio::system_error& e){
                spdlog::info("Couldn't send chat message to clients.");
            }
        }
    }
}

void ChatServer::handleClient(Client &_client)
{

    if (clients.size() > 0)
    {
        _client.setId(clients.size() - 1);
    }
    else
    {
        _client.setId(0);
    }

    networking::MessageType messageType;
    networking::receiveProtoMessageType(*(_client.getSocket()), messageType);

    if (messageType == networking::MessageType::SignIn)
    {
        Chat::SignIn signInMessage;
        networking::receiveProtoMessage(*(_client.getSocket()), signInMessage);
        _client.setName(signInMessage.name());
        _client.setIp(signInMessage.ip());
        _client.setPort(signInMessage.port());
        broadcastMessage(ref(_client), _client.getName() + " just signed in!", "all");
    }else if(messageType == networking::MessageType::SignOut){
        Chat::SignOut signOutMessage;
        networking::receiveProtoMessage(*(_client.getSocket()), signOutMessage);
        broadcastMessage(ref(_client), _client.getName() + " logged out.", "all");
        _client.getSocket()->close();
    }
    else
    {
        spdlog::error("Received invalid Message");
    }
}

ChatServer::ChatServer(short unsigned int _port)
{
    io_context ctx;
    ip::tcp::endpoint ep{ip::tcp::v4(), _port};
    ip::tcp::acceptor acc{ctx, ep};
    acc.listen();

    while (true)
    {
        spdlog::info("Waiting for clients");
        Client client = make_shared<asio::ip::tcp::socket>(acc.accept());
        this->clients.push_back(client);
        std::thread t(&ChatServer::handleClient, this, ref(clients.at(clients.size() - 1)));
        t.join();
    }
}