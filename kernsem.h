/*
 * kernsem.h

 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */




#ifndef KERNSEM_H_
#define KERNSEM_H_
#include "queue.h"
#include "semaphor.h"
class KernelSem {
public:


	friend class Semaphore;

private:
	int val; //value
	Queue BlockedThreads;
	void block();
	void deblock ();
	int wait (int toBlock);
	void signal();
	KernelSem (int init, Semaphore* mysem):val(init), mySem(mysem) {}; //only Semaphore can call this
	Semaphore* mySem;
	~KernelSem();
};




#endif /* KERNSEM_H_ */
