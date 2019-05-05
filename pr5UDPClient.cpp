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

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);
    server_addr.sin_addr = *((struct in_addr *)argv[1]);

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cout << "Error getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }
    
    struct sockaddr src_addr;
    socklen_t addrlen=sizeof(struct sockaddr);

	// protocolo = 0 -> el protocolo se elige en funcion del tipo de socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    freeaddrinfo(res);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
         NI_NUMERICHOST | NI_NUMERICSERV);

    sendto(sd, "hola", 4, 0, (struct sockaddr *)&server_addr, addrlen);

    return 0;
}