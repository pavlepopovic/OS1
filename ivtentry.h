/*
 * ivtentry.h
 *
 *  Created on: May 19, 2018
 *      Author: OS1
 */




#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "symbols.h"
#include "system.h"




//system.h is included because there is a definition for type pInterrupt
// typedef void interrupt (*pInterrupt) (...);

class KernelEv;

class IVTEntry {
public:
	IVTEntry (IVTNo ivtno, pInterrupt newInterrupt);
	friend class KernelEv;
	friend class Event;
	~IVTEntry();
	void callOld();
	void signal();
private:
	pInterrupt oldInterrupt; //old interrupt routine
	IVTNo ivtno;
	KernelEv* myImpl;
	static volatile IVTEntry* entries[256];

};
#define PREPAREENTRY(numEntry,callFlag)\
void interrupt interr##numEntry(...);\
IVTEntry entry##numEntry(numEntry,interr##numEntry);\
void interrupt interr##numEntry(...){\
	entry##numEntry.signal();\
	if (callFlag != 0){\
		entry##numEntry.callOld();\
	}\
}


#endif /* IVTENTRY_H_ */
