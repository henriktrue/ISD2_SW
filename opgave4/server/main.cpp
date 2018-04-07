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

string message(string input) {
    string output("");

    if (input == string("GET TEMP")) {
        float f = calculator();
        output = "TEMP IS: " + to_string(f) + "\n";
    }
    else if (input == string("1"))
    {
        syslog(LOG_INFO, "Turning heat on");
        mode(1);
    }
    else if (input == string("0"))
    {
        syslog(LOG_INFO, "Turning heat off");
        mode(2);
    }
    else
    {
        syslog(LOG_INFO, "Unknown command");
        output = "command is not recognised \n";
    }

    return output;
}

int main(int argc, char* argv[]) {
    hardware_init(); // Initialize hardware
	daemon_init(); //Start the daemon
    server_init("0.0.0.0", "1955", message); //Starts the server

    while (1) {
        sleep(1);
    }
}

