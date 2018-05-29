/*
 * semaphor.cpp
 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */
#include "semaphor.h"
#include "symbols.h"
#include "kernsem.h"


Semaphore::Semaphore(int init) {
	this->myImpl=new KernelSem(init,this);
}
Semaphore::~Semaphore() {
	delete myImpl;
	myImpl=NULL;
}
void Semaphore::signal () {
	myImpl->signal();
}

int Semaphore::wait(int toBlock) {
	return myImpl->wait(toBlock);
}
int Semaphore::val () const {
	return myImpl->val;
}
