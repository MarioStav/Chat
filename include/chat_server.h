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
#include <map>

class ChatServer
{
  private:
    std::vector<Client> clients;
    std::map<std::string, std::vector<int>> channels;
    int nextClientId = 0;

  public:
    ChatServer(){};
    ChatServer(short unsigned int _port);
    void handleClient(Client &_client);
    void broadcastMessage(Client &_client, std::string _text, std::string _channel);
    void createChannel(std::string _name);
    void joinChannel(std::string _channelName, int id);
};