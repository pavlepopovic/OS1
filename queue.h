/*
 * queue.h
 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */

#include <iostream.h>
#include "system.h"
#include "pcb.h"


#ifndef QUEUE_H_
#define QUEUE_H_

class Queue {
public:
	PCB* get ();
	void put (PCB* pcb); //puts pcb's here
	Queue () : head(NULL),tail(NULL) {};
private:
	struct Node {
		PCB* blockedPCB;
		Node* next;
		Node(PCB* pcb):blockedPCB(pcb),next(NULL) {};
	};
	Node *head, *tail;
};



#endif /* QUEUE_H_ */
