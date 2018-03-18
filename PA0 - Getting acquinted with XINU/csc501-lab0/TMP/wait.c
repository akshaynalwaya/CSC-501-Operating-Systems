/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;
	unsigned long start_time;
	unsigned long run_time;
    
	if(track_sys_call==1){
		start_time = ctr1000;
	}
	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(track_sys_call==1){
			run_time = ctr1000 - start_time;
			call_freq[currpid][26]++;
			call_time[currpid][26] = call_time[currpid][26] + run_time;
		}	

		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		if(track_sys_call==1){
			run_time = ctr1000 - start_time;
			call_freq[currpid][26]++;
			call_time[currpid][26] = call_time[currpid][26] + run_time;
		}	

		return pptr->pwaitret;
	}
	restore(ps);
	if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][26]++;
		call_time[currpid][26] = call_time[currpid][26] + run_time;
	}	
	return(OK);
}
