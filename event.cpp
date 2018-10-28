/*
 * event.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: OS1
 */

#include "event.h"
#include "dos.h"
#include "System.h"

Event::Event(IVTNo ivtno) {
	System::softLock();

	EventConsArgs *args = new EventConsArgs(ivtno,this);

	System::softUnlock();

#ifndef BCC_BLOCK_IGNORE
	unsigned argsSeg = FP_SEG(args);
	unsigned argsOff = FP_OFF(args);
	asm {
		push dx
		push ax
		push bx

		mov dx, 000bh
		mov ax, argsSeg
		mov bx, argsOff

		int 61h

		pop bx
		pop ax
		pop dx
	}


#endif
	System::softLock();
	delete args;
	System::softUnlock();
}

Event::~Event(){
#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx

		mov dx, 000eh

		int 61h

		pop dx
	}

#endif
}


void Event::wait() {
	unsigned id = myKernelEvID;

#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push cx

		mov dx, 000ch
		mov cx, id

		int 61h

		pop cx
		pop dx
	}

#endif
}

void Event::signal() {
	unsigned id = myKernelEvID;

#ifndef BCC_BLOCK_IGNORE
	asm {
		push dx
		push cx

		mov dx, 000dh
		mov cx, id

		int 61h

		pop cx
		pop dx
	}




#endif
}
