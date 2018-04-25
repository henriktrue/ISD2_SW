/* 
 * File:   main.cpp
 * Author: Henrik
 */


#include <iostream>
#include <signal.h>
#include "connect.h"
#include "control.h"

using namespace std;

connect tcp;
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

void print_help(void) {
    cout << "you need to supply an input argument temperature (2 digits)" << endl;
    cout << endl;
}

int heater(float temperature, int target) {
    if (temperature < target) {
        cout << "Heater ON" << endl;
        return 1;
    } else {
        cout << "Heater OFF" << endl;
        return 0;
    }
}

int toint(std::string s) //The conversion function
{
    return atoi(s.c_str());
}

int main(int argc, char *argv[]) {
	// in case no input argument was not valid
    if (argc == 2 && string(argv[1]).length() == 2) {
        target = toint(string(argv[1]));
    } else {
        print_error();
        exit(0);
    }
    signal(SIGINT, sig_exit);

	// ip is 0.0.0.0 to listen on all interfaces
    tcp.setup("0.0.0.0", 1955); // ip and port
    while (1) {
        tcp.Send("GETTEMP"); // ask host for temperature 
        string rec = tcp.receive(); // calls recv() and does error handling
        
        if (rec != "") { // If rec is empty, recieve again
            cout << "Temperature is: " << rec << " degC" << endl;
            
            // Convert temperature string to float, so i can compared to the setpoint in heater()
            temp = stof(rec);
            
            // Instructs the host to turn heater on or off
            tcp.Send(tostr(heater(temp, target)));
        }
        sleep(3); // wait 3 seconds
    }
    return 0;
}