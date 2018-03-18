#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

	STATWORD ps;
	disable(ps);

	int min_num_pages = 1;
	int max_num_pages = 256;
	if(npages < min_num_pages || npages > max_num_pages){
		kprintf("ERROR in get_bs: Number of pages requested are out of bounds.\n");
		return SYSERR;
	}

	if(bs_id < 0 || bs_id > (BS_MAX-1)){
		kprintf("ERROR in get_bs: Backing Store ID is out of bounds.\n");
		return SYSERR;
	}	

	if(bsm_tab[bs_id].bs_sem == 1 || bsm_tab[bs_id].bs_priv_heap == 1){
		kprintf("ERROR in get_bs: Requested backing store couldn't be allocated.\n");
		return SYSERR;
	}

	int num_page;
	num_page = bsm_tab[bs_id].bs_npages;
	//when backing store is unmapped
	if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED){
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
		bsm_tab[bs_id].bs_pid = currpid;
		bsm_tab[bs_id].bs_sem = 0;
		restore(ps);
		return npages;
	}
	else{
		//when backing store is already mapped
		restore(ps);
		return num_page;
	}
	restore(ps);
    return num_page;
}


