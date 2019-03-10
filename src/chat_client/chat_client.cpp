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

void ChatClient::handleUserInput(ip::tcp::socket &_socket)
{

    try
    {
        string line;
        string channel{""};
        while (getline(cin, line))
        {
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf), end;

            std::vector<std::string> tokens(beg, end);
            if ((tokens.size() == 2) && (tokens.at(0) == "/join"))
            {
                cout << "joined channel " << tokens.at(1) << endl;
                channel = tokens.at(1);
                Chat::joinChannel joinChannelMessage;
                joinChannelMessage.set_channelname(tokens.at(1));
                networking::sendProto(_socket, joinChannelMessage);
            }
            else if ((tokens.size() == 1) && (tokens.at(0) == "/leave"))
            {
                cout << "left channel " << channel << endl;
                Chat::leaveChannel leaveChannelMessage;
                leaveChannelMessage.set_channelname(channel);
                channel = "";
                networking::sendProto(_socket, leaveChannelMessage);
            }
            else if ((tokens.size() == 2) && (tokens.at(0) == "/createChannel"))
            {  
                cout << "created channel " << tokens.at(1) << endl;
                Chat::createChannel createChannelMessage;
                createChannelMessage.set_channelname(tokens.at(1));
                networking::sendProto(_socket, createChannelMessage);
            }
            else if ((tokens.size() == 2) && (tokens.at(0) == "/deleteChannel"))
            {
                cout << "deleted channel " << tokens.at(1) << endl;
                Chat::deleteChannel deleteChannelMessages;
                deleteChannelMessages.set_channelname(tokens.at(1));
                networking::sendProto(_socket, deleteChannelMessages);
            }
            else if ((tokens.size() == 1) && (tokens.at(0) == "/showChannels"))
            {
                Chat::showChannels showChannelsMessage;
                networking::sendProto(_socket, showChannelsMessage);
            }
            else if ((tokens.size() == 1) && (tokens.at(0) == "/help"))
            {
                cout << "Invalid Command entered!" << endl;
                cout << "valid commands are: " << endl;
                cout << "/say <message> --> share a message with other users in your current channel" << endl;
                cout << "/signOut --> leave the server" << endl;
                cout << "/join <channelName> --> join the entered channel" << endl;
                cout << "/leave --> leave the current channel" << endl;
                cout << "/createChannel <channelName> (just available as admin) --> create a new channel" << endl;
                cout << "/showChannels --> show all channels" << endl;
            }
            else if ((tokens.size() == 1) && (tokens.at(0) == "/signOut"))
            {
                Chat::SignOut signOutMessage;
                networking::sendProto(_socket, signOutMessage);
            }
            else if ((tokens.size() >= 2) && (tokens.at(0) == "/say"))
            {
                if (channel != "")
                {
                    Chat::ChatMessage cm;
                    string textInput;
                    for (size_t i = 1; i < tokens.size(); i++)
                    {
                        textInput += tokens.at(i) + " ";
                    }
                    cm.set_text(textInput);
                    cm.set_channel(channel);
                    networking::sendProto(_socket, cm);
                }
                else
                {
                    cout << "You first have to join a channel before you can communicate with others!" << endl;
                    cout << "To do so, you have to enter the command /join <channelName>. " << endl;
                }
            }
            else
            {
                if (tokens.size() != 0)
                {
                    cout << "Invalid Command entered!" << endl;
                    cout << "valid commands are: " << endl;
                    cout << "/say <message> --> share a message with other users in your current channel" << endl;
                    cout << "/signOut --> leave the server" << endl;
                    cout << "/join <channelName> --> join the entered channel" << endl;
                    cout << "/leave --> leave the current channel" << endl;
                    cout << "/createChannel <channelName> (just available as admin) --> create a new channel" << endl;
                    cout << "/showChannels --> show all channels" << endl;
                }
            }
        }
    }catch(exception& e){
        cout << e.what() << endl;
    }
}

ChatClient::ChatClient(short unsigned int _port, string _name, bool admin)
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
        signInMessage.set_admin(admin);
        networking::sendProto(socket, signInMessage);

        std::thread t(&ChatClient::handleUserInput, this, ref(socket));
        while (true)
        {
            try
            {
                networking::MessageType mType;
                networking::receiveProtoMessageType(socket, mType);
                if (mType == networking::MessageType::ChatMessage)
                {
                    Chat::ChatMessage cm;
                    networking::receiveProtoMessage(socket, cm);
                    cout << "\n[" + cm.channel() + "]: " + cm.text() << endl;
                }
                else if (mType == networking::MessageType::showChannelsResponse)
                {
                    Chat::showChannelsResponse showChannelsResponseMessage;
                    networking::receiveProtoMessage(socket, showChannelsResponseMessage);
                    for (auto help : showChannelsResponseMessage.channels())
                    {
                        cout << help << endl;
                    }
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

        t.join();
    }
    catch (const asio::system_error &e)
    {
        spdlog::error("Unable to connect to server.");
        spdlog::error(e.what());
    }
}