#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    memcpy(_data, type, sizeof(uint8_t));
    memcpy(_data, nick, sizeof(char) * 80);
    memcpy(_data, message,  sizeof(char) * 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    memcpy(type, _data, sizeof(uint8_t));
    memcpy(nick, _data, sizeof(char) * 80);
    memcpy(message, _data,  sizeof(char) * 80);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        ssize_t bytes;   // aqui guardaremos lo que ocupa el mensaje recibido
        char buffer[ChatMessage::MESSAGE_SIZE]; // aqui guardaremos el mensaje recibido
        struct sockaddr addrcliente;
        socklen_t clientelen=sizeof(struct sockaddr);

		// procesamiento del mensaje
        bytes=recvfrom(socket, (void*)buffer, ChatMessage::MESSAGE_SIZE, 0, &cliente, &clientelen);
        buffer[bytes]='\0'; // le decimos que es final de cadena (llega hasta ahi)

        Socket cliente = Socket(addrcliente, clientelen);
        
        uint8_t msgType;
        memcpy(&msgType, &buffer, sizeof(uint8_t));

        switch(msgType){
            case ChatMessage::LOGIN:
            	clientes.push_back(&cliente); // REPASAR ESTO
            break;

            case ChatMessage::MESSAGE:
            	for(int i = 0; i< clientes.size(); i++){
            		if(!(clientes[i] == cliente))
            			sendto(sd, buffer, hatMessage::MESSAGE_SIZE, 0, &clientes[i].sa, clientes[i].sa_len);
            	}
            break;

            case ChatMessage::LOGOUT:
            	auto it = clientes.begin();
            	while(it!=cliente)it++;
            	if(it != clientes.end())clientes.erase(it);
            break;
        }
    }
}

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
	std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        char* msg = std::getline();

        // Enviar al servidor usando socket
        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        ChatMessage em;
        socket.recv(em, &socket);

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        if(em.nick != nick)std::cout << em.nick << ": " << em.message << std::endl;
    }
}

