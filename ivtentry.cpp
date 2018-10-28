/*
 * ivtentry.cpp
 *
 *  Created on: Aug 23, 2018
 *      Author: OS1
 */



#include "ivtentry.h"
#include <dos.h>
#include "kernelev.h"


volatile IVTEntry* IVTEntry::entries[256] = {NULL};

void IVTEntry::signal() {
	myImpl->signal();
}

IVTEntry::IVTEntry(IVTNo ivtno, pInterrupt newInterrupt) {
	entries[ivtno]=this;
	this->ivtno = ivtno;
	myImpl = new KernelEv();

#ifndef BCC_BLOCK_IGNORE
	asm cli
	this->oldInterrupt = getvect(ivtno);
	setvect(ivtno,newInterrupt);
	asm sti
#endif
}

void IVTEntry::callOld() {
	this->oldInterrupt();
}
IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	asm cli

	setvect(ivtno,oldInterrupt);

	asm sti

#endif
	delete myImpl;
}

