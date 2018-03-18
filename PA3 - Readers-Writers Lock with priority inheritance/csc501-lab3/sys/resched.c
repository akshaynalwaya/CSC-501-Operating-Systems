/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	optr = &proctab[currpid];
	if(optr->pstate == PRCURR){
		if(isempty(rdyhead)){
			return OK;
		}
	}
	int pid = q[rdytail].qprev;
	int max_prio = -1,max_pid=-1;
	int temp=0;
	while(pid != rdyhead){
		max_prio = temp;
		max_pid = pid;
		pid = q[pid].qprev;
	}
	pid = q[rdytail].qprev;
	while(pid != rdyhead){
		if(proctab[pid].pinh < 0){
			temp = proctab[pid].pinh;
		}
		else if(proctab[pid].pinh == 0){
			temp = proctab[pid].pprio;
		}
		else if(proctab[pid].pinh > 0){
			temp = proctab[pid].pinh;
		}
		else{
			temp = proctab[pid].pinh;			
		}
		if(temp > max_prio){
			max_prio = temp;
			max_pid = pid;
		}
		pid = q[pid].qprev;
	}
	int prio_comp = -1;
	if(optr->pinh == 0){
		if(max_pid < optr->pprio){
			if(optr->pstate == PRCURR){
				prio_comp = 1;
			}
		}
	}
	if(optr->pinh!=0){
		if(max_pid < optr->pinh){
			if(optr->pstate == PRCURR){
				prio_comp = 1;
			}
		}
	}
	if(prio_comp==1)
		return OK;
	else{
		//kprintf("Condition not met.continuing to execute.");
	}
	
	/* no switch needed if current process priority higher than next*/
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}	
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	
	currpid = max_pid;
    dequeue(max_pid);
	nptr = &proctab[currpid];
	
	//nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
