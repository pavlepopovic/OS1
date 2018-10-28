/*
 * timer.cpp
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
/*#include "symbols.h"
#include "thread.h"
#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"
#include "pcb.h"
//included global symbols, dos.h, iostream.h, lock, unlock

unsigned tss;
unsigned tsp;
unsigned tbp; //needed for stack segment, stack pointer, base pointer, which will we store in PCB
volatile Time thread_time_left ;; // left time for running thread on processor
volatile boolean demanded_context_switch = FALSE;

void interrupt timer () {  //timer interrupt
	if (demanded_context_switch == FALSE) thread_time_left --;
	if (thread_time_left == 0 || demanded_context_switch == TRUE) {
		//context switch
#ifndef BCC_BLOCK_IGNORE
		asm { // saving sp, ss and bp
			mov tss, ss
			mov tsp, sp
			mov tbp, bp
		}
#endif
		PCB::running -> sp= tsp;
		PCB::running -> ss = tss;
		PCB::running ->bp = tbp;
		//saved context
		Scheduler::put( (PCB*)PCB::running);
		PCB::running = Scheduler :: get ();
		thread_time_left = PCB::running -> quantum;
		//tsp=PCB::running->sp;
		//tss=PCB::running->ss;
		//tbp=PCB::running ->bp; //?????
#ifndef BCC_BLOCK_IGNORE
		asm { //restoring context
			mov ss, tss
			mov sp, tsp
			mov bp, tbp
		}
#endif
		//calling old interrupt routine if context switch isn't demanded
		if (demanded_context_switch == FALSE) {
#ifndef BCC_BLOCK_IGNORE
			asm {
				int 60h
			}
#endif
		}
		demanded_context_switch == FALSE;
	}
}
unsigned oldTimerOFF, oldTimerSEG;
void inic(){
#ifndef BCC_BLOCK_IGNORE
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
#endif
}
void restore(){
#ifndef BCC_BLOCK_IGNORE
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
#endif
}
int main () {};
*/
