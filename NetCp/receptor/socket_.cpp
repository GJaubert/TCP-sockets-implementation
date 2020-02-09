#include "socket_.h"

Socket::Socket(const sockaddr_in& address){
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ < 0) {
    std::cerr << "no se pudo crear el socket: " <<    
    std::strerror(errno) << '\n';
  }

  int result = bind(fd_, reinterpret_cast<const sockaddr*>(&address),
              sizeof(address));
  if (result < 0) {
    std::cerr << "falló bind: " << std::strerror(errno)  << '\n';
  }

}

Socket::~Socket(){
  close(fd_);
}

void Socket::send_to(const Message& message, const sockaddr_in& address){
  
  int result = sendto(fd_, &message, sizeof(message), 0, 
		reinterpret_cast<const sockaddr*>(&address),sizeof(address));
  if (result < 0) {
    std::cerr << "falló sendto: " << std::strerror(errno) << '\n';
  }
}

sendInfo Socket::receive_from(Message& message, sockaddr_in& address){
  socklen_t src_len = sizeof(address);
  int result = recvfrom(fd_, &message, sizeof(message)-1, 0,
		         reinterpret_cast<sockaddr*>(&address), 
 			  &src_len);
  if (result < 0) {
    std::cerr << "falló receivefrom: " << std::strerror(errno) << '\n';
  }
  
  sendInfo tmp;
  tmp.remote_ip = inet_ntoa(address.sin_addr);
  tmp.remote_port = ntohs(address.sin_port);
  message.text[1023] = '\0';
  tmp.receivedmessage = message.text.data();
  tmp.ip = address.sin_addr.s_addr;
  tmp.port = address.sin_port;
  return tmp;
}
