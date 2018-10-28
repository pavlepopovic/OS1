/*
 * System.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: OS1
 */

#include <dos.h>
#include "PCB.h"
#include "System.h"
#include "symbols.h"
#include "SCHEDULE.H"
#include "iostream.h"
#include "idle.h"
#include "kernel.h"

volatile System::SleepingThreads* System::SleepingThreadsHead = NULL;
volatile PCB* System::idleThread = NULL;
volatile PCB* System::runningUserThread = NULL;
volatile PCB* System::mainThread = NULL;
volatile int System::counter = 20;
volatile boolean System::demanded_context_switch=FALSE;
volatile int System::number_of_user_threads=0;
pInterrupt System::oldISR=NULL;
volatile KernelThread* System::runningKernelThread = NULL;
pInterrupt System::old61=NULL;
pInterrupt System::old63=NULL;
volatile boolean System::onKernelThread=FALSE;
volatile boolean System::lockedFlag=FALSE;
volatile unsigned System::SysCallID=0;
volatile unsigned System::SysCallObjectID=0;
volatile unsigned System::SysCallArg1=0;
volatile unsigned System::SysCallArg2=0;

static unsigned tsp,tss,tbp;
static unsigned tempAx,tempBx,tempDx,tempCx;


void System::addSleepingThread (Time toSleep) {
	if (toSleep <= 1) return;

	System::SleepingThreads* newSleepingThread = new System::SleepingThreads((PCB*)System::runningUserThread,toSleep);
	System::runningUserThread->state = PCB::BLOCKED;


	if (System::SleepingThreadsHead == NULL) System::SleepingThreadsHead = newSleepingThread;
	else {
		System::SleepingThreads *cur = (System::SleepingThreads*) System::SleepingThreadsHead, *prev = NULL;
		while ((cur != NULL) && (((int)toSleep - (int) cur->timeLeft) >=0)) {
			//finding a position
			prev=cur;
			toSleep-=cur->timeLeft;
			cur=cur->next;
		}

		//adding to the beginning of the list

		if (prev == NULL) {
			System::SleepingThreadsHead->timeLeft-=toSleep;
			newSleepingThread->next=(System::SleepingThreads*)System::SleepingThreadsHead;
			System::SleepingThreadsHead=newSleepingThread;
		}
		else if (cur == NULL) {
			//adding to the end of list
			prev->next=newSleepingThread;
			newSleepingThread->timeLeft=toSleep;
		}
		else {
			//adding somewhere in the middle of the list
			prev->next=newSleepingThread;
			newSleepingThread->next=cur;
			newSleepingThread->timeLeft=toSleep;
			cur->timeLeft-=toSleep;

		}
	}
	System::demanded_context_switch=TRUE; // dispatch();
}

void System::wakeUp () {
	if (System::SleepingThreadsHead == NULL) return;
	System::SleepingThreadsHead->timeLeft--; // it can only be 1, 2 ,4 .... NEVER 0
	if (System::SleepingThreadsHead->timeLeft == 0) {
		//deblocking all threads who are ready to be woken up
		System::SleepingThreads *cur = NULL;
		while (System::SleepingThreadsHead != NULL && System::SleepingThreadsHead->timeLeft == 0) {
			cur = (System::SleepingThreads*)System::SleepingThreadsHead;
			System::SleepingThreadsHead=System::SleepingThreadsHead->next;
			cur->mypcb->state=PCB::READY;
			Scheduler::put(cur->mypcb);
			cur->mypcb=NULL;
			cur->next=NULL;
			delete cur;
		}
	}
}

void interrupt System::goToKernelMode(...) {

	//saving register parameters
#ifndef BCC_BLOCK_IGNORE
	asm {
		mov tempAx, ax
		mov tempBx, bx
		mov tempCx, cx
		mov tempDx, dx
	}

#endif

	//saving sysCall arguments
	// dx - sysCallId
	// cx - objectID
	// ax - arg1
	// bx - arg2


	System::SysCallID=tempDx;
	System::SysCallObjectID=tempCx;
	System::SysCallArg1=tempAx;
	System::SysCallArg2=tempBx;

	System::lockedFlag=TRUE;
	System::onKernelThread=TRUE;
	//locking the system

	//saving user thread context

#ifndef BCC_BLOCK_IGNORE
		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
#endif

		System::runningUserThread->s_p=tsp;
		System::runningUserThread->s_s=tss;
		System::runningUserThread->b_p=tbp;


		//restoring kernel thread


		tsp=System::runningKernelThread->kernel_sp;
		tss=System::runningKernelThread->kernel_ss;
		tbp=System::runningKernelThread->kernel_bp;


#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
#endif


}

