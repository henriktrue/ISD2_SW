/* 
 * File:   controller.h
 * Author: Henrik
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <fstream>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

void print_help(void);
int toint(std::string s);
int heater(float temperature, int target);

#endif /* CONTROL_H */