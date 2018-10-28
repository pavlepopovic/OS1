/*
 * PCB.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: OS1
 */


#include "PCB.h"
#include "symbols.h"
#include <dos.h>
#include "SCHEDULE.H"
#include "thread.h"
#include "System.h"

volatile PCB::PCBlist *PCB::head=NULL, *PCB::tail=NULL;
volatile int PCB::lastID=0;

void PCB::addToPCBlist() {

	PCBlist* node = new PCBlist (this);

	if (PCB::head == NULL) {
		PCB::head=node;
		PCB::tail=node;
	} else {
		PCB::tail->next=node;
		PCB::tail=node;
	}
}

PCB* PCB::getPCBbyID(ID id) {
	for (PCBlist* cur = (PCBlist*)PCB::head; cur != NULL; cur=cur->next) {
		if(cur->elem->myID == id) return cur->elem;
	}
	return NULL;
}

void PCB::removeFromPCBlist() {
	PCBlist *cur= (PCBlist*) head, *prev=NULL;
	while (cur->elem != this) {
		prev=cur;
		cur=cur->next;
	}

	if (prev == NULL) {
		//deleting at head postion
		head=head->next;
		if (head == NULL) tail=NULL;
		cur->elem=NULL;
		cur->next=NULL;
		delete cur;
	}
	else if (cur->next == NULL) {
		//deleting last element
		tail=prev;
		tail->next=NULL;
		cur->elem=NULL;
		delete cur;
	}
	else {
		//deleting middle element
		prev->next=cur->next;
		cur->next=NULL;
		cur->elem = NULL;
		delete cur;
	}
}





void PCB::wrapper() {
	System::runningUserThread->myThread->run();
	System::exit_thread();

}



PCB::PCB(StackSize stacksize, Time timeSlice, Thread *thr) {
	this->state=NEW;
	if (stacksize > maxStackSize) stacksize = maxStackSize;
	size=stacksize/sizeof(unsigned);
	myThread=thr;
	stack = new unsigned [size];
	quantum=timeSlice;
	myID=++lastID;
	addToPCBlist();
	thr->pcbID=myID;

	this->WaitingThreadsHead=NULL;
	this->WaitingThreadsTail=NULL;
}


/*
PCB::PCB(StackSize stacksize, Time timeSlice, void (*f)()) {
	fun=f;
	size=stacksize;
	quantum=timeSlice;
	stack = new unsigned [size];
	myID=++lastID;
	addToPCBlist();
	this->state=NEW;
}

*/

PCB::~PCB() {
	removeFromPCBlist();
	delete [] stack;
	stack = NULL;
	this->myThread=NULL;
	stack = NULL;
}

void PCB::createContext() {
	if (this->state == PCB::NEW) {
		this->state = PCB::READY;
#ifndef BCC_BLOCK_IGNORE
		this->stack[this->size-1]=FP_SEG(this->myThread);
		this->stack[this->size-2]=FP_OFF(this->myThread);
		//skipping -3 -4 (callback)
		this->stack[this->size-5]=0x200; //I bit = 1
		this->stack[this->size-6]=FP_SEG(Thread::threadWrapper);
		this->stack[this->size-7]=FP_OFF(Thread::threadWrapper);
		this->s_s=FP_SEG(this->stack + (this->size-16 ));
		this->s_p=FP_OFF(this->stack + (this->size-16));
		this->b_p=FP_OFF(this->stack + (this->size-16));

		if (this != System::idleThread) {
			Scheduler::put(this);
			System::number_of_user_threads++;
		}
#endif
	}
}




void PCB::pcb_waitToComplete(){
	if (this->state == PCB::FINISHED) {
		return;
	}
	PCB::WaitingThreads* nw = new PCB::WaitingThreads((PCB*)System::runningUserThread);

	if (this->WaitingThreadsHead == NULL) {
		this->WaitingThreadsHead=this->WaitingThreadsTail=nw;
	} else {
		this->WaitingThreadsTail->next=nw;
		this->WaitingThreadsTail=nw;
	}

	System::runningUserThread->state=PCB::BLOCKED;

	System::demanded_context_switch=TRUE; // context switch requested

	// dispatch () ;
}

void PCB::deblockAllThreads(){
	System::softLock();
	if (this->WaitingThreadsHead != NULL) {
		//there are some blocked threads
		PCB::WaitingThreads* cur = this->WaitingThreadsHead;

		while (cur != NULL) {
			//deblocking threads
			WaitingThreads* old = cur;
			cur->mypcb->state = PCB::READY;
			Scheduler::put(cur->mypcb);
			cur->mypcb = NULL;
			cur=cur->next;

			//mozda syscall pa ne treba okruziti sa lock/unlock
			delete old;

		}

		this->WaitingThreadsHead = this->WaitingThreadsTail = NULL;
	}
	System::softUnlock();

}
