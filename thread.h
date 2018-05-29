/*
 * thread.h
 *
 *  Created on: May 9, 2018
 *      Author: OS1
 */

#ifndef THREAD_H_
#define THREAD_H_

//user types
typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, multiples of 55ms
const Time defaultTimeSlice = 2; // default = 2 * 55ms
typedef int ID;
const StackSize maxStackSize=32768; //64 kB max stack size


class PCB; //declaration
class System;


class Thread {
public:
	void start ();
	void waitToComplete();
	virtual ~Thread ();
	static void sleep (Time timeToSleep);
protected:
	friend class PCB;
	friend class System;
	friend class KernelEv;
	Thread (StackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run () {} ;

private:
	PCB *mypcb;
};
void dispatch();

#endif /* THREAD_H_ */