void interrupt System::restoreUserThread(...) {
	System::lockedFlag=FALSE;
	System::onKernelThread=FALSE;

	//restoring user thread

	if (System::demanded_context_switch == TRUE) {
		if ((System::runningUserThread->state == PCB::READY) && (System::runningUserThread != System::idleThread)) {
			Scheduler::put((PCB*)runningUserThread);

		}
		System::demanded_context_switch=FALSE;
		System::runningUserThread=Scheduler::get();

		if (System::runningUserThread == NULL ) System::runningUserThread = System::idleThread;

		System::counter=System::runningUserThread->quantum;
	}


	//saving kernel thread context
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
#endif
	System::runningKernelThread->kernel_sp=tsp;
	System::runningKernelThread->kernel_ss=tss;
	System::runningKernelThread->kernel_bp=tbp;

	//restoring user thread context


	tsp=System::runningUserThread->s_p;
	tss=System::runningUserThread->s_s;
	tbp=System::runningUserThread->b_p;

#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
#endif

}

void tick();

void interrupt System::timer(...) {

	tick();
	System::wakeUp();


	if ((System::runningUserThread->quantum >0) && (System::onKernelThread == FALSE)){
		System::counter--;
	}
	if (System::counter <= 0 && System::runningUserThread->quantum > 0) {
		if (System::lockedFlag == FALSE) {
			//context switch permission granted
			//saving running thread context
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
#endif
		System::runningUserThread->s_p=tsp;
		System::runningUserThread->s_s=tss;
		System::runningUserThread->b_p=tbp;

		//restoring user thread context

		if ((System::runningUserThread->state == PCB::READY) && (System::runningUserThread != System::idleThread)) {

			Scheduler::put((PCB*)System::runningUserThread);
		}

		System::runningUserThread=Scheduler::get();


		if (System::runningUserThread == NULL) {
			System::runningUserThread = System::idleThread;
		}

		//restoring context

		tsp=System::runningUserThread->s_p;
		tss=System::runningUserThread->s_s;
		tbp=System::runningUserThread->b_p;

		System::counter=System::runningUserThread->quantum;

#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
#endif


		} else System::demanded_context_switch=TRUE;
	}

#ifndef BCC_BLOCK_IGNORE
	asm int 60h
#endif


}









/*
void interrupt System::timer (...) {
	if (System::demanded_context_switch == FALSE && System::runningUserThread->quantum != 0) System::counter--;
	if (System::counter == 0 || demanded_context_switch == TRUE) {
		if ((System::runningUserThread->quantum > 0) || (System::demanded_context_switch == TRUE)) {
			//context switch - permission gained
			//saving runningKernelThread context
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
#endif
		System::runningUserThread->s_s=tss;
		System::runningUserThread->s_p=tsp;
		System::runningUserThread->b_p=tbp;

		if (System::runningUserThread->state == PCB::READY) {
			Scheduler::put((PCB*)System::runningUserThread);
		}

		System::runningUserThread = Scheduler::get();

		//restoring context

		tss=System::runningUserThread->s_s;
		tsp=System::runningUserThread->s_p;
		tbp=System::runningUserThread->b_p;

		System::counter=System::runningUserThread->quantum;

#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
#endif
		}
	}
	if (System::demanded_context_switch == FALSE) {
#ifndef BCC_BLOCK_IGNORE
		asm int 60h
#endif
	}
	System::demanded_context_switch=FALSE;
} */


void System::inic_system() {

	System::runningKernelThread = new KernelThread();

#ifndef BCC_BLOCK_IGNORE
	asm cli ;
	System::old61=getvect(0x61);
	System::old63=getvect(0x63);
	setvect(0x61,System::goToKernelMode);
	setvect(0x63,System::restoreUserThread);
	System::oldISR=getvect(0x8);
	setvect(0x8,System::timer);
	setvect(0x60,System::oldISR);
	asm sti ;
#endif



	System::mainThread = PCB::getPCBbyID((new Thread (0,2))->pcbID);
	System::mainThread->state=PCB::READY;
	System::runningUserThread=System::mainThread;
	System::counter=System::mainThread->quantum;


	System::idleThread = PCB::getPCBbyID(((new IdleThread(300,2))->pcbID));
	System::idleThread->myThread->start();
}


void System::restore_system() {
#ifndef BCC_BLOCK_IGNORE
	asm cli ;
	if (System::number_of_user_threads > 0) {
		System::mainThread->state=PCB::BLOCKED;
	asm sti ;
		dispatch();
	}
	delete System::runningKernelThread;
	cout << "Main restored!\n";

	asm cli;
		setvect(0x8,System::oldISR);
		setvect(0x61,System::old61);
		setvect(0x63,System::old63);
	asm sti;
#endif

}






void System::exit_thread() {

#ifndef BCC_BLOCK_IGNORE
	System::runningUserThread->deblockAllThreads();
#endif


	System::runningUserThread->state=PCB::FINISHED;
	System::number_of_user_threads--;
#ifndef BCC_BLOCK_IGNORE
	asm cli ;
#endif
	if (System::number_of_user_threads==0 && System::mainThread->state == PCB::BLOCKED) {
		//unblocking of main
		System::mainThread->state=PCB::READY;
		Scheduler::put((PCB*)mainThread);
	}
#ifndef BCC_BLOCK_IGNORE
	asm sti ;
#endif
	dispatch();
}

void System::softLock() {
	System::lockedFlag = TRUE;
}

void System::softUnlock () {
	System::lockedFlag=FALSE;
	if (System::demanded_context_switch == TRUE) {
		dispatch();
	}
}

