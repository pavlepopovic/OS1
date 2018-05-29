/*
 * pcb.h
 *
 *  Created on: May 9, 2018
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "symbols.h"
#include "thread.h"
class KernelSem;


class System;
class PCB {
public:
	friend class System;
	friend class Thread;
	friend class KernelSem;
	friend class KernelEv;
	friend class IVTEntry;
	enum State {NEW, READY, BLOCKED, FINISHED };
	//PCB(StackSize stacksize,Time timeSlice, Thread* mythread);
	~PCB();
	static void PCB_dispatch ();
	void create_context ();
	struct WaitingThreads {
		//threads that wait for this one to finish
		PCB *waitingThread;
		WaitingThreads *next;
		WaitingThreads (PCB *thrd): waitingThread(thrd), next (NULL) {};
	};

private:
    // need to implement sleep (TIme to sleep) and wait (time to Wait);
	State state ;
	unsigned *stack; // this thread's stack
	unsigned sp,ss; //stack pointer and stack segment
	unsigned bp; //base pointer
	Time quantum; //this thread's timeSLice
	StackSize size;
	Thread *myThread; //it may break here
	PCB(StackSize stacksize,Time timeSlice, Thread* mythread); //can only be called in thread constructor
	WaitingThreads *WaitingThreadsHead, *WaitingThreadsTail; //head and tail of a list
	static void wrapper (); //called only in non-main threads
	void deblockAllThreads();
};



#endif /* PCB_H_ */
