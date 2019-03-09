#pragma once

/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_server.h
*/

#include "includes.h"
#include <string>
#include <iostream>
#include <vector>
#include "client.h"
#include "networking.h"
#include <thread>


class ChatServer{
private:
    std::vector<Client> clients;
    std::vector<std::vector<int>> channels;
    std::vector<std::string> channelNames;
public:
    ChatServer() {};
    ChatServer(short unsigned int _port);
    void handleClient(Client& _client);
    void broadcastMessage(Client& _client, std::string _text, std::string _channel);
};