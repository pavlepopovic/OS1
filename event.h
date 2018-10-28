/*
 * event.h
 *
 *  Created on: May 16, 2018
 *      Author: OS1
 */

#ifndef EVENT_H_
#define EVENT_H_
#include "ivtentry.h"
#include "kernelev.h"
#include <iostream.h>
class IVTEntry;


class KernelEv;

class Event {
public:
	Event (IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KernelEv;
	void signal(); //can call KernelEv

private:
	KernelEv* myImpl;
};






#endif /* EVENT_H_ */
