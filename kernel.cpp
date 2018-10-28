/*
 * kernel.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: OS1
 */


#include "kernel.h"
#include "System.h"
#include <dos.h>
#include <iostream.h>
#include "thread.h"
#include "PCB.h"
#include "semaphor.h"
#include "kernsem.h"
#include "event.h"
#include "kernelev.h"





KernelThread::KernelThread () {
	this->size = 2048; //4096 bytes
	this->kernelStack = new unsigned [this->size];
	//creating kernel thread context
#ifndef BCC_BLOCK_IGNORE
	this->kernelStack[this->size-1]=0x200; //I bit = 1
	this->kernelStack[this->size-2]=FP_SEG(KernelThread::kernelWrapper);
	this->kernelStack[this->size-3]=FP_OFF(KernelThread::kernelWrapper);
	this->kernel_sp=FP_OFF(this->kernelStack + (this->size - 12));
	this->kernel_ss=FP_SEG(this->kernelStack + (this->size - 12));
	this->kernel_bp=FP_OFF(this->kernelStack + (this->size - 12));
#endif
	//cout << "Created kernel thread!\n";

}


void KernelThread::run () {
	while (1) {
		//cout << "In syscall!\n";
		switch(System::SysCallID) {
			case 0x0:
				// dispatch ()
				System::demanded_context_switch=TRUE;
				break;
			case 0x1:
				// Thread (StackSize, Time)
#ifndef BCC_BLOCK_IGNORE
				Thread::threadConstructorArgs* args = (Thread::threadConstructorArgs*)MK_FP(System::SysCallArg1,System::SysCallArg2);
#endif
				//cout << "In threadCall\n";

				new PCB(args->stacksize, args->timeslice, args->threadArg);
				break;
			case 0x2:
				//Thread::start()
				//cout << "start sys call!\n";
				PCB* startPCB = PCB::getPCBbyID(System::SysCallObjectID);
				startPCB->createContext();
				break;
			case 0x3:
				//Thread::waitToComplete();
				//cout << "WaitToComplete SysCall id: " << System::SysCallObjectID << endl;

				PCB* waitToCompletePCB = PCB::getPCBbyID(System::SysCallObjectID);
				waitToCompletePCB->pcb_waitToComplete();
				break;
			case 0x4:
				// Thread :: sleep (Time toSleep)
				//cout << "Sys call - sleep:\n";
				System::addSleepingThread(System::SysCallArg1);
				break;
			case 0x5:
				// Thread::~Thread();
				//cout <<"Sys call ~Thread()\n";
				PCB* destructorArg = PCB::getPCBbyID(System::SysCallObjectID);
				delete destructorArg;
				break;
			case 0x6:
				// Semaphore::Semaphore(int init)
				//cout << "Semaphore() sysCall\n";
#ifndef BCC_BLOCK_IGNORE
				Semaphore::semConstructorArgs *semConsArgs = (Semaphore::semConstructorArgs*)MK_FP(System::SysCallArg1,System::SysCallArg2);
#endif
				new KernelSem(semConsArgs->initVal, semConsArgs->sem);
				break;
			case 0x7:
				// int Semaphore::val() const;
				//cout << "Semaphore::val() sysCall id: " << System::SysCallObjectID <<endl;
#ifndef BCC_BLOCK_IGNORE
				int *retValue = (int*)MK_FP(System::SysCallArg1,System::SysCallArg2);
#endif
				KernelSem *kSem = KernelSem::getKernelSembyID(System::SysCallObjectID);
				*retValue = kSem->value();
				break;
			case 0x8:
				// int Semaphore::wait(int)
				//cout << "Syscall wait (toBlock)\n id:" << System::SysCallObjectID << endl;
#ifndef BCC_BLOCK_IGNORE
				Semaphore::semWaitArgs* waitArgs = (Semaphore::semWaitArgs*)MK_FP(System::SysCallArg1,System::SysCallArg2);
#endif
				KernelSem* waitSem = KernelSem::getKernelSembyID(System::SysCallObjectID);
				waitArgs->ret=waitSem->wait(waitArgs->toBlock);
				break;
			case 0x9:
				//void Semaphore::signal()
				//cout << "Syscall: Semaphore::signal() id: " << System::SysCallObjectID<<endl;
				KernelSem* signalSem = KernelSem::getKernelSembyID(System::SysCallObjectID);
				signalSem->signal();
				break;
			case 0xa:
				// Semaphore::~Semaphore();
				//cout << "Syscall: Semaphore::~Semaphore() id: " << System::SysCallObjectID<<endl;
				KernelSem* destroySem = KernelSem::getKernelSembyID(System::SysCallObjectID);
				delete destroySem;
				break;
			case 0xb:
				//event(ivtno)
#ifndef BCC_BLOCK_IGNORE
				Event::EventConsArgs* eventCons = (EventConsArgs*)MK_FP(System::SysCallArg1, System::SysCallArg2);
#endif
				KernelEv::connect(eventCons->ivt, eventCons->ev);
				break;
			case 0xc:
				// event.wait()
				KernelEv* waitEv = KernelEv::getKernelEvByID(System::SysCallObjectID);
				waitEv->wait();
				break;

			case 0xd:

				//event.signal()
				KernelEv* signalEv = KernelEv::getKernelEvByID(System::SysCallObjectID);
				signalEv->signal();


				break;
			case 0xe:
				//~Event()
				// nothing to be done here
				//cout << "~Event()\n";
				break;


		}
#ifndef BCC_BLOCK_IGNORE
		asm int 63h
#endif

	}
}



void KernelThread::kernelWrapper() {
	System::runningKernelThread->run();
}




KernelThread::~KernelThread() {
	delete [] this->kernelStack;
	this->kernelStack = NULL;
	System::runningKernelThread = NULL;
}
