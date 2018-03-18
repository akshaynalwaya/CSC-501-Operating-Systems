/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	unsigned long start_time;
	unsigned long run_time;
    
	if(track_sys_call==1){
		start_time = ctr1000;
	}	
	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][0]++;
		call_time[currpid][0] = call_time[currpid][0] + run_time;
		}

		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][0]++;
		call_time[currpid][0] = call_time[currpid][0] + run_time;
	}
	return(newprio);
}
