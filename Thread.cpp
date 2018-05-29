#include <iostream.h>
#include "thread.h"
#include "pcb.h"
#include "system.h"
#include "symbols.h"

/* from symbols.h
 *
 */
Thread::Thread(StackSize size, Time timeSlice) {
	if (size <= maxStackSize)mypcb = new PCB (size,timeSlice,this);
	else mypcb= new PCB (maxStackSize,timeSlice,this);
}
Thread::~Thread() {
	waitToComplete();
	delete mypcb;
	mypcb=NULL;
}
void Thread::waitToComplete () { //modify this !
#ifndef BCC_BLOCK_IGNORE
	lock
	if (this -> mypcb == NULL || this->mypcb->state == PCB::FINISHED /*|| this->mypcb->state == PCB::NEW*/) {
		//completed thread
		unlock
		return;
	}
	PCB::WaitingThreads *nw = new PCB::WaitingThreads ((PCB*)System::running); //running  thread becomes blocked
	if (this->mypcb->WaitingThreadsHead == NULL) {
		this->mypcb->WaitingThreadsHead=this->mypcb->WaitingThreadsTail=nw;
	}
	else {
		this->mypcb->WaitingThreadsTail->next=nw;
		this->mypcb->WaitingThreadsTail=nw;
	}
	System::running->state=PCB::BLOCKED;
	unlock
#endif
	//razmotriti lockove i unlockove
	dispatch () ; //must take care for blocked thread !
}
void Thread::start () {
	if (this->mypcb != NULL && this->mypcb->state == PCB::NEW) {
#ifndef BCC_BLOCK_IGNORE
		lock
		this->mypcb->create_context();
		unlock
#endif
	}

}
void dispatch () {
	System::running->PCB_dispatch();
}

void Thread::sleep(Time timeToSleep) {
	System::addSleepingThread(timeToSleep+1);
}

