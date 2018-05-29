/*
 * MAIN.cpp
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */

#include "pcb.h"
#include <iostream.h>
#include "system.h"
#include "symbols.h"
#include "thread.h"
#include "semaphor.h"
#include "kernsem.h"
#include <stdlib.h>
#include <event.h>






/*#ifndef BCC_BLOCK_IGNORE
void a(){
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u a() i = "<<i<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <300; ++j);
	}

}
#endif
void b(){
#ifndef BCC_BLOCK_IGNORE
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u b() i = "<<i<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
#endif

}
void c(){
#ifndef BCC_BLOCK_IGNORE
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u c() i = "<<i<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
#endif
	System::exit_thread();
}void d(){
#ifndef BCC_BLOCK_IGNORE
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u d() i = "<<i<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
#endif
	System::exit_thread();
}
void doSomething () {
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	for (int m=0; m < 100; m++) {
		 ->create_context(a);
		cout <<m<<"th "<<"thread created!\n";
		lock
	}
#ifndef BCC_BLOCK_IGNORE
	unlock

	for (int i = 0; i < 30; ++i) {

	  	cout<<"main "<<i<<endl;
	  	unlock

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
		}
		cout<<"Happy End"<<endl;
		unlock
#endif
}*/

class A : public Thread {
public:
	A (StackSize size, Time timeSlice) : Thread(size,timeSlice) {};
	 virtual void run ();
	 ~A() {waitToComplete();}
};
 void A::run () {
#ifndef BCC_BLOCK_IGNORE

	 Thread::sleep(700);
		for (int i =0; i < 30; ++i) {

				cout<<"in class A: i = "<<i<<endl;
				unlock
				for (int k = 0; k<100 /*k<100*/; ++k)
					for (int j = 0; j <30000; ++j);
			}
#endif
}
volatile A* thrA = NULL;
class B : public Thread {
public:
	B(StackSize size, Time timeSlice) : Thread(size,timeSlice) {};
	virtual void run () ;
	~B() {waitToComplete();}
};
 void B::run() {
#ifndef BCC_BLOCK_IGNORE
	/* if (thrA == NULL){
		 //need snync
		 thrA = new A (1024, 5);
		 thrA->start();
	 }
*/
	 //cout << "In B, waiting for A to finish\n";
	// thrA->waitToComplete();
	 //cout <<"A created! Dispatching now!\n";
	 //dispatch();
	 /*Thread *a = new A(1024,50);
	 a->start();
	 cout << "In B, A started\n";
	 delete a;
	 cout <<"In B, A deleted\n";*/
	 //cout <<"A to sleeplist!\n\n";
	 (new A (300,50)->start());
	 Thread::sleep(50);
		for (int i =0; i < 30; ++i) {

				cout<<"in class B: i = "<<i<<endl;
				unlock
				/*if (i == 15) {
					Thread* a = new A (1024,50);
					a->start();
					delete a;
				}*/
				for (int k = 0; k<100 /*k< 100*/; ++k)
					for (int j = 0; j <30000; ++j);
			}
		cout<<" A FINISHED !\n\n\n\n\n\n\n";
#endif
}
class C : public Thread {
public:
	C (StackSize size, Time timeSlice) : Thread(size,timeSlice) {};
	virtual void run () ;
	~C() {waitToComplete();}
};
void C::run () {
/*	if (thrA == NULL) {
		thrA = new A (1024,5);
		thrA->start();
	}
	cout <<"In B, waiting for A to finish\n";
	thrA->waitToComplete();*/
	//Thread* b = new B (400,50);
	//b->start();
	//delete b;
	//cout << "Thread C is sleeping for 60 seconds\n";
	//(new B (200,50)) ->start();
	Thread::sleep(1091+545);
	//Thread* tA = new A(200,40);
	//tA->start();
	//delete tA;
#ifndef BCC_BLOCK_IGNORE
		for (int i =0; i < 30; ++i) {

				cout<<"in class C: i = "<<i<<endl;
				unlock
				//Thread* b = new B (400,50);
				//b->start();
				//delete b;
				for (int k = 0; /*k<10000*/ k<100; ++k)
					for (int j = 0; j <30000 ; ++j);
			}
#endif
}

struct shared_data {
	int array[2];
	int i;
	Semaphore *mutexProducers, *readyToProduce, *readyToConsume;
};

class Producer1 : public Thread {

public:
	~Producer1() {waitToComplete();}
	Producer1 (StackSize size, Time timeSlice, shared_data* shd) : Thread(size,timeSlice), data(shd) {};
	virtual void run ();
private:
	shared_data* data;
};

void Producer1::run() {
	int iter=10;
	while (iter--) {
		data->readyToProduce->wait(1);
		data->mutexProducers->wait(1);
		//producing
		data->array[data->i]= rand()%10;
		data->i=(data->i + 1) %2;
		data->mutexProducers->signal();
		data->readyToConsume->signal();
		Thread::sleep(rand()%100);
	}
}


class Consumer1 : public Thread {
public:
	~Consumer1() {waitToComplete();}
	Consumer1 (StackSize size, Time timeSlice, shared_data* shd) : Thread(size,timeSlice), data(shd) {};
	virtual void run ();
private:
	shared_data* data;

};

void Consumer1::run () {
	int iter=10;
	while (iter--) {
		data->readyToConsume->wait(1);
		data->readyToConsume->wait(1);
		//ready to consume
		cout<<data->array[0]<<" + "<<data->array[1]<<" = " << data->array[0]+data->array[1]<<endl;
		data->readyToProduce->signal();
		data->readyToProduce->signal();
	}
}





void userMain () {
	//Thread *threads[100];
	//Thread *thrC = new C (1024, 70);
	//Thread* thrB = new B (1024,40);
	//Thread* thrB1 = new B (1024,30);

	/*for (int m=0; m< 100; m++) {
		threads[m]=new C (500,40);
		threads[m]->start();
		//if (m == 50) (new A(500,30))->start(),cout <<"A started!\n";

	}
	for (int g=0; g < 100; g++) {
		delete threads[g];
		if (g == 99) (new B(500,30))->start(),cout<<"B created\n";
	}*/
	shared_data *data = new shared_data();
	data->i=0;
	data->mutexProducers=new Semaphore(1);
	data->readyToConsume=new Semaphore(0);
	data->readyToProduce=new Semaphore(2);
	Thread* p1 = new Producer1(1024,2,data);
	Thread* p2 = new Producer1 (1024,2,data);
	Thread* c = new Consumer1(1024,2,data);
	p1->start();
	p2->start();
	c->start();
	delete p1;
	delete p2;
	delete c;
#ifndef BCC_BLOCK_IGNORE
	lock
	cout << "C deleted!\n";
	unlock
#endif
	//thrC->start()
	//cout << "Waiting for C to finish\n";
	//delete thrC;
	//cout << "C deleted, waiting for B to finish\n";
	//delete thrB;
	//cout <<"B deleted, can go on\n";

//cout goooood heeeeeeelp meeeee
#ifndef BCC_BLOCK_IGNORE
	unlock

	for (int i = 0; i < 30; ++i) {

	  	cout<<"main "<<i<<endl;
	  	unlock

		for (int j = 0; j< 10000; ++j)
			for (int k = 0; k < 30000; ++k);
		}
		cout<<"Happy End"<<endl;
		unlock
#endif
}

int userMain (int argc, char* argv[]);
int main (int argc, char* argv []) {
	System::inic_system();
	userMain(argc,argv);
	System::restore_system();

}

