/**
 ============================================================================
 *
 * main.cpp
 *
 *  Created on: 08. mar. 2018
 *  Author: henrik
 *	Description : main.cpp
 ============================================================================
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <locale>
#include <syslog.h>
#include <cstdlib>
#include <string>
#include <iostream>

#include "daemon.h"
#include "hardware.h"
#include "server.h"

#define on true
#define off false

using namespace std;

void * loop(void * m) {
    pthread_detach(pthread_self()); // Thread should not be joined
    while (1) {
        // reads "message" string, which is updated 
        string str = tcp.getMessage();
        
        // str will always be either one of these options
        if (str != "") {
            if (str == "GETTEMP") {
                
                /* Reads the ADC pin, does the temperature conversion and sends */
                tcp.Send(tostr(calculator));
            } else if (str.length() == 1) { // Message is 0 or 1
               mode(toint(str));
            }
            tcp.clean(); // zeroes the "Message" variable and the receive buffer
        }
        sleep(1);
    }
    tcp.detach();
}

int main(int argc, char* argv[]) {
    hardware_init(); // Initialize hardware
	daemon_init(); //Start the daemon

	pthread_t msg;
    tcp.setup(1955); // Socket, Bind, Listen
    
    while (1) {
        if (pthread_create(&msg, NULL, loop, (void *) 0) == 0) {
            tcp.receive(); // Starts the recieve tast that updates "Message"
        } else {
            to_syslog("Could not create thread: loop");
        }
    }
	
	return 0;
}

