/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;		/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;		/* arguments (treated like an	*/
						/* array in the code)		*/
{
	//kprintf("To be implemented!\n");
	
	STATWORD ps;
	disable(ps);
	int pid,bs_store;
	pid = create(procaddr,ssize,priority,name,nargs,args);
	
	//check if a particular backing store can be acquired
	if(get_bsm(&bs_store) == SYSERR)
		return SYSERR;

	//calling bsm_map as no error conditions found true
	bsm_map(pid,VPN_BASE,bs_store,hsize);
	
	proctab[pid].store = bs_store;
	proctab[pid].vhpno = VPN_BASE;
	proctab[pid].vhpnpages = hsize;
	proctab[pid].vmemlist->mnext = VPN_BASE * NBPG;
	struct mblock *base;
	base = BACKING_STORE_BASE + (bs_store * BACKING_STORE_UNIT_SIZE);
	base->mlen = hsize * NBPG;
	base->mnext = NULL;

	restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
