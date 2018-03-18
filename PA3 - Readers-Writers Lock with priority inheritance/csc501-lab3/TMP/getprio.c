/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	STATWORD ps;    
	disable(ps);
	struct	pentry	*pptr;

	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}

	int return_prio;
	switch(pptr->pinh){
		case 0:
			return_prio = pptr->pprio;
			break;
		default:
			return_prio = pptr->pinh;
			break;
	}

	restore(ps);
	return(return_prio);
}
