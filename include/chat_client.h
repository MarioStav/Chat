#pragma once

/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_client.h
*/

#include <string>
#include "includes.h"
#include "networking.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>

class ChatClient{
public:
    nlohmann::json config;
    ChatClient(){};
    ChatClient(short unsigned int _port, std::string _name, bool admin);
    ChatClient(short unsigned int _port, std::string _name, bool admin, nlohmann::json _config);
    void signIn(asio::ip::tcp::socket& _socket);
    void handleUserInput(asio::ip::tcp::socket& _socket); 
    void startClient(short unsigned int _port, std::string _name, bool admin);
};
