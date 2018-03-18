/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	STATWORD ps;
	disable(ps);

    int i = 0;
    while(i < BS_MAX){
    	bsm_tab[i].bs_status = BSM_UNMAPPED;
    	bsm_tab[i].bs_pid = -1;
    	bsm_tab[i].bs_vpno = VPN_BASE;
    	bsm_tab[i].bs_npages = 0;
    	bsm_tab[i].bs_sem = 0;
    	bsm_tab[i].bs_priv_heap = 0;
    	i++;
    }

	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	while(i < BS_MAX){
		if(bsm_tab[i].bs_status == BSM_UNMAPPED){
			*avail = i;
			restore(ps);
			return OK;		
		}
		i++;
	}
	kprintf("ERROR: Backing store couldn't be allocated\n");
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	while(i < BS_MAX){
		if(bsm_tab[i].bs_pid == pid){
			*store = i;
			*pageth = (vaddr/NBPG) - bsm_tab[i].bs_vpno;
			restore(ps);
			return OK;
		}
		i++;
	}

	//kprintf("ERROR: Unable to find the requested backing store\n");
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	bsm_tab[source].bs_status = BSM_MAPPED;
	bsm_tab[source].bs_pid = pid;
	bsm_tab[source].bs_vpno = vpno;
	bsm_tab[source].bs_npages = npages;
	bsm_tab[source].bs_sem = 1;
	bsm_tab[source].bs_priv_heap = 0;
		
	proctab[currpid].vhpno = vpno;
	proctab[currpid].store = source;
}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno)
{
	//kprintf("To be implemented!");
	int i = 0,bs_number,pageth;
	unsigned long vaddr = vpno*NBPG;

	while(i < NFRAMES){
		if(frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_PAGE)
  		{
			bsm_lookup(pid,vaddr,&bs_number,&pageth);
			write_bs( (i+NFRAMES)*NBPG, bs_number, pageth);
  		}
		i++;
	}

	bsm_tab[bs_number].bs_status = BSM_UNMAPPED;
	bsm_tab[bs_number].bs_pid = -1;
	bsm_tab[bs_number].bs_vpno = VPN_BASE;
	bsm_tab[bs_number].bs_npages = 0;
	bsm_tab[bs_number].bs_sem = 0;
	bsm_tab[bs_number].bs_priv_heap = 0;
	
}


