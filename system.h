/*
 * system.h
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
#ifndef SYSTEM_H_
#define SYSTEM_H_
#include "symbols.h"
#include "pcb.h"
class KernelSem;


typedef void interrupt (*pInterrupt) (...);

class System { //in this class, we will have everything related to system operations
public:
	static void interrupt timer ();
	static void inic_system();
	static void restore_system();
	friend class PCB;
	friend class Thread;
	friend class KernelSem;
	static void exit_thread();
	volatile static PCB* running;
	struct SleepingThreads {
			PCB* mypcb; //sleeping thread's PCB
			Time timeLeft;
			SleepingThreads* next;
			SleepingThreads(PCB* pcb, Time timeToSleep){
				mypcb=pcb;
				timeLeft=timeToSleep;
				next=NULL;
			}
		};
	static void addSleepingThread(Time timeToSleep); //adds a sleeping thread into a list
	static void wakeThreadsUp();

private:

	volatile static int counter;
	volatile static boolean demanded_context_switch;
	static pInterrupt oldISR; //old interrupt routine
	volatile static PCB* mainThread; //main
	volatile static int number_of_working_threads;
	volatile static SleepingThreads* SleepingThreadsHead;
	volatile static PCB* idleThread;
};



#endif /* SYSTEM_H_ */
