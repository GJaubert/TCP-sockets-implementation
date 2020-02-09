#include "funciones.cpp"

//MAIN
 
 int protected_main(int argc, char* argv[]) {
   CommandLineArguments arguments(argc, argv);

  
//SERVER  

  if (arguments.server_mode) {
    sockaddr_in local_address{};    // Así se inicializa a 0, como se recomienda
    local_address.sin_family = AF_INET;    // Pues el socket es de dominio AF_INET
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(arguments.conn_port);

    sockaddr_in escucha{};

    Socket receptor(local_address);

    std::set<std::tuple<uint32_t, in_port_t, std::string, int>> receiving_data;

    while (!quit) {

    Message message;
    sendInfo item = receptor.receive_from(message, escucha);
    int fc,size;
    if ((fc = open(message.nombreArchivo, O_WRONLY | O_CREAT, 0644)) < 0) {
      throw std::invalid_argument("Error al abrir el fichero");
    }
    else {
      message.user[strlen(message.user)] = '\0';
      struct passwd *pwd;
      pwd = getpwnam(message.user); 
      if (pwd == NULL) {
	 std::cout << "Error usuario no encontrado\n" << message.user << " USERNAME\n";
 	 strcpy(message.user,std::getenv("USER"));
      }
      if (getuid() != 0) {
        std::cout << "\nEl servidor no esta en modo root\n";
        strcpy(message.user,std::getenv("USER"));
      } else {
	struct stat info;
        stat(filename, &info);
	struct group *gr = getgrgid(info.st_gid);
	  if (chown(message.nombreArchivo,pwd->pw_uid,pwd->pw_gid) == -1) {
            std::cout << "Error en chown\n";
          }
        }
      write(fc, message.user, strlen(message.user)); 
      write(fc, item.receivedmessage.c_str(), strlen(item.receivedmessage.c_str())-1);
      close(fc);
    }
      std::cout.flush();
     //receiving_data.insert(std::make_tuple(item.ip, item.port, message.nombreArchivo, message.identificador));
     std::cout << "El usuario " << message.user; 
     std::cout << " con ip " << item.remote_ip  << " : " << item.remote_port;
     std::cout << " envió el mensaje '" << item.receivedmessage << "'\n";
     
     }
   
//CLIENTE  

   } else if (arguments.client_mode) {
	 if (arguments.show_user == false) {
           std::cout << " Username predeterminado\n";
          arguments.user_name = std::getenv("USER");
	 }
       sockaddr_in server{};    // Así se inicializa a 0, como se recomienda
       server.sin_family = AF_INET;    // Pues el socket es de dominio AF_INET
       server.sin_port = htons(arguments.conn_port);
       inet_aton("0.0.0.0", &server.sin_addr);

       Socket emisor(Make_ip_address(arguments.ip, 7777));

       Message message;
       /*if (getuid() != 0) {
        std::cout << "\nUsted no es root\n";
        strcpy(message.user,std::getenv("USER"));
      } else*/
         strcpy(message.user,arguments.user_name);
       std::exception_ptr eptr1 {};
       std::thread* main_thread = new std::thread(ComandLine,std::ref(message),std::ref(emisor), std::ref(eptr1), server);
       {
       std::mutex mutex_;
       std::lock_guard<std::mutex> lock(mutex_);
       active_threads.push_back(std::make_pair(0,main_thread));
       }

       while (!quit);
         for(int i = 0; i < active_threads.size(); i++) {
           request_cancellation(*active_threads[i].second);
           //active_threads[0].second->join();
         }
       }
 }

 int main(int argc, char* argv[]){

   try {
     return protected_main(argc,argv);
   }
   catch(std::invalid_argument& e) {
     std::cerr << e.what() << '\n';
   }
   return 0; //Victory
 }
