/*
 * hardware.cpp
 *
 *  Created on: Mar 15, 2018
 *      Author: henrik
 */

#include "hardware.h"

#include<signal.h>
#include<stdio.h>
#include<pthread.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sys/time.h>
#include<syslog.h>
#include<sstream>
#include<stdlib.h>
#include<syslog.h>

using namespace std;

#define ADC "/sys/bus/iio/devices/iio:device0/in_voltage"
#define SETUP_ADC "/sys/devices/platform/bone_capemgr/"
#define INIT_LOAD "/sys/class/gpio/export"
#define LOAD_LOC "/sys/class/gpio/gpio49/"

volatile float Temperature = -273.15f;

pthread_mutex_t lock;

float calculator(void) {
	int voltage = 0;
    stringstream vo;
    //loads current voltage into voltage integer
    vo << ADC << voltage << "_raw";
    fstream fs;
    fs.open(vo.str().c_str(), fstream::in);
    if(fs.is_open()) {
        fs >> voltage;
        fs.close();
    } else {
        voltage =  0;
    }

    if(voltage > 0) {
        float cur_voltage = voltage * (1.80f / 4096.0f);
        return (100 * cur_voltage);
    } else {
        syslog(LOG_WARNING, "Could not read ADC, returning pi");
        return 3.14; //Return pi
    }
}

void periodically_called_func(int sig) {
	float Temperature;

    //try to lock.
    if (pthread_mutex_lock(&lock) == 0) float Temperature = calculator();
    else Temperature = 0;

    //the lock is for adjusting the value.
    pthread_mutex_unlock(&lock);

    syslog(LOG_INFO, "Temperature read = %.1f", Temperature);
}


void ADC_init(void)
{
    system("setting up ADC");
    //setup ADC
    ofstream adc;
    const char * path = SETUP_ADC;
    char buffer[256];

    //open slots
    sprintf (buffer, "%sslots", path);
    adc.open(buffer);

    //write "BB-ADC" to slots
    adc << "BB-ADC";
    adc.close();
}

void load_init(void)
{
    system("setting up sensor");
    //setup ADC
    ofstream se;
    se.open(INIT_LOAD);
    //write "49" to gpio/export
    se << "49";
    se.close();
}

void load_direction(void)
{
    system("configuring sensor");
    //setting direction of sensor
    ofstream cse;
    const char * path = LOAD_LOC;
    char buffer[256];

    //open slots
    sprintf (buffer, "%sdirection", path);
    cse.open(buffer);

    //write "out" to direction
    cse << "out";
    cse.close();
}

void load_value(int value)
{
    system("configuring sensor");
    //loads value of sensor 
    ofstream vse;
    const char * path = LOAD_LOC;
    char buffer[256];

    //open slots
    sprintf (buffer, "%svalue", path);
    vse.open(buffer);

    //write "value" to slots
    vse << "value";
    vse.close();
}



// initialises the timer, this timer updates itself with SIGALARM function
void timer_init(void)
{
    if (signal(SIGALRM, periodically_called_func) == SIG_ERR)
    {
        syslog(LOG_ERR, "SIGALRM function failed %d", SIGALRM);
    }

    struct itimerval timer;
    // Initialise the lock
    pthread_mutex_init(&lock, NULL);

	// Configure the timer to expire after 10 s
	timer.it_value.tv_sec = 10;
	timer.it_value.tv_usec = 0;
	// ... and every 10 s after that.
	timer.it_interval.tv_sec = 10;
	timer.it_interval.tv_usec = 0;
	// Start a virtual timer. It counts down whenever
	// this process is  executing.

    if (setitimer(ITIMER_REAL, &timer, NULL) < 0)
    {
        syslog(LOG_NOTICE, "Could not install timer\n");
    }


}


void hardware_init()
{
	//sets default sensor value to 0
	int default_value = 0;

	// setup ADC on P9 32/34
	ADC_init();

	//setup sensor at P9.23
	load_init();
	
	// setup direction to out
	load_direction();

	//config sensor
	load_value(default_value);

	//timer setup
	timer_init();
}

//turns heating on/off
void mode(int state){
    if(state == 1) {
    	   ofstream wv;
    	   const char * path = LOAD_LOC;
    	   char buffer[256];
    	   sprintf (buffer, "%svalue", path);
    	   wv.open(buffer);
    	   wv << 1;
    	   wv.close();
    }

    else
    {
    	   ofstream wv;
    	   const char * path = LOAD_LOC;
    	   char buffer[256];
    	   sprintf (buffer, "%svalue", path);
    	   wv.open(buffer);
    	   wv << 0;
    	   wv.close();
    }
}
