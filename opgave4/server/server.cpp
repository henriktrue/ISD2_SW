/*
 ============================================================================
 * server.cpp

 *
 *  Created on: 08. mar. 2018
 *      Author: henrik
 *	Description : server
 ============================================================================
 *
 */
#include <stdio.h>
#include <cctype>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <locale>
#include <stdint.h>
#include <netdb.h>
#include <string>
#include <pthread.h>
#include <algorithm>
#include <iostream>

void * connection_handler(void *);
void * listen_thread(void *);

enum state {
    notSet, Set
};

//Stores the filedescriptors. listen on sock_fd
int sockfd;
std::string(*handle_callback)(std::string) ;

// trim from right
static inline void rcut(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from left
static inline void lcut(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

void server_init(char * interface, char *port, std::string(*handler)(std::string)) {
    int new_fd;
    handle_callback = handler;
    struct addrinfo hints, *servinfo, *p;

    //writing 0x00 to hints
    memset(&hints, 0x00, sizeof (hints));

    //can use both IPv4 or IPv6
    hints.ai_family = AF_UNSPEC;
    // streaming socket type
    hints.ai_socktype = SOCK_STREAM;
    //Use this pc's IP
    hints.ai_flags = AI_PASSIVE;

    //returns a pointer with servinfo info
    int rv;
    if ((rv = getaddrinfo(interface, port, &hints, &servinfo)) != 0) {
        //returns error code from getaddrinfo.
        syslog(LOG_ERR, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    //bind to first result possible
    for (p = servinfo; p != NULL; p = p->ai_next) {
        //socket creates endpoint for communication.
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            syslog(LOG_ERR, "Cannot bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    //If the linked list has reached the end without binding.
    if (p == NULL) {
        syslog(LOG_ERR, "Sockfd Could not associate with port");
        exit(1);
    }

    //Listen for connections on a socket.
    if (listen(sockfd, 10) == -1) {
        syslog(LOG_ERR, "Error on listen");
        exit(1);
    }

    syslog(LOG_INFO, "server: waiting for connections...");

    pthread_attr_t attr;

    pthread_attr_init(&attr);

    // Collect tid's here
    pthread_t threads;

    //&threads = unique identifier for created thread.
    int rc = pthread_create(&threads, &attr, listen_thread, NULL);

    pthread_attr_destroy(&attr);
}

void * listen_thread(void * p) {
    while (1) {
        // connector's address information
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof (their_addr);

        //intptr_t is an integer with the same size as a pointer on the system
        intptr_t new_fd = accept(sockfd, (struct sockaddr *) &their_addr,
                &sin_size);
        if (new_fd == -1) {
            syslog(LOG_ERR, "Could not accept");
            continue;
        }

        syslog(LOG_INFO, "Accepted incomming connection");

        pthread_attr_t attr;
        pthread_attr_init(&attr);

        // Collect tid's here
        pthread_t threads;

        //send file descriptor for the new connection.
        int rc = pthread_create(&threads, &attr, connection_handler,
                (void*) new_fd);
        if (rc) {
            syslog(LOG_ERR, "Couldn't create thread: %d", rc);
            exit(1);
        }

        pthread_attr_destroy(&attr);
    }
    return NULL;
}

void * connection_handler(void * new_fd) {
    enum state done = notSet;

    //Cast back fd to an integer.
    intptr_t fd = (intptr_t) new_fd;

    char buf_out[1024];
    strcpy(buf_out, "CONNECTED!\n");
    send(fd, buf_out, strlen(buf_out), 0);

    while (!done) {
        char buf_in[1024];

        memset(buf_in, 0x00, strlen(buf_in));

        int n = recv(fd, buf_in, sizeof (buf_in), 0);


        if (n < 0) {
            syslog(LOG_ERR, "Could not read from socket");
            done = Set;
            break;
        }

        if (n == 0) {
            syslog(LOG_INFO, "Peer has performed orderly shutdown");
            done = Set;
            break;
        }

        syslog(LOG_INFO, "Received %d bytes: %s", n, buf_in);

        //End of file
        if (buf_in[0] == 0x04) {
            done = Set;
            syslog(LOG_INFO, "Peer has performed orderly shutdown");
            break;
        }

        std::string input(buf_in);
        lcut(input);
        rcut(input);
        std::string output = handle_callback(input);
        if(output.length() > 1)
        {
            n = send(fd, output.c_str(), output.length(), 0);
            if (n < 0) {
                syslog(LOG_ERR, "Could not write to socket");

                done = Set;
                break;
            }
        }
    }
    close(fd);
    return NULL;
}
