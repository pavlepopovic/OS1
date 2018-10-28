/*
 * kernsem.cpp
 *
 *  Created on: Aug 23, 2018
 *      Author: OS1
 */


#include "kernsem.h"
#include "symbols.h"
#include "semaphor.h"
#include "thread.h"
#include "PCB.h"
#include "SCHEDULE.H"

volatile KernelSem::KernelSemList* KernelSem::head=NULL;
volatile KernelSem::KernelSemList* KernelSem::tail=NULL;
volatile ID KernelSem::semLastID=0;

void KernelSem::addToKernelSemList() {
	//adding THIS object to global KernelSemList
	KernelSem::KernelSemList *newElem = new KernelSem::KernelSemList(this);

	if (KernelSem::head == NULL) {
		KernelSem::head=newElem;
		KernelSem::tail=newElem;
	} else {
		KernelSem::tail->next=newElem;
		KernelSem::tail=newElem;
	}
}

KernelSem* KernelSem::getKernelSembyID(ID id) {
	for (KernelSemList* cur = (KernelSemList*)KernelSem::head; cur != NULL; cur=cur->next) {
		if (cur->elem->myID == id) return cur->elem;
	}
	return NULL;
}

void KernelSem::removeFromKernelSemList() {
	KernelSemList *cur=(KernelSemList*)head, *prev=NULL;
	while (cur->elem != this) {
		prev=cur;
		cur=cur->next;
	}
	if (prev == NULL) {
		//deleting at head position
		head=head->next;
		if (head == NULL) tail = NULL;
		cur->elem=NULL;
		cur->next=NULL;
		delete cur;
	} else if (cur->next == NULL) {
		//deleting from the end of the list
		prev->next=NULL;
		tail=prev;
		cur->elem=NULL;
		cur->next=NULL;
		delete cur;
	} else {
		prev->next=cur->next;
		cur->next=NULL;
		cur->elem=NULL;
		delete cur;
	}
}

KernelSem::KernelSem(int init, Semaphore* mySem) {
	this->myID=++KernelSem::semLastID;
	this->addToKernelSemList();
	this->val=init;
	this->mySem=mySem;
	this->mySem->myKernelSemID = this->myID;
}

int KernelSem::value () {
	return this->val;
}
void KernelSem::block() {
	System::runningUserThread->state = PCB::BLOCKED;
	BlockedThreads.put((PCB*)System::runningUserThread);
	System::demanded_context_switch=TRUE;
}

void KernelSem::deblock() {
	PCB* readyThread = BlockedThreads.get();
	if (readyThread != NULL) {
		readyThread->state=PCB::READY;
		Scheduler::put(readyThread);
	}
}

void KernelSem::signal() {
	if (val++ < 0) deblock();
}


int KernelSem::wait(int toBlock) {
	//returns 0 if thread ISN'T blocked
	//returns -1 if thread IS blocked
	int ret=0;
	if (!toBlock && val <=0) ret = -1;
	else {
		if (--val < 0) {
			ret=1;
			block();
		}
	}
	return ret;
}

KernelSem::~KernelSem() {
	//deblocking all blocked threads :)

	removeFromKernelSemList();

	PCB* cur = BlockedThreads.get();
	while (cur != NULL) {
		cur->state=PCB::READY;
		Scheduler::put(cur);
	}

}


