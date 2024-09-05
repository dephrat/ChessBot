#ifndef SERVER_H
#define SERVER_H
#include <string>

class Server {
    char* name;
    int serv_socket;
    char buffer[1024]={0};
    public:
    // Initializing server
    int init(char* n);

    // Socket funcionality
    std::string read_msg();
    void send_msg(char* msg,int len);
    void close_server();

    // Get methods
    std::string getname();
    
};

#endif
