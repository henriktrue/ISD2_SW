/*
 ============================================================================
 * daemon.cpp
 *
 *  Created on: 08. mar. 2018
 *      Author: henrik
 *	Description: starts the daemon process
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include "daemon.h"


void daemon_init() {
	pid_t process_id = 0;
	pid_t sid = 0;

	// Create child process
	process_id = fork();

	// Indication of fork() failure
	if (process_id < 0) {
		syslog(LOG_INFO, "daemon failed fork function!");
		// Return failure in exit status
		exit(1);
	}

	// PARENT PROCESS. Need to kill it.
	if (process_id > 0) {
		syslog(LOG_INFO, "process_id of child process %d", process_id);
		printf("Daemon started: PID %d\n", process_id);
		exit(0);
	}

	//unmask the file mode
	umask(0);
	//set new session
	sid = setsid();
	if (sid < 0) {
        syslog(LOG_ERR,"Can't run a program in a new session\n");
		// Return failure
        syslog(LOG_INFO, "Stopping daemon");
        closelog();
        exit(1);
	}

	// Change the current working directory to root.
	chdir("/");

	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	syslog(LOG_INFO, "Daemon is alive!!.");

	if (signal(SIGHUP, terminate) == SIG_ERR) {
		syslog(LOG_ERR,"Error catching %d",SIGHUP);
	}
}

//terminate the program
void terminate(int kill_sig) {
	if (kill_sig == SIGHUP) {
            syslog(LOG_INFO, "program has been terminated");
        	syslog(LOG_INFO, "Stopping daemon");
        	closelog();
            exit(1);
	}
}
