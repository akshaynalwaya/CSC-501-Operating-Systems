/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	if(pptr -> pstate == PRREADY) {
		dequeue(pid);
		insert(pid, rdyhead, pptr -> pprio);
	}

	int flag = update_priorities(pid,newprio);
	if(flag == -1){
		restore(ps);
		return(SYSERR);
	}
	restore(ps);
	return(newprio);
}

int update_priorities(int pid,int newprio){
	int x = 1;
	struct	pentry	*pptr;
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		x = -1;
	}
	update_lock_prio(pid);
	while(pptr->pstate == PROC_LOCK){
		modify_lock_prio(pptr->l_proc);
		update_lock_prio(pptr->l_proc);
		x = 1;
		break;
	}
	return(x);
}
