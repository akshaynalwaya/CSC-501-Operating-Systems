/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	//kprintf("To be implemented!\n");
	STATWORD ps;
	struct mblock *p,*q;
	unsigned top;

	// checking for possible error conditions 
	if (block < VPN_BASE * NBPG || size == 0){
		kprintf("ERROR in vfreemem: block size exceeds the allowed size.\n");
		return SYSERR;
	}
	disable(ps);

	for( p=proctab[currpid].vmemlist->mnext,q=proctab[currpid].vmemlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= proctab[currpid].vmemlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		return(SYSERR);
	}

	if ( q!= proctab[currpid].vmemlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}

	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}

	restore(ps);
	return(OK);
}
