/*
 * kernelev.cpp
 *
 *  Created on: May 19, 2018
 *      Author: OS1
 */

#include "kernelev.h"
#include "system.h"
#include "symbols.h"
#include "SCHEDULE.H"

KernelEv::KernelEv() {
	creator=(PCB*)System::running;
	val=0;
	blocked_creator=NULL;
}
KernelEv::~KernelEv() {
	if (blocked_creator != NULL) {
		creator->state=PCB::READY;
		Scheduler::put(creator);
	}
	creator=NULL;
	blocked_creator=NULL;
}
void KernelEv::signal() {
	//will be called ONLY IN AN INTERRUPT ROUTINE - NO NEED FOR LOCK AND UNLOCK
	if (blocked_creator == NULL) val = 1;
	else {
		creator->state=PCB::READY;
		blocked_creator=NULL;
		Scheduler::put(creator);
		dispatch(); //unblocking creator thread
	}
}
void KernelEv::wait() {
	if ((PCB*)System::running != creator) return; //only creator thread can access wait();
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	if (val == 1) val = 0;
	else {
		//val == 0
		//block the thread
		creator->state=PCB::BLOCKED; //block it
		blocked_creator=creator;
		dispatch(); //gets another thread
	}
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif
}
