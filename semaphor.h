/*
 * semaphor.h
 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */

#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

class KernelSem;

class Semaphore {
public:
	Semaphore (int init=0);
	virtual ~Semaphore();

	virtual int wait (int toBlock);
	virtual void signal ();

	int val () const;
private:
	KernelSem* myImpl; //Implementation
};



#endif /* SEMAPHOR_H_ */
