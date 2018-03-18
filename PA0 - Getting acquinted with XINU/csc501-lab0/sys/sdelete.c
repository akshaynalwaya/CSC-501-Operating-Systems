/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL sdelete(int sem)
{
	STATWORD ps;    
	unsigned long start_time;
	unsigned long run_time;
    
	if(track_sys_call==1){
		start_time = ctr1000;
	}
	int	pid;
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		restore(ps);
		if(track_sys_call==1){
			run_time = ctr1000 - start_time;
			call_freq[currpid][11]++;
			call_time[currpid][11] = call_time[currpid][11] + run_time;
		}

		return(SYSERR);
	}
	sptr = &semaph[sem];
	sptr->sstate = SFREE;
	if (nonempty(sptr->sqhead)) {
		while( (pid=getfirst(sptr->sqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][11]++;
		call_time[currpid][11] = call_time[currpid][11] + run_time;
	}
	return(OK);
}
