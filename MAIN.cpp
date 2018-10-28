/*
 * main.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: OS1
 */

#include <stdlib.h>
#include "System.h"
#include "PCB.h"
#include <iostream.h>
#include <stdio.h>
#include "thread.h"
#include "semaphor.h"

class A : public Thread {
public:
	void run ();
	~A(){waitToComplete();};
	A(StackSize s, Time t):Thread(s,t) {};
};

void A::run() {
	int id = PCB::getPCBbyID(this->pcbID)->myID;


	for (int i =0; i < 30; ++i) {
			System::softLock();
			cout << "A i="<<i<<" id:"<<id<<endl;
			System::softUnlock();
			if (i == 2) dispatch ();
			for (int k = 0; k<10000; ++k)
				for (int j = 0; j <30000; ++j);
		}
}

class B : public Thread {
public:
	void run ();
	~B(){waitToComplete();};
	B(StackSize s, Time t):Thread(s,t) {};
};


void B::run () {
/*
	Thread* Athrds[100];

	for (int g = 0; g < 100 ; g++) {
		Athrds[g] = new A (200, 2);
		Athrds[g]->start();
	}

	for (int p = 0 ; p < 100 ; p++) {
		delete Athrds[p];
	}

	cout <<"B sleep 2 minuta\n";
	Thread::sleep(2182);
*/

	int id = this->pcbID;
	for (int i =0; i < 30; ++i) {
			System::softLock();
			cout << "B i="<<i<<" id:"<<id<<endl;
			System::softUnlock();
			if (i == 2) dispatch();

			for (int k = 0; k<15000; ++k)
				for (int j = 0; j <30000; ++j);
		}
}


class C : public Thread {
public:
	~C() {waitToComplete();}
	C():Thread(){};
	void run ();
};

void C::run() {
	Thread::sleep(5455);
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
	/*PCB* thr1 = new PCB (1024,2,a);
	PCB* thr2 = new PCB (1024,2,b);
	thr1->createContext();
	thr2->createContext();*/
/*

	Thread* threadsA [50], *threadsB[50];
	Semaphore* semaphores[10];


	System::softLock();

	for (int v = 0; v < 10 ; v++) {
		if (v == 0) semaphores[v] = new Semaphore();
		else semaphores[v] = new Semaphore(v+1);
	}


	for (int e = 0 ; e < 10 ; e++) {
		cout << "i = " << e <<"  val = " << semaphores[e]->val() << endl;
	}

	System::softUnlock();



	Thread::sleep(500);



	System::softLock();
	for (int z = 0; z < 1; z++) {
		threadsA[z] = new A (200, 2);
		threadsB[z] = new B (200, 2);
		threadsA[z]->start();
		threadsB[z]->start();
	}

	System::softUnlock();


	System::softLock();*/
/*	for (int x=25; x < 50 ; x++) {
		threadsA[x]=new A (200,2);
		threadsB[x] = new B (200,2);

		threadsA[x]->start();
		threadsB[x]->start();
	}*/


	//srand(time(0));

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


		delete data->readyToConsume;
		delete data->readyToProduce;
		delete data->mutexProducers;







	cout<<"threadWrapper 60  SPORE niti timeslice DVADESET KERNEL MODE\n";

#ifndef BCC_BLOCK_IGNORE
	for (int i = 0; i < 30; ++i) {
		System::softLock();
	  	cout<<"U main i="<<i<<endl;
	  	System::softUnlock();

		for (int j = 0; j< 10000; ++j)
			for (int k = 0; k < 30000; ++k);
		}
		cout<<"Happy End"<<endl;
#endif

		/*for (int l = 0; l < 1 ; l++) {
				delete threadsA[l];
				delete threadsB[l];

			}*/

		cout << "deleted all\n";
}

int userMain(int argc, char* argv []);
int main (int argc, char* argv[]) {
	System::inic_system();
	userMain(argc, argv);
	System::restore_system();
}
