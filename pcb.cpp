/*
 * pcb.cpp
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
#include "symbols.h"
#include "pcb.h"
#include "system.h"
#include "dos.h"
#include "SCHEDULE.H"
#include <iostream.h>
PCB::PCB(StackSize stacksize, Time timeSlice, Thread* mythread) {
	//creating context for a new thread
	this->stack = new unsigned int [stacksize] ; // stack
	this->quantum=timeSlice;
	this->state = NEW;
	this->size=stacksize;
	this->myThread=mythread;
	//System::number_of_threads++; //incrementing number of threads
	this->WaitingThreadsHead=NULL;
	this->WaitingThreadsTail=NULL;
}
void PCB::deblockAllThreads() {
	if (WaitingThreadsHead != NULL) {
		//there are some blocked threads
		WaitingThreads *cur =WaitingThreadsHead;
		while (cur != NULL) {
			//deblocking those threads
			WaitingThreads *old=cur;
			cur->waitingThread->state=PCB::READY;
			//scheduler lock ?
			Scheduler::put(cur->waitingThread);
			//scheduler unlock ?
			cur->waitingThread=NULL;
			cur=cur->next;
			//lokc?
			delete old;
			//unlock ?
			old = NULL;
		}
		WaitingThreadsTail=NULL;
	}
}
PCB::~PCB () {
	this->myThread->mypcb=NULL;
	this->myThread=NULL;
	delete [] stack; //for now, will be modified later
	stack = NULL;
}
void PCB::PCB_dispatch () {
#ifndef BCC_BLOCK_IGNORE
	lock
	System::demanded_context_switch=TRUE;
	System::timer();
	unlock
#endif
}
//!!!!change this!!!!!
void PCB::create_context() { //creating a thread at first
#ifndef BCC_BLOCK_IGNORE

	this->stack[this->size-1]=0x200; // setting of PSW I bit
	this->state=READY;
	this->stack[size-2]=FP_SEG(PCB::wrapper);
	this->stack[size-3]=FP_OFF(PCB::wrapper); //return address
	this->sp=FP_OFF(stack+(size-12));
	this->ss=FP_SEG(stack+(size-12));
	this->bp=FP_OFF(stack+(size-12));
	//thread is ready, put it in Scheduler;
	//temporarily version! CHANGE IT ASAP;
	if (this != System::idleThread) { //we never put idleThread in scheduler
	System::number_of_working_threads++;
	Scheduler::put(this); //need the cast - nope
	}
#endif
}
void PCB::wrapper () {
	System::running->myThread->run(); // run method
	System::exit_thread();
}
