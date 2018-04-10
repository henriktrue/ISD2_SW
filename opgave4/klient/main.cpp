/*
 * main.cpp
 *
 *  Created on: Apr 3, 2018
 *      Author: henrik
 */

 #include <iostream>
 #include <signal.h>
 #include "connect.h"
#include "control.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <fstream>
 
 using namespace std;
 
connect tcp;
 int target = 25; //default target temperature
 float temp = 26; //default temperature reading resulting in "OFF" state
 
@@ -51,27 +60,29 @@ int main(int argc, char *argv[]) {
     if (argc == 2 && string(argv[1]).length() == 2) {
         target = toint(string(argv[1]));
     } else {
        print_help();
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
            float temp = strtof((rec).c_str(),0);

             // Instructs the host to turn heater on or off
             tcp.Send(tostr(heater(temp, target)));
         }
         sleep(3); // wait 3 seconds
     }
     return 0;
}

