#include "server.h"
#include "chessexception.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#define PORT 2104

// Initialize TCP server
int Server::init(char* n){
    ChessException ce;
    int server_socket, socket_accept, message, close_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char *reply = NULL;
    size_t reply_len = 0;
    ssize_t linesize = 0;
    this->name = n;
    char hello[512];
    strcpy(hello, "You ");
    strcat(hello, "have joined ");
    strcat(hello, n);
    strcat(hello, "'s lobby. Welcome!");
    try{
        server_socket = socket(AF_INET,SOCK_STREAM,0);
        if(server_socket == -1){
            perror("Socket error");
            throw;
        } 
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_socket, (struct sockaddr *)&address, sizeof(address))<0){
            perror("Bind error");
            throw;
        }

        if (listen(server_socket,3)<0){
            perror("Listen error");
            throw;
        }
        std::cout << "Server is now listening" << std::endl;
        std::cout << "Waiting for 1 player to join the lobby" << std::endl;
        socket_accept = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if(socket_accept < 0){
            perror("Accept error");
            throw;
        }
        message = read(socket_accept, buffer, 1024);
        send(socket_accept, hello, 512,0);
        this->serv_socket = socket_accept;
        std::cout << buffer << std::endl; //("%s\n",buffer);
        for (int i = 0; i<1024;i++){
            buffer[i] = '\0';
        }
    }catch(ChessException){
        std::cout << "Catch error" << std::endl;
        std::cout << "Closing socket" << std::endl;
        close_socket = shutdown(server_socket,2);
        if(close_socket==-1){
            perror("Error happened when closing socket");
        }
        std::cout << "Socket closed" << std::endl;
        throw ce; 
    }
    return 1;
}

// Get methods
std::string Server::getname(){
    std::string s(this->name);
    return s;
}

// Closing TCP connections. End both reception and transmission.
void Server::close_server(){
    int close_socket = shutdown(this->serv_socket,2);
    if(close_socket==-1){
        perror("Error happened when closing socket");
    }
}

// Recieve message through TCP socket.
std::string Server::read_msg(){
    for (int i = 0; i<1024;i++){
        this->buffer[i] = '\0';
    }
    read(this->serv_socket, this->buffer, 1024);
    std::string s(this->buffer);
    return s;
}

// Send messages through TCP socket.
void Server::send_msg(char* msg,int len){
    send(this->serv_socket,msg,len,0);
}
