#pragma once

/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: chat_client.h
*/

#include <string>
#include <thread>
#include "includes.h"

class Client{
private:
    std::string name;
    std::string ip;
    short unsigned int port;
    int id;
    std::shared_ptr<asio::ip::tcp::socket> socket;
    bool admin;
public:
    Client() {};
    Client(std::shared_ptr<asio::ip::tcp::socket> _socket) {this->socket = _socket;};
    void setName(std::string _name) {this->name = _name;};
    std::string getName() {return this->name;}
    void setIp(std::string _ip) {this->ip = _ip;};
    std::string getIp() {return this->ip;};
    void setId(int _id) {this->id = _id;};
    int getId() {return this->id;};
    void setPort(short unsigned int _port) {this->port = _port;};
    short unsigned int getPort() {return this->port;};
    void setAdmin(){this->admin = true;};
    bool getAdmin(){return this->admin;};
    std::shared_ptr<asio::ip::tcp::socket> getSocket() {return this->socket;};
};