/*
 * kernelev.cpp
 *
 *  Created on: Aug 23, 2018
 *      Author: OS1
 */

#include "kernelev.h"
#include "System.h"
#include "SCHEDULE.H"
#include "PCB.h"
#include "ivtentry.h"
#include "event.h"

volatile ID KernelEv::lastKernelevID = 0;
volatile KernelEv::KernelEvList* KernelEv::head=NULL;
volatile KernelEv::KernelEvList* KernelEv::tail=NULL;

void KernelEv::addToGlobalKernelEvList() {
	KernelEvList* node = new KernelEvList(this);

	if (KernelEv::head == NULL) {
		KernelEv::head=node;
		KernelEv::tail=node;
	} else {
		KernelEv::tail->next=node;
		KernelEv::tail=node;
	}

}

KernelEv* KernelEv::getKernelEvByID(ID id) {
	for (KernelEvList* cur = (KernelEvList*)KernelEv::head; cur != NULL; cur=cur->next) {
		if (cur->elem->myID == id) return cur->elem;
	}
	return NULL;
}

void KernelEv::removeFromGlobalKernelEvList() {
	KernelEvList* cur = (KernelEvList*) head, *prev = NULL;
	while (cur -> elem != this) {
		prev=cur;
		cur=cur->next;
	}
	if (prev == NULL) {
		//deleting at head position
		head=head->next;
		if (head == NULL) tail = NULL;
		cur->elem = NULL;
		cur->next = NULL;
		delete cur;
	}
	else if (cur->next == NULL) {
		// deleting last element
		tail=prev;
		tail->next=NULL;
		cur->elem=NULL;
		delete cur;
	}
	else {
		prev->next=cur->next;
		cur->next=NULL;
		cur->elem=NULL;
		delete cur;
	}
}

KernelEv::KernelEv() {
	this->myID = ++ KernelEv::lastKernelevID;
	this->addToGlobalKernelEvList();
	creator=(PCB*)System::runningUserThread;
	val=0;
	blocked_creator=NULL;
}
KernelEv::~KernelEv() {
	this->removeFromGlobalKernelEvList();
	if (blocked_creator != NULL) {
		creator->state=PCB::READY;
		Scheduler::put(creator);
	}
	creator=NULL;
	blocked_creator=NULL;
}
void KernelEv::wait() {
	if ((PCB*)System::runningUserThread != creator) return;

	if (val == 1) val = 0;
	else {
		//val == 0
		creator->state=PCB::BLOCKED;
		blocked_creator=creator;
		System::demanded_context_switch=TRUE;
	}
}

void KernelEv::signal () {
	//this can be called as a :
	// event.signal() - this will be a system call
	//ivtentry.signal() - in regular code, we won't be on Kernel Thread stack
	//intentry.signal() - in KernelThread stack - need to take care of it

	if (blocked_creator == NULL) val = 1;
	else {
		creator->state = PCB::READY;
		blocked_creator = NULL;
		Scheduler::put(creator);
		if (System::onKernelThread == TRUE) {
			System::demanded_context_switch=TRUE;
		}
		else {
			dispatch();
		}
	}

}

void KernelEv::connect(IVTNo ivtno, Event* ev) {
	ev->myKernelEvID = IVTEntry::entries[ivtno]->myImpl->myID;
}

