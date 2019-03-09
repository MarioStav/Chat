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
    for (auto &client : this->clients)
    {
        if (client.getId() != _client.getId())
        {
            try
            {

                Chat::ChatMessage cm;
                cm.set_text(_text);
                cm.set_channel(_channel);
                networking::sendProto(*(client.getSocket()), cm);
            }
            catch (asio::system_error &e)
            {
                spdlog::error("Couldn't broadcast chat message to clients.");
                spdlog::error(e.what());
            }
        }
    }
}

void ChatServer::handleClient(Client &_client)
{
    _client.setId(nextClientId);
    clients.reserve(nextClientId + 1);
    clients.at(nextClientId) = _client;
    nextClientId++;

    try
    {
        while (true)
        {
            networking::MessageType messageType;
            networking::receiveProtoMessageType(*(_client.getSocket()), messageType);

            if (messageType == networking::MessageType::SignIn)
            {
                Chat::SignIn signInMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), signInMessage);
                _client.setName(signInMessage.name());
                _client.setIp(signInMessage.ip());
                _client.setPort(signInMessage.port());
                _client.setAdmin();
                broadcastMessage(ref(_client), _client.getName() + " just signed in!", "all");
            }
            else if (messageType == networking::MessageType::SignOut)
            {
                Chat::SignOut signOutMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), signOutMessage);
                broadcastMessage(ref(_client), _client.getName() + " logged out.", "all");
                _client.getSocket()->close();
            }
            else if (messageType == networking::MessageType::createChannel)
            {
                Chat::createChannel createChannelMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), createChannelMessage);
            }
            else if (messageType == networking::MessageType::joinChannel)
            {
                Chat::joinChannel joinChannelMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), joinChannelMessage);
                joinChannel(joinChannelMessage.channelname(), _client.getId());
                spdlog::info("Client {} joined channel {}", _client.getName(), joinChannelMessage.channelname());
            }
            else
            {
                spdlog::error("Received invalid Message");
            }
        }
    }
    catch (asio::system_error &e)
    {
        spdlog::error("Client Disconnected");
    }
}

ChatServer::ChatServer(short unsigned int _port)
{
    io_context ctx;
    ip::tcp::endpoint ep{ip::tcp::v4(), _port};
    ip::tcp::acceptor acc{ctx, ep};
    acc.listen();

    std::vector<std::thread> threads;

    while (true)
    {
        spdlog::info("Waiting for clients");
        Client client = make_shared<asio::ip::tcp::socket>(acc.accept());

        threads.push_back(std::thread(&ChatServer::handleClient, this, ref(clients.at(clients.size() - 1))));
    }

    for (auto &t : threads)
    {
        t.join();
    }
}

void ChatServer::createChannel(std::string _name)
{
    this->channels[_name] = vector<int>{};
}

void ChatServer::joinChannel(std::string _channelName, int id)
{
    this->channels[_channelName].push_back(id);
}