/*
 * symbols.h
 *
 *  Created on: May 9, 2018
 *      Author: OS1
 */

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

//in this file, we define global symbols, such as null, true, false ... etc
//this file is included in every other file

void dispatch();


#define NULL 0
#define LOCKED 0
#define UNLOCKED 1
typedef unsigned char IVTNo;

typedef unsigned int boolean;
const boolean TRUE=1;
const boolean FALSE = 0;



#define lock asm cli
#define unlock asm sti
//global symbols

void interrupt timer ();
void inic ();
void restore ();

//global function declarations
#endif /* SYMBOLS_H_ */
