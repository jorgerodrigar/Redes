#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

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
        ssize_t bytes;   // aqui guardaremos lo que ocupa el mensaje recibido
        char buffer[80]; // aqui guardaremos el mensaje recibido
    	char host[NI_MAXHOST];
    	char serv[NI_MAXSERV];
        struct sockaddr src_addr;
        socklen_t addrlen=sizeof(struct sockaddr);

        int sd_cliente = accept(sd, &src_addr, &addrlen);

        getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
         NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "CONEXIÓN DESDE IP: " << host << " Puerto: " << serv << std::endl;

        // bucle para cada mensaje de la conexion
        while(true){
        	bytes = recv(sd_cliente, (void*)buffer, sizeof(char)*80, 0);
        	if(buffer[0] == 'q' && bytes <= 1)break; // si se escribe q, se cierra
        	buffer[bytes] = '\0';
        	//std::cout << "Mensaje: " << buffer;
        	sendto(sd_cliente, buffer, bytes, 0, &src_addr, addrlen); // enviar respuesta al cliente (eco)
        }
        std::cout << "CONEXIÓN TERMINADA" << std::endl;
    }

    return 0;
}