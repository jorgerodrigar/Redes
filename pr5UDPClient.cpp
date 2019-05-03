#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){
    
    struct addrinfo hints;
    memset((void*)&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    struct addrinfo *res;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cout << "Error getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }
    
    struct sockaddr src_addr;
    socklen_t addrlen=sizeof(struct sockaddr);

	// protocolo = 0 -> el protocolo se elige en funcion del tipo de socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    bind(sd, res->ai_addr, res->ai_addrlen); // enlazado
    freeaddrinfo(res);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    sendto(sd, argv[3], 1, 0, &src_addr, addrlen);

    return 0;
}