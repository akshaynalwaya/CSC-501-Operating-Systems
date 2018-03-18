#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include "math.h"
#include "sched.h"
#include <stdio.h>

#define LAMBDA 0.1

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
	int random_process;				/* to store random value for exponential scheduler */
	int next_process;				/* to store the value of process */
	int optr_priority;				/* storing the priority of old process entry */

	/* no switch needed if current process priority higher than next*/
	/* next process chosen based on scheduler policy		*/

	// Implementation of Linux scheduler policy
	if (scheduler_class == LINUXSCHED) {
		int good_max = 0;
		int next_process = 0;
		int temp = 0;
		optr = &proctab[currpid];

		int i = 0;
		int j = optr->counter;
		while(i==0){
			temp = optr->goodness;
			//kprintf("%d\n",temp);
			i++;
		}

		if(i!=0){
			optr_priority = temp - j;
			temp = optr_priority;
		}
		//kprintf("optr_priority: %d\n",temp);

		while(preempt){
			optr->goodness = temp + preempt;
			break;
		}
		optr->counter = preempt;
		
		// Handling NULL process
		if (currpid == NULLPROC || optr->counter <=0){
			optr->counter = 0;
			optr->goodness = 0;
		}

		/* Find the process which has maximum goodness amongst all runnable processes */
		int proc_num = q[rdytail].qprev;
		REPEAT:
		if(proc_num != rdyhead){
			while(proctab[proc_num].goodness > good_max)
			{
				next_process = proc_num;
				good_max = proctab[proc_num].goodness;
			}
			proc_num = q[proc_num].qprev;
			goto REPEAT;
		}

		if (optr->counter == 0) {
			if (good_max == 0){
			int n = 0;
			struct pentry *p;
			LOOPING:
			if(n < NPROC){
				p = &proctab[n];
				if (p->pstate != PRFREE){
					//carry forward only half the value of counter
					p->counter = p->pprio + (int)(p->counter)/2;
				}
				// Updating the goodness value
				p->goodness =  p->pprio + p->counter;
				n += 1;
				goto LOOPING;
			}
			//Update the preempt value
			preempt = optr->counter;
			
			if (currpid != NULLPROC){
				next_process = NULLPROC;
				#ifdef	RTCLOCK
					preempt = QUANTUM;
				#endif
			}
			else if (currpid == NULLPROC)
		 		return OK;
			}
		}
		/* for processes having +ve goodness, 
		 * and also more that maximum value */
		else if (optr->goodness > 0){
			if (optr->goodness >= good_max){
				if (optr->pstate != PRCURR)
				{
					temp = optr->counter;
					i = temp + (int)temp/2;
				}
				else{
					preempt = optr->counter;
					return(OK);
				}
			}
		}
		
		/* Assigning values for next process and 
		 * performing context switch */
		/* Context switch Code as present in XINU documentation */
		if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
		}

		nptr = &proctab[next_process];
		nptr->pstate = PRCURR;
		dequeue(next_process);
		currpid = next_process;
		preempt = nptr->counter;
		
		ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
		return(OK);

	}

    // Implementation for exponential scheduling policy
    else if (EXPDISTSCHED == scheduler_class){
		
			// for mean of the exponential distribution to be 10,
			// so, lambda should be 0.1
			random_process = (int) expdev(LAMBDA);
			next_process = expdistsched_nextproc(random_process);

			int flag1=0,flag2=0,flag3=0;
			
			optr = &proctab[currpid];
			
			//check for higher priority process
			if (q[next_process].qkey > optr->pprio)
				flag1 = 1;
			//check for random value lower that priority
			if (optr->pprio > random_process)
				flag2 = 1;
			//check for current process
			if (optr->pstate == PRCURR)
				flag3 = 1;

			//preemption_required(flag1, flag2, flag3);
			
			//checking if preemption counter needs to be reset or not
			if (flag3 && (flag1 && flag2)){
				#ifdef	RTCLOCK
					preempt = QUANTUM;
				#endif
				return(OK);
			}
			
			/*
			//This part not working, so used the flags above
			while(optr->pstate == PRCURR){
				//for current process check these
				if(q[next_process].qkey > optr->pprio){
					if (optr->pprio > random_process)
					{
						#ifdef	RTCLOCK
							preempt = QUANTUM;
						#endif
						return(OK);
					}
				}
			}*/
			
	}
	else {
		//default scheduler code for XINU
		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
			(lastkey(rdytail) < optr->pprio)) {
			return(OK);
		}
	}

	/* force context switch */
	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	/* highest priority decided according to scheduler	*/

	// deciding value of currpid based on the scheduler type
	switch(scheduler_class){
		case EXPDISTSCHED:
			// if exponential scheduler
			currpid = expdistsched_getproc(next_process);
			break;
		
		case LINUXSCHED:
			// if Linux scheduler
			currpid = getlast(rdytail);
			break;

		default:
			// by default in Xinu Scheduler
			currpid = getlast(rdytail);
			break;
	}

	nptr = &proctab[currpid];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter 	*/
	#endif

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}
void preemption_required(int a, int b, int c){
	if (c && (a && b)){
		#ifdef	RTCLOCK
			preempt = QUANTUM;
		#endif
		return(OK);
	}
}
