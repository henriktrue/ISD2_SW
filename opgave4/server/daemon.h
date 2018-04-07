/*
 ============================================================================
/*
 * daemon.h
 *
 *  Created on: 08. mar. 2018
 *      Author: henrik
 *	Description: starts the daemon process
*/

#ifndef DAEMON_H_
#define DAEMON_H_

#ifdef __cplusplus
extern "C" {
#endif

void daemon_init();
void terminate(int);

#ifdef __cplusplus
}
#endif
#endif /* DAEMON_H_ */
