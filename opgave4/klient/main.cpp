/*
 * main.cpp
 *
 *  Created on: Apr 3, 2018
 *      Author: henrik
 */

#include <iostream>
#include <signal.h>
#include "TCPconnection.h"
#include "control.h"

using namespace std;

TCPClient tcp;
int target = 25; //default target temperature
float temp = 26; //default temperature reading resulting in "OFF" state

template <typename T> string tostr(const T& t) {
    ostringstream os;
    os << t;
    return os.str();
}

void sig_exit(int s) {
    tcp.exit();
    exit(0);
}

int main(int argc, char *argv[]) {

    /* Input error handling */
    if (argc == 2 && string(argv[1]).length() == 2) {
        target = toint(string(argv[1]));
    } else {
        print_help();
        exit(0);
    }
    signal(SIGINT, sig_exit);

    tcp.setup("127.0.0.1", 1955); // <ip>, <port>
    while (1) {
        tcp.Send("GETTEMP"); // Always ask the host for the temperature
        string rec = tcp.receive(); // calls recv() and does error handling

        if (rec != "") { // If rec is empty, recieve again
            cout << "Temperature is " << rec << " degC" << endl;

            /* Convert temperature string to float, making it possible to
             * compare it to the setpoint in heater() */
            temp = stof(rec);

            /* Instructs the host to turn on/off the heater by sending a
             * "1" or a "0" */
            tcp.Send(tostr(heater(temp, target)));
        }
        sleep(3); // Don't spam the output (temperature changes are slow)
    }
    return 0;
}


