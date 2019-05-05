#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <pthread.h>

class ConnectionThread
{
    private:
        int sd_cliente;
        ssize_t bytes;
        char buffer[80];
    public:
        ConnectionThread(int sd_):sd_cliente(sd_){}

        void do_connection()
        {
            // bucle para cada mensaje de la conexion
            while(true){
                struct sockaddr src_addr;
                socklen_t addrlen=sizeof(struct sockaddr);
                bytes = recv(sd_cliente, (void*)buffer, sizeof(char)*80, 0);

                if(bytes <= 0)break;
                buffer[bytes] = '\0';
                std::cout << "THREAD: " << pthread_self() << " Mensaje: " << buffer << std::endl;

                sendto(sd_cliente, buffer, bytes, 0, &src_addr, addrlen); // enviar respuesta al cliente (eco)
            }
            std::cout << "CONEXIÓN TERMINADA" << std::endl;
        }
};

extern "C" void* _do_connection(void* ct_)
{
    ConnectionThread* ct = static_cast<ConnectionThread*>(ct_);
    ct->do_connection();
    delete ct;
    return 0;
}

int main(int argc, char** argv){
    
    struct addrinfo hints;
    memset((void*)&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    struct addrinfo *res;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cout << "Error getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

	// protocolo = 0 -> el protocolo se elige en funcion del tipo de socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    bind(sd, res->ai_addr, res->ai_addrlen); // enlazado
    freeaddrinfo(res);

    listen(sd, 16); // publicar el servidor (16 = maximo de clientes (?))

	// bucle para cada conexion
    while(true){
    	char host[NI_MAXHOST];
    	char serv[NI_MAXSERV];
        struct sockaddr src_addr;
        socklen_t addrlen=sizeof(struct sockaddr);

        int sd_cliente = accept(sd, &src_addr, &addrlen);

        getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
         NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "CONEXIÓN DESDE IP: " << host << " Puerto: " << serv << std::endl;

        ConnectionThread* ct = new ConnectionThread(sd_cliente);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // cuando acabe, no se usa el thread
        pthread_t thread_id;                                           // id del thread para poder referirnos a el, param de salida
        pthread_create(&thread_id, &attr, _do_connection, ct);            // mt sera el argumento de la funcion _do_message
    }

    return 0;
}