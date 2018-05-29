/*
 * queue.cpp
 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */
#include "queue.h"

void Queue::put (PCB* pcb) {
	//head - first element
	// tail - last element
	//FIFO structure
	Node* newNode = new Node (pcb);
	if (head == NULL) { //empty
		head=tail=newNode;
	}
	else {
		tail->next=newNode;
		tail=newNode;
	}
}

PCB* Queue::get () {
	//FIFO structure, removes elements from head
	if (head == NULL) return NULL; //empty queue
	PCB* ret=head->blockedPCB;
	Node* old = head;
	head=head->next;
	if (head == NULL) tail=NULL;
	old->blockedPCB=NULL;
	old->next=NULL;
	//locking ?
	delete old;
	//unlocking ?
	return ret;
}

