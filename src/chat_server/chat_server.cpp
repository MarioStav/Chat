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

void ChatServer::multicastMessage(Client &_client, string _text, string _channel)
{

    try
    {
        for (auto pair : this->channels)
        {
            if (find(pair.second.begin(), pair.second.end(), _client.getId()) != pair.second.end())
            {
                for (size_t id = 0; id < pair.second.size(); id++)
                {
                    Chat::ChatMessage cm;
                    cm.set_text(_text);
                    cm.set_channel(_channel);
                    networking::sendProto(*(this->clients.at(id).getSocket()), cm);
                }
            }
        }
    }
    catch (asio::system_error &e)
    {
        spdlog::error("Couldn't broadcast chat message to clients.");
        spdlog::error(e.what());
    }
}

void ChatServer::handleClient(Client &_client)
{
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
                if(signInMessage.admin()){
                    _client.setAdmin();
                }
                broadcastMessage(ref(_client), _client.getName() + " just signed in!", "SystemMessage");
            }
            else if (messageType == networking::MessageType::ChatMessage)
            {
                Chat::ChatMessage cm;
                networking::receiveProtoMessage(*(_client.getSocket()), cm);
                multicastMessage(_client, _client.getName() + " said --> " + cm.text(), cm.channel());
                spdlog::info(_client.getName() + " said --> " + cm.text() + " in channel: " + cm.channel());
            }
            else if (messageType == networking::MessageType::createChannel)
            {

                Chat::createChannel createChannelMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), createChannelMessage);
                if (_client.getAdmin())
                {
                    createChannel(createChannelMessage.channelname());
                }
                else
                {
                    spdlog::error("You have to be admin");
                }
            }
            else if (messageType == networking::MessageType::deleteChannel)
            {
                Chat::deleteChannel deleteChannelMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), deleteChannelMessage);
                if (_client.getAdmin())
                {
                    deleteChannel(deleteChannelMessage.channelname());
                }
                else
                {
                    spdlog::error("You have to be admin");
                }
            }
            else if (messageType == networking::MessageType::joinChannel)
            {
                Chat::joinChannel joinChannelMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), joinChannelMessage);
                joinChannel(joinChannelMessage.channelname(), _client.getId());
            }
            else if (messageType == networking::MessageType::leaveChannel)
            {
                Chat::leaveChannel leaveChannelMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), leaveChannelMessage);
                leaveChannel(leaveChannelMessage.channelname(), _client.getId());
                spdlog::info("Client {} left channel {}", _client.getName(), leaveChannelMessage.channelname());
            }
            else if (messageType == networking::MessageType::showChannels)
            {
                Chat::showChannels showChannelsMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), showChannelsMessage);
                showChannels(ref(_client));
                spdlog::info("showing channels...");
            }
            else if (messageType == networking::MessageType::SignOut)
            {
                Chat::SignOut signOutMessage;
                networking::receiveProtoMessage(*(_client.getSocket()), signOutMessage);
                _client.getSocket()->close();
            }
            else
            {
                spdlog::error("Invalid messagetype received");
            }
        }
    }

    catch (asio::system_error &e)
    {
        spdlog::error("Client Disconnected");
        spdlog::error(e.what());
    }
}

ChatServer::ChatServer(short unsigned int _port)
{
    io_context ctx;
    ip::tcp::endpoint ep{ip::tcp::v4(), _port};
    ip::tcp::acceptor acc{ctx, ep};
    acc.listen();

    std::vector<std::thread> threads;
    spdlog::info("Waiting for clients");
    while (true)
    {
        Client client = make_shared<asio::ip::tcp::socket>(acc.accept());

        client.setId(nextClientId);
        clients.resize(nextClientId + 1);
        clients.at(nextClientId) = client;
        nextClientId++;

        std::thread t(&ChatServer::handleClient, this, ref(client));
        t.detach();
    }

    // for (auto &t : threads)
    // {
    //     t.join();
    // }
}

void ChatServer::createChannel(std::string _name)
{

    this->channels.insert(pair<string, vector<int>>(_name, vector<int>{}));
    spdlog::info("Channel " + _name + " created!");
}

void ChatServer::deleteChannel(std::string _name)
{

    this->channels.erase(_name);
    spdlog::info("Channel " + _name + " deleted!");
}

void ChatServer::joinChannel(std::string _channelName, int id)
{
    if (this->channels.count(_channelName) > 0)
    {
        this->channels[_channelName].push_back(id);
        spdlog::info("Client joined channel {}", _channelName);
    }
}

void ChatServer::leaveChannel(std::string _channelName, int id)
{
    if (this->channels.count(_channelName) > 0)
    {
        this->channels[_channelName].erase(remove(
                                               this->channels[_channelName].begin(),
                                               this->channels[_channelName].end(),
                                               id),
                                           this->channels[_channelName].end());
    }
}

void ChatServer::showChannels(Client &_client)
{
    Chat::showChannelsResponse showChannelsResponseMessage;
    for (auto &pair : this->channels)
    {
        showChannelsResponseMessage.add_channels(pair.first);
    }
    networking::sendProto(*(_client.getSocket()), showChannelsResponseMessage);
}