/*
 * ivtentry.cpp
 *
 *  Created on: May 19, 2018
 *      Author: OS1
 */
#include "ivtentry.h"
#include <dos.h>
#include "kernelev.h"

volatile IVTEntry* IVTEntry::entries[256] = {NULL} ;

void IVTEntry::signal() {
	myImpl->signal();
}
IVTEntry::IVTEntry(IVTNo ivtno, pInterrupt newInterrupt) {
	entries[ivtno] = this;
	this->ivtno=ivtno;
	//need hard lock maybe
	myImpl = new KernelEv ();
#ifndef BCC_BLOCK_IGNORE
	lock
	this->oldInterrupt=getvect(ivtno);
	setvect(ivtno, newInterrupt);
	unlock
#endif
}
void IVTEntry::callOld () {
	this->oldInterrupt();
}

IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	lock
	setvect(ivtno,oldInterrupt);
	unlock
#endif
	delete myImpl;
}

