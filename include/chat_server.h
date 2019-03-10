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
#include <algorithm>

class ChatServer
{
private:
  std::vector<Client> clients;
  std::map<std::string, std::vector<int>> channels;

public:
  ChatServer();
  ChatServer(nlohmann::json config);
  void handleClient(Client &_client);
  void broadcastMessage(Client &_client, std::string _text, std::string _channel);
  void createChannel(std::string _name);
  void joinChannel(std::string _channelName, int id);
  void leaveChannel(std::string _channelName, int id);
  void showChannels(Client &_client);
  void multicastMessage(Client &_client, std::string _text, std::string _channel);
  void deleteChannel(std::string _name);
};