#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    if(nick.size() < 8)
    	nick[nick.size()+1]='\0';
    memcpy(tmp, nick.c_str(), sizeof(char) * 8);
    tmp += sizeof(char) * 8;
    if(message.size() < 80)
    	message[message.size()+1]='\0';
    memcpy(tmp, message.c_str(),  sizeof(char) * 80);
    tmp += sizeof(char) * 80;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char * tmp = _data;

    //Reconstruir la clase usando el buffer _data
    memcpy(&type, tmp, sizeof(uint8_t));
    tmp+=sizeof(uint8_t);
    nick.resize(sizeof(char) * 8);
    memcpy(&nick[0], tmp, sizeof(char) * 8);
    tmp+=sizeof(char) * 8;
    message.resize(sizeof(char) * 80);
    memcpy(&message[0], tmp,  sizeof(char) * 80);
    tmp+=sizeof(char) * 80;

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

		// procesamiento del mensaje
        Socket* cliente;
        ChatMessage msg;
        socket.recv(msg, cliente);

        switch(msg.type){
            case ChatMessage::LOGIN:
            	clients.push_back(cliente);
            	std::cout << "El cliente " << msg.nick.c_str() << " se ha conectado" << std::endl;
            break;

            case ChatMessage::MESSAGE:
            	for(int i = 0; i < clients.size(); i++){
            		if(!(*clients[i] == *cliente))
            			socket.send(msg, *clients[i]);
            	}
            	std::cout << "El cliente " << msg.nick.c_str() << " ha enviado un mensaje" << std::endl;
            	//delete cliente;
            break;

            case ChatMessage::LOGOUT:
            	auto it = clients.begin();
            	while(!(**it==*cliente))it++;
            	if(it != clients.end()){
            		std::cout << "El cliente " << msg.nick.c_str() << " se ha desconectado" << std::endl;
            		Socket* aux = *it;
            		clients.erase(it);
            		delete aux;
            	}
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
        std::string msg;
        std::getline(std::cin, msg);

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
        socket.recv(em);

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        if(em.nick != nick)std::cout << em.nick.c_str() << ": " << em.message.c_str() << std::endl;
    }
}

