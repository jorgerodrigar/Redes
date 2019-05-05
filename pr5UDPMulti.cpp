#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <pthread.h>

static bool fin = true;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

class MessageThread
{
private:
	ssize_t bytes;   // msg size
	int sd;          // socket
	char buffer[80]; // msg
public:
	MessageThread(int sd_):sd(sd_){}

	void do_message() // funcion que trata cada mensaje recibido
	{
		int i = 0;

    	while(fin){
        	struct sockaddr src_addr;
        	socklen_t addrlen=sizeof(struct sockaddr);

			// procesamiento del mensaje
        	bytes=recvfrom(sd, (void*)buffer, sizeof(char)*80, 0, &src_addr, &addrlen);
        	buffer[bytes]='\0'; // le decimos que es final de cadena (llega hasta ahi)

        	// info del que manda el mensaje
        	char host[NI_MAXHOST];
    		char serv[NI_MAXSERV];
        	getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
         	NI_NUMERICHOST | NI_NUMERICSERV);

        	// escribimos la info del que manda el mensaje
         	std::cout << "IP: " << host << " PUERTO: " << serv << " THREAD: " << pthread_self()
         	 << " Mensaje: " << buffer << std::endl;
        
         	// echo
            sendto(sd, buffer, bytes, 0, &src_addr, addrlen);

        	i++; if(i == 5) fin=true;
        	if(fin)pthread_cond_signal(&cond); // protegemos la variable fin
    	}	
	}
};

// para que se compile en C
extern "C" void* _do_message(void* mt_)
{
	MessageThread* mt = static_cast<MessageThread*>(mt_);
	mt->do_message();
	delete mt;
	return 0;
}

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

	// protocolo = 0 -> el protocolo se elige en funcion del tipo de socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    bind(sd, res->ai_addr, res->ai_addrlen); // enlazado
    freeaddrinfo(res);

    // crear threads
    for(int i = 0; i < 5; i++){
    	MessageThread* mt = new MessageThread(sd);
    	pthread_attr_t attr;
    	pthread_attr_init(&attr);
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // cuando acabe, no se usa el thread
    	pthread_t thread_id;                                           // id del thread para poder referirnos a el, param de salida
    	pthread_create(&thread_id, &attr, _do_message, mt);            // mt sera el argumento de la funcion _do_message
	}

    pthread_mutex_lock(&mtx);
    while(!fin)pthread_cond_wait(&cond, &mtx);
    pthread_mutex_lock(&mtx);

    return 0;
}