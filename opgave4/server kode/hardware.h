/*
 ============================================================================
 * server.h
 *
 *  Created on: 08. mar. 2018
 *      Author: henrik
 *	Description: handler for the LM35 temp sensor
*/

#ifndef HARDWARE_H
#define HARDWARE_H

	float calculator(void);
	void periodically_called_func(int);
	void ADC_init(void);
	void load_init(void);
	void load_direction(void);
	void load_value(int);
	void timer_init(void);
	void hardware_init(void);
    void mode(int);

#endif /* HARDWARE_H */
