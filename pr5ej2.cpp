#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){
    
    struct addrinfo hints;
    memset((void*)&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    struct addrinfo *res;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cout << "Error getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    bind(sd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    bool run = true;
    while(run){
        ssize_t bytes;
        char buffer[80];
        struct sockaddr src_addr;
        socklen_t addrlen=sizeof(struct sockaddr);

        bytes=recvfrom(sd, (void*)buffer, sizeof(char)*80, 0, &src_addr, &addrlen);
        buffer[bytes]='\0';
        getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
         NI_NUMERICHOST | NI_NUMERICSERV);
        
        time_t aux = time(0);
        struct tm* tiempo;
        char bufferTiempo[9];

        switch(buffer[0]){
            case 't':
            tiempo = localtime(&aux);
            strftime(bufferTiempo, sizeof(bufferTiempo), "%T", tiempo);
            sendto(sd, bufferTiempo, sizeof(bufferTiempo), 0, &src_addr, addrlen);
            break;

            case 'd':
            tiempo = localtime(&aux);
            strftime(bufferTiempo, sizeof(bufferTiempo), "%D", tiempo);
            sendto(sd, bufferTiempo, sizeof(bufferTiempo), 0, &src_addr, addrlen);
            break;

            case 'q':
            run=false;
            break;
        }

        std::cout << "Mensaje: " << buffer << " IP: " << host << " Puerto: " << serv << std::endl;
    }

    return 0;
}