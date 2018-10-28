/*
 * thread.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: OS1
 */
#include "thread.h"
#include "System.h"
#include "symbols.h"
#include "PCB.h"
#include <dos.h>

void dispatch () {
#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		mov dx, 0h
		int 61h
		pop dx
	}
#endif
}

Thread::Thread(StackSize stacksize, Time timeSlice) {
	System::softLock();
	Thread::threadConstructorArgs* args = new threadConstructorArgs(stacksize,timeSlice,this);
	System::softUnlock();

#ifndef BCC_BLOCK_IGNORE
	unsigned argSeg = FP_SEG(args);
	unsigned argOff = FP_OFF (args);

	asm  {
		push dx
		push ax
		push bx

		mov dx, 1h
		mov ax, argSeg
		mov bx, argOff

		int 61h

		pop bx
		pop ax
		pop dx
	}
#endif

	System::softLock();
	delete args;
	args = NULL;
	System::softUnlock();


}

Thread::~Thread() {
	unsigned id = this->pcbID;

	if (System::runningKernelThread != NULL) {

	#ifndef BCC_BLOCK_IGNORE
		asm {
			push dx
			push cx

			mov dx, 5h
			mov cx, id

			int 61h

			pop cx
			pop dx
		}


	#endif
	} else {
		delete PCB::getPCBbyID(this->pcbID);
	}
}

void Thread::start() {

	unsigned id = this->pcbID;

#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push cx

		mov dx, 2h
		mov cx, id

		int 61h

		pop cx
		pop dx
	}
#endif

}

void Thread::threadWrapper(Thread* runningThread) {
	runningThread->run();
	System::exit_thread();
}



void Thread::waitToComplete() {
	unsigned id = this->pcbID;
#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push cx

		mov dx, 3h
		mov cx, id

		int 61h

		pop cx
		pop dx

	}


#endif
}


void Thread::sleep(Time timeToSleep) {
	unsigned toSleep = timeToSleep+1;

#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push ax

		mov dx, 4h
		mov ax, toSleep

		int 61h


		pop ax
		pop dx
	}

#endif
}









