#ifndef CLIENT_H
#define CLIENT_H
#include <string>

class Client{
    char* name;
    int cli_socket;
    char buffer[1024]={0};
    public:
    // Initializing connection
    int init(char* n);

    // Socket functionality
    std::string read_msg();
    void send_msg(char* msg, int msg_len);

    // Get methods
    std::string getname();
    
};

#endif
