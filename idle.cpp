/*
 * idle.cpp
 *
 *  Created on: May 15, 2018
 *      Author: OS1
 */
#include "idle.h"
#include <iostream.h>
void IdleThread :: run () {
	for (int i =0; i < 100; i++) {
		//cout <<"In idle thread!\n";
		if (i==90) i=0;
		for (int j =0; j< 30000; j++)
			for (int k=0; k<30000;k++);
	}
}



