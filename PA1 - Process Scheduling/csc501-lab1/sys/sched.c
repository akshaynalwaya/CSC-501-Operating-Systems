#include <kernel.h>
#include <proc.h>
#include <q.h>
#include "sched.h"
#include "math.h"

scheduler_class = 0;	/* Initializing the scheduler type with 0 */

//setter function for scheduling policy type
void setschedclass(int sched_class){
//set the scheduler class to the value passed as parameter
	scheduler_class = sched_class;
}

//getter function for scheduling policy type
int getschedclass(){
	//return the scheduling class
	return(scheduler_class);
}

/* remove and return a process from the list for
 * exponential distribution scheduler
 */
int expdistsched_getproc(int expdist_proc){
	
	int flag = 0;
	//check if process is less than NPROC
	
	if (expdist_proc < NPROC)
		flag = 1;
	else if (expdist_proc > NPROC)
		flag = 0;
	else if (expdist_proc = NPROC)
		flag = 0;
	else
		flag = 0;

	/* For all processes with flag = 1, dequeue that process
	*  and return the process id; Return 'EMPTY' otherwise
	*/ 
	switch(flag){
		case 1:
			return(dequeue(expdist_proc));
			//break;
		case 0:
			return(EMPTY);
			//break;
		default:
			return(EMPTY);
			//break;
	}

}

/* Finding the next process for exponential scheduling policy 
 * and Linux like scheduling policy
 */

int expdistsched_nextproc(int random_priority){
	int this_process;
	int previous_process;

	this_process = q[rdytail].qprev;
	previous_process = q[this_process].qprev;

	if(random_priority < q[previous_process].qkey){
		if(previous_process < NPROC){
			if(q[this_process].qkey != q[previous_process].qkey){
				this_process = previous_process;
			}
			previous_process = q[previous_process].qprev;
		}
	}
	
	int flag = 0;
	if (this_process == NPROC)
		flag = 1;
	else if (this_process > NPROC)
		flag = 1;
	else
		flag = 0;

	switch(flag){
		case 0:
			return(this_process);
			break;
		case 1:
			return(NULLPROC);
			break;
		default:
			return(this_process);
			break;
	}
}
