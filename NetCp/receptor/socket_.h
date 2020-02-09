#include <array>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>

struct Message{
    std::array<char, 1024> text;
    int tipoMensaje;
    int identificador;
    char nombreArchivo[25];
    char user[30];
  };
struct sendInfo {
  char* remote_ip;
  int remote_port;
  std::string receivedmessage;
  uint32_t ip;
  in_port_t port;
};

class Socket
{
public:
    Socket(const sockaddr_in& address);
    ~Socket();
  
    void send_to(const Message& message, const sockaddr_in& address);
    sendInfo receive_from(Message& message, sockaddr_in& address); 
private:
    int fd_;
};
