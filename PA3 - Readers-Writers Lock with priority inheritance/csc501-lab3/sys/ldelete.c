#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
int ldelete (int lockdescriptor)
{
	STATWORD ps;    
	disable(ps);

	struct lentry *lptr;
	int temp = lockdescriptor/10;
	int pid,lock=temp;
	int x = lock*10,y=0;
	int lock_index = lockdescriptor - lock * 10;
	
	if(lock < 0 || lock>NLOCKS){
		//kprintf("ERROR: Invalid lock.\n");
		restore(ps);
		return(SYSERR);
	}

	if(locks[lock].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	
	lptr = &locks[lock];
	lptr->lstate = LFREE;
	switch(lock_index - locks_traverse){
		case 0:
			y = (lock_index = locks_traverse)*100;
			break;	
		case 1:
			break;
		default:
			restore(ps);
			return(SYSERR);
			break;
	}

    int queue_head_status;
	queue_head_status = nonempty(lptr->lqhead);
	
	if(queue_head_status){
		pid = getfirst(lptr->lqhead);
		while(pid != EMPTY){
		    proctab[pid].plockret = DELETED;
		    ready(pid,RESCHNO);
		}
		resched();
	}
	restore(ps);
	return(OK);
}
