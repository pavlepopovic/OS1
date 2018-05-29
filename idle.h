/*
 * idle.h
 *
 *  Created on: May 15, 2018
 *      Author: OS1
 */
#include "thread.h"
#include "symbols.h"
#ifndef IDLE_H_
#define IDLE_H_
class IdleThread : public Thread {
public:
	virtual void run ();
	friend class System;
	friend class PCB;
private:
	IdleThread (StackSize size, Time timeSlice):Thread(size,timeSlice) {};
};




#endif /* IDLE_H_ */
