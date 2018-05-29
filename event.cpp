/*
 * event.cpp
 *
 *  Created on: May 19, 2018
 *      Author: OS1
 */


#include"event.h"
#include "kernelev.h"


Event::Event(IVTNo ivtno) {
	//linking it with IVTEntry
	myImpl=IVTEntry::entries[ivtno]->myImpl;
}
Event::~Event() {
	//delete myImpl;
	myImpl = NULL; //probably a failure
}
void Event::wait() {
	myImpl->wait();
}
void Event::signal() {
	myImpl->signal();
}
