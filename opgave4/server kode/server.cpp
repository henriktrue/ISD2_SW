#include "server.h" 

string TCPServer::Message;

void* TCPServer::Task(void *arg) {
    int n;
    int newsockfd = (long) arg;
    char msg[MAXPACKETSIZE];
    pthread_detach(pthread_self());
    while (1) {

        // Stores recieved data in "msg"
        n = recv(newsockfd, msg, MAXPACKETSIZE, 0);

        if (n == 0) { // Has client closed the socket?
            close(newsockfd);
            break;
        }

        msg[n] = 0;
        Message = string(msg); //update Message Variable
    }
    return 0;
}

void TCPServer::setup(int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddress, 0, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    listen(sockfd, 0);
}

string TCPServer::receive() {
    string str;
    while (1) {
        socklen_t sosize = sizeof (clientAddress);
        newsockfd = accept(sockfd, (struct sockaddr*) &clientAddress, &sosize);
        str = inet_ntoa(clientAddress.sin_addr);
        pthread_create(&serverThread, NULL, &Task, (void *) newsockfd);
    }
    return str;
}

string TCPServer::getMessage() {
    return Message;
}

void TCPServer::Send(string msg) {
    send(newsockfd, msg.c_str(), msg.length(), 0);
}

void TCPServer::clean() {
    Message = "";
    memset(msg, 0, MAXPACKETSIZE);
}

void TCPServer::detach() {
    close(sockfd);
    close(newsockfd);
} 
