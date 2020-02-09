#include "socket_.h"
#include <thread>
#include <vector>
#include <utility>
#include <pthread.h>
#include <atomic>
#include <set>
#include <tuple>
#include <functional>
#include <pwd.h>
#include <cstdlib>
#include <grp.h>
#include <unistd.h>
#include <getopt.h>
#include <mutex>

 const char* filename;
 std::vector<std::pair<int, std::thread*>> active_threads;
 std::atomic<bool> quit(false);

 struct CommandLineArguments {
   bool show_help = false;
   bool server_mode = false;
   bool client_mode = false;
   unsigned short conn_port = 0;
   bool show_user = false;
   char* user_name;
   std::string ip;
   std::vector<std::string> other_arguments;

   CommandLineArguments(int argc, char* argv[]);
 };

CommandLineArguments::CommandLineArguments(int argc, char* argv[]){
  int c,d;

  static struct option long_options[] =
  {
    {"server", no_argument, 0, 'a'},
    {"client", required_argument,  0, 'b'},
    {"username", required_argument, 0, 'f'}

  };
  int option_index = 0;

  while (d = getopt_long (argc, argv, "ab:c:hsc:p:01u:", long_options, &option_index)){
    switch (d) {
      case 'a':
         std::cerr << "opcion server\n";
	 server_mode = true;
         break;
      case 'b':
         std::cerr << "opcion cliente con ip \n";
         client_mode = true;
         ip = optarg;
         break;
      case 'f':
         std::cerr << "opcion username \n";
         show_user = true;
         user_name = optarg;
      case '0':
      case '1':
             std::cerr << "opcion" << c << std::endl;
             break;
      case 'h':
             std::cerr << "opcion h\n";
             show_help = true;
             break;
      case 's':
             std::cerr << "opcion s\n";
             server_mode = true;
             break;
      case 'c':
             std::cerr << "opcion c con ip " << optarg << std::endl;
             client_mode = true;
             ip = optarg;
             break;
      case 'p':
             std::cerr << "opcion p con valor " << optarg << std::endl;
             conn_port = std::atoi(optarg);
             break;
      case 'u':
             std::cerr << "opcion u\n";
             show_user = true;
             user_name = optarg;
             break;
       case '?':
             throw std::invalid_argument("Argumento de línea de comandos desconocido");
       default:
             return;
             break;
             // throw std::runtime_error("Error procesando la línea de comandos");
 }
  }

  if (optind < argc) {
      std::cerr << "-- empezamos con los argumentos no opción --\n";
      for (; optind < argc; ++optind) {
        std::cerr << "argv[" << optind << "]: " << argv[optind] << '\n';
        other_arguments.push_back(argv[optind]);
      }
    }

}

 sockaddr_in Make_ip_address(const std::string& ip_address, int port){
   sockaddr_in address{};
   address.sin_family = AF_INET;
   address.sin_port = htons(port);
   inet_aton(ip_address.c_str(), &address.sin_addr);
   return address;
 }


 int GetId() {
   int fs = open(filename, O_RDONLY);
   return fs;
 }

 void request_cancellation(std::thread& thread) {
   pthread_cancel(thread.native_handle());
 }


 void ReadandSendFile(Message message, Socket &my_socket, int &descriptor, sockaddr_in direccion){
   int pos = active_threads.size();
   int fs,ret;
   std::mutex mutex_;

     if ((fs = open(filename, O_RDONLY)) < 0) {
       throw std::invalid_argument("Error al abrir el fichero");
     }
     else {
       do {
         while ((ret = read(fs, message.text.data(), sizeof(message.text))) < 0) {
           std::cout << fs;
         }

         message.text[ret] = 0x00;
           if (message.text[1023] == ' ') {
           if (message.tipoMensaje != 2)
             message.tipoMensaje = 4; //unico
           else
             message.tipoMensaje = 3; //final
           } else {
             if (message.tipoMensaje == 1)
             message.tipoMensaje = 2; //medio
             else 
               message.tipoMensaje = 1;
          }
         message.identificador = fs;
         close(fs);
       strcpy(message.nombreArchivo,filename);

       my_socket.send_to(message, direccion);
       } while ((message.tipoMensaje == 3)||(message.tipoMensaje == 4));
       descriptor = fs;
       std::lock_guard<std::mutex> lock(mutex_);
       active_threads.erase(active_threads.begin()+pos);
    }
 }

 void ComandLine(Message &message, Socket &my_socket, std::exception_ptr& eptr, sockaddr_in direccion){
   std::string input;
   while(input.compare("quit")) {
     std::getline(std::cin,input);
     std::string substring = input.substr(0,4);
     if (substring.compare("send") == 0) {
       std::string tmp = input.substr(5);
       filename = tmp.c_str();

       int id = 0;
       std::thread* temporal_thread = new std::thread(ReadandSendFile,message,std::ref(my_socket),std::ref(id), direccion);
       active_threads.push_back(std::make_pair(id,temporal_thread));
       temporal_thread->join();

     }
     substring = input.substr(0,5);
   }
   quit = true;
 }

