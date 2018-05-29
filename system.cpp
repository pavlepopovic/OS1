/*
 * system.cp
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
#include "symbols.h"
#include "pcb.h"
#include "system.h"
#include <dos.h>
#include "SCHEDULE.H"
#include <iostream.h>
#include "idle.h"
volatile int System :: counter = 20;
volatile PCB *System :: running =NULL; //initialisation
volatile boolean System :: demanded_context_switch = FALSE;
volatile PCB *System :: mainThread = NULL;
volatile int System::number_of_working_threads=0;  //will be implemented in createThread
pInterrupt System::oldISR=NULL;
volatile System::SleepingThreads *System::SleepingThreadsHead=NULL;
volatile PCB* System::idleThread=NULL;
static unsigned tss, tsp, tbp;
void tick();



void System::wakeThreadsUp () {
	if (System::SleepingThreadsHead == NULL) return;
	System::SleepingThreadsHead->timeLeft--; //it can only be 1, 2, 3, 4,..... NEVER 0
	if (System::SleepingThreadsHead->timeLeft == 0) {
		//deblocking all threads which are ready to be woken up
		System::SleepingThreads *cur=NULL;
		while (System::SleepingThreadsHead != NULL && System::SleepingThreadsHead->timeLeft == 0) {
			cur=(System::SleepingThreads*)System::SleepingThreadsHead;
			System::SleepingThreadsHead=System::SleepingThreadsHead->next; //move head to the next element
			cur->mypcb->state=PCB::READY; //deblocking
			Scheduler::put(cur->mypcb);   //put woken up thread into Scheduler
			cur->mypcb=NULL;
			cur->next=NULL;				//delete list node
			delete cur;
		}
	}
	//cout<<"Waking up threads!\n";
}


void interrupt System :: timer () {
	//called every 55 ms, timer interrupt
	 //cout << "In timer" << endl << "Counter is: " << counter << endl;
	 //lock;
	//cout<<"In timer!\n";
	if (demanded_context_switch == FALSE) System::wakeThreadsUp(),tick();
	if (demanded_context_switch == FALSE && running -> quantum != 0) counter --; //timeSlice == 0 => no context switch
	if (demanded_context_switch == TRUE || counter == 0) {//switching context
		if ((System::running->quantum > 0) || demanded_context_switch == TRUE) { //main thread can do context switches, other threads with quantum == 0 can't
#ifndef BCC_BLOCK_IGNORE
			asm {
				mov tss, ss
				mov tsp, sp
				mov tbp, bp
			}
#endif

		running -> ss = tss;
		running -> sp = tsp;
		running -> bp = tbp;
		//cout << "In timer!"<<endl;
		//cout << "Switching context!\n";
#ifndef BCC_BLOCK_IGNORE
	//	asm cli
#endif
		if (running -> state == PCB::READY && System::idleThread != System::running)
		Scheduler :: put ((PCB*)running); //not putting threads if they are blocked or finished
		running = Scheduler::get(); //IDLE thread needed
		if (running == NULL) running=System::idleThread;
		//if context switch isn't demanded, waking up threads
		 //!!
		//restoring context
		tss=running->ss;
		tsp=running->sp;
		tbp=running -> bp;
		System::counter=running->quantum;
#ifndef BCC_BLOCK_IGNORE
		asm {
			mov ss, tss
			mov sp, tsp
			mov bp, tbp
		}
#endif
		}
	}
	if (demanded_context_switch == FALSE)
#ifndef BCC_BLOCK_IGNORE
		asm int 60h
#endif
	demanded_context_switch = FALSE;

}
void System::exit_thread () {
#ifndef BCC_BLOCK_IGNORE
	System::running->deblockAllThreads();
#endif
	//deblocking all threads , after this, all should be deblocked
#ifndef BCC_BLOCK_IGNORE
	// upcoming code should be atomic - it can't be allowed that state is FINSHED, and before restoring main
	// thread timer interrupt comes, and main can't be restored
	lock
	System::running->state=PCB::FINISHED;
	System::number_of_working_threads--; //modify this
	if (System::number_of_working_threads == 0){
		//no more left threads besides main thread, restore it
		System::mainThread->state=PCB::READY;
		Scheduler::put((PCB*)mainThread);
	}
	unlock
#endif
	PCB::PCB_dispatch(); //must restore main thread

}

void System::inic_system() { //called as the first instruction
	//changing interrupt routine
	//creating main thread
#ifndef BCC_BLOCK_IGNORE
	asm cli ; // locking
	System::oldISR = getvect (0x8); //old interrupt routine
	setvect (0x8,(pInterrupt)System::timer);
	setvect (0x60,System::oldISR); //changing old interrupt routine to 60h
	asm sti ; //unlocking
#endif
	//defining main thread
	//creating the idleTHread
	System::idleThread = (new IdleThread (100,2))->mypcb;
	System::idleThread->myThread->start(); //initialise idleThread, don't put it in scheduler
	System::mainThread= (new Thread (0,2))->mypcb; //main thread, put it in scheduler actually
	System::mainThread->state=PCB::READY;
	System::running=System::mainThread;
	System::counter=System::running->quantum; //defining the main thread
}

void System::restore_system() {
	//cout << "Number of working theads is: "<<System::number_of_working_threads<<endl;
	if (System::number_of_working_threads > 0){

		//only main thread can call this, so we block it
		System::mainThread->state=PCB::BLOCKED;
		dispatch();
	}
	cout << "Main thread restored!\n";
	//DELETE MAIN THREAD !!!!!!!!!!!!!
#ifndef BCC_BLOCK_IGNORE
	asm cli
	setvect(0x8, System::oldISR);
	asm sti
#endif
}

void System::addSleepingThread(Time timeToSleep){
	if (timeToSleep <= 1) return; //error
	//cout<<"In addSleepingThread!\n";
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	System::SleepingThreads *newSleepingThread = new System::SleepingThreads((PCB*)System::running,timeToSleep);
	//must be locked !, can't let 2 threads manipulate the same list at the same time, and we can't let a thread go into timer while it is blocked
	System::running->state=PCB::BLOCKED; //thread becomes blocked
	//cout<<"In addSleepingThread!\n";
	if (System::SleepingThreadsHead == NULL) SleepingThreadsHead=newSleepingThread;
	else {
		System::SleepingThreads *cur=(System::SleepingThreads*)System::SleepingThreadsHead, *prev=NULL;
		while (cur != NULL && (int) timeToSleep -(int) cur->timeLeft >= 0) {
			//finding a position
			prev=cur;
			timeToSleep-=cur->timeLeft;
			cur=cur->next;
		}
		//adding to the beggining of the list
		if (prev == NULL) {
			System::SleepingThreadsHead->timeLeft-=timeToSleep;
			newSleepingThread->next=(System::SleepingThreads*)System::SleepingThreadsHead;
			System::SleepingThreadsHead=newSleepingThread;
		}
		else if (cur == NULL) {
			//adding at the end of the list
			prev->next=newSleepingThread;
			newSleepingThread->timeLeft=timeToSleep;
		}
		else {
			//adding somewhere in the middle of the list
			prev->next=newSleepingThread;
			newSleepingThread->next=cur;
			newSleepingThread->timeLeft=timeToSleep;
			cur->timeLeft-=timeToSleep;
		}

	}
	//if (System::running->state == PCB::BLOCKED) cout <<"I BLOCKED A FUCKING THREAD\n";
	/*cout<<"Sleeping threads list: \n";
	for (SleepingThreads* cur =(SleepingThreads*) SleepingThreadsHead; cur != NULL; cur=cur->next){
		cout<<cur->timeLeft<<" ";
	}
	cout<<endl;*/
	PCB::PCB_dispatch();
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif*/

}


