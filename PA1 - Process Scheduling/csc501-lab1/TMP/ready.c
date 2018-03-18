/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include "sched.h"

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;

	int temp;
	int key_value = pptr->pprio;

	switch(scheduler_class){

		case EXPDISTSCHED:
			// Exponential Distribution Scheduler
			insert(pid,rdyhead,key_value);
			break;

		case LINUXSCHED:
			// Linux Scheduler
			temp = key_value + pptr->counter;
			insert(pid,rdyhead,temp);
			break;

		default:
			insert(pid,rdyhead,key_value);
			break;
	}

	if (resch)
	{
		//printf("%s calling ready.c\n",proctab[currpid].pname);
		resched();
	}
	return(OK);
}
