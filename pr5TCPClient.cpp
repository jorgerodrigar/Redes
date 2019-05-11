#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){
    
    struct addrinfo hints;
    char buffer[80];
    memset((void*)&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
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

    int sd_server = connect(sd, res->ai_addr, res->ai_addrlen);

    sendto(sd, argv[3], sizeof(argv[3]), 0, res->ai_addr, res->ai_addrlen);

    ssize_t bytes = recvfrom(sd, (void*)buffer, sizeof(char)*80, 0, res->ai_addr, &res->ai_addrlen);
    buffer[bytes]='\0';

    std::cout << buffer << std::endl;

    freeaddrinfo(res);

    return 0;
}