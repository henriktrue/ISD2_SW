/*
 ============================================================================
 * server.h
 *
 *  Created on: 08. mar. 2018
 *      Author: henrik
 *	Description : server
 ============================================================================
 *
 */


#ifndef SERVER_H_
#define SERVER_H_

void server_init(char * interface, char *port, std::string(*handler)(std::string));
void * listen_thread(void *);
void * connection_handler(void *);

#endif /* SERVER_H_ */
