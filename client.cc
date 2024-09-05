#include "client.h"
#include "chessexception.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <iostream>
#define PORT 2104

// Initialize TCP connection towards the server.
int Client::init(char* n){
    ChessException ce;
    int client_socket, message, close_socket;
    struct sockaddr_in server_addr;
    char reply[1024] ={0};
    char *r = reply;
    size_t reply_len = 1024;
    ssize_t linesize = 0;
    char hello[512];
    strcpy(hello, "Player ");
    strcat(hello, n);
    strcat(hello, " has joined the lobby. Woo Hoo~");
    this->name = n;
    try{
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0){
            std::cout << "Socket error" << std::endl;
            throw;
        }
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        char serveripaddr[32]={0};
        std::cout << "Enter the host IPv4 address" << std::endl;
        std::cin >> serveripaddr;

        if(inet_pton(AF_INET, serveripaddr, &server_addr.sin_addr)<=0){
            std::cout << "Invalid IP address" << std::endl;
            throw;
        }

        if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
            std::cout << "Connection error" << std::endl;
            throw;
        }
        std::cout << "Connected to host" << std::endl;
        send(client_socket, hello, strlen(hello), 0);
        message = read(client_socket, buffer,1024);
        std::cout << buffer << std::endl;//("%s\n",buffer);
        this->cli_socket = client_socket;
        for(int i = 0; i<1024;i++){
            buffer[i] = '\0';
        }
    }catch(...){
        std::cout << "Error caught" << std::endl;
        close_socket = shutdown(client_socket,2);
        if(close_socket==-1){
            perror("Error happened when closing socket");
            throw ce;
        }
        else {throw ce;}
    }
    return 1;
}

// Get methods
std::string Client::getname(){
    std::string s(this->name);
    return s;
}

// Recieve messages from server.
std::string Client::read_msg(){
    for (int i = 0; i<1024;i++){
        buffer[i] = '\0';
    }
    read(this->cli_socket, this->buffer, 1024);
    std::string s(this->buffer);
    return s;
}

// Send messages to server
void Client::send_msg(char* msg,int msg_len){
    send(this->cli_socket,msg,msg_len,0);
}
