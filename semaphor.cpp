/*
 * semaphor.cpp
 *
 *  Created on: Aug 23, 2018
 *      Author: OS1
 */


#include "semaphor.h"
#include <dos.h>
#include "System.h"

Semaphore::Semaphore(int init) {

	System::softLock();
	semConstructorArgs* constructorArgs = new Semaphore::semConstructorArgs(init,this);
	System::softUnlock();

#ifndef BCC_BLOCK_IGNORE
	unsigned argSeg = FP_SEG(constructorArgs);
	unsigned argOff = FP_OFF(constructorArgs);
	asm {
		push dx
		push ax
		push bx

		mov dx, 6h
		mov ax, argSeg
		mov bx, argOff

		int 61h

		pop bx
		pop ax
		pop dx
	}

#endif


	System::softLock();
	delete constructorArgs;
	System::softUnlock();

}

void Semaphore::signal() {
	unsigned id = this->myKernelSemID;
#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push cx

		mov dx, 9h
		mov cx, id

		int 61h

		pop cx
		pop dx
	}


#endif
}

int Semaphore::val() const {
	int *retValue = new int ;
	unsigned id = this->myKernelSemID;
#ifndef BCC_BLOCK_IGNORE
	unsigned retValueSeg = FP_SEG(retValue);
	unsigned retValueOff = FP_OFF(retValue);

	asm {
		push dx
		push cx
		push ax
		push bx

		mov dx, 7h
		mov cx, id
		mov ax, retValueSeg
		mov bx, retValueOff

		int 61h

		pop bx
		pop ax
		pop cx
		pop dx
	}


#endif

	int ret = *retValue;
	delete retValue;
	return ret;
}

int Semaphore::wait(int toBlock) {
	System::softLock();
	Semaphore::semWaitArgs* semWait = new semWaitArgs(toBlock);
	System::softUnlock();
	unsigned id = this->myKernelSemID;
#ifndef BCC_BLOCK_IGNORE
	unsigned semWaitSeg = FP_SEG(semWait);
	unsigned semWaitOff = FP_OFF(semWait);

	asm {
		push dx
		push cx
		push ax
		push bx

		mov dx, 8h
		mov cx, id
		mov ax, semWaitSeg
		mov bx, semWaitOff

		int 61h

		pop bx
		pop ax
		pop cx
		pop dx
	}

#endif

	int ret = semWait->ret;
	System::softLock();
	delete semWait;
	System::softUnlock();

	return ret;
}
Semaphore::~Semaphore() {
	unsigned id = this->myKernelSemID;
#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push cx

		mov dx, 000ah
		mov cx, id

		int 61h


		pop cx
		pop dx
	}

#endif
}
