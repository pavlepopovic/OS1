/*
 * kernelev.h
 *
 *  Created on: May 19, 2018
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_
class PCB;

class KernelEv {
public:
	void wait ();
	void signal ();
	~KernelEv();

	friend class IVTEntry;
	friend class Event;

private:

	KernelEv();
	PCB* creator; //easier with pcb-s
	PCB* blocked_creator; //MUST HAVE
	int val;
};



#endif /* KERNELEV_H_ */
