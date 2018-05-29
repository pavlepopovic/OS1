/*
 * kernsem.cpp
 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */

#include "kernsem.h"
#include "SCHEDULE.H"


void KernelSem::block() {
	//blocking running thread
	System::running->state=PCB::BLOCKED;
	BlockedThreads.put((PCB*)System::running);
	dispatch();
}
void KernelSem::deblock() {
	PCB* readyThread = BlockedThreads.get();
	if (readyThread != NULL) { //get () can return NULL
		readyThread->state=PCB::READY;
		Scheduler::put(readyThread);
	}
}
int KernelSem::wait(int toBlock) {
	// returns 0 if thread ISN'T blocked
	// returns -1 if thread IS blocked
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	int ret=0;
	if (!toBlock && val <=0) ret = -1;
	else {
		if (--val < 0) {
			ret=1;
			block();
		}
	}
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif
	return ret;
}
void KernelSem::signal() {
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	if (val++ < 0) deblock();
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif
}

KernelSem::~KernelSem() {
	PCB* cur = BlockedThreads.get();
	while (cur != NULL) {
		cur->state=PCB::READY;
		Scheduler::put(cur); //deblocking all threads
	}
}




