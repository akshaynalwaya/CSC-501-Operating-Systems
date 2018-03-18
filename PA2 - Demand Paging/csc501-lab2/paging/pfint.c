/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

#define NOT_FOUND -1

extern int page_replace_policy;

void initialize_page_table(int i){
	pt_t *page_table;

	while(i < NFRAMES){
		page_table[i].pt_pres = 0;
		page_table[i].pt_write = 0;
		page_table[i].pt_user = 0;
		page_table[i].pt_pwt = 0;
		page_table[i].pt_pcd = 0;
		page_table[i].pt_acc = 0;
		page_table[i].pt_dirty = 0;
		page_table[i].pt_mbz = 0;
		page_table[i].pt_global = 0;
		page_table[i].pt_avail = 0;
		page_table[i].pt_base = 0;
		
		i++;
	}

}

int create_page_table()
{
	int i=0,frame_num;
	unsigned int frame_addr;
	pt_t *page_table;

	get_frm(&frame_num);

	frame_addr = (FRAME0 + frame_num) * NBPG;
	page_table = (pt_t*)frame_addr;

	/* Creating page table with 1024 entries */
	while(i < NFRAMES){
		page_table[i].pt_pres = 0;
		page_table[i].pt_write = 0;
		page_table[i].pt_user = 0;
		page_table[i].pt_pwt = 0;
		page_table[i].pt_pcd = 0;
		page_table[i].pt_acc = 0;
		page_table[i].pt_dirty = 0;
		page_table[i].pt_mbz = 0;
		page_table[i].pt_global = 0;
		page_table[i].pt_avail = 0;
		page_table[i].pt_base = 0;
		
		i++;
	}
	//initialize_page_table(i);
	return frame_num;
}

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);

	virt_addr_t *virt_addr; 
	int page_table_new,frame_new_pfint,bs_number,present,page_offset;
	unsigned int virt_pt_offset,virt_pd_offset;
	unsigned long pdbr,v_addr,*temp_fr_content,*temp_bs_content; 
	present = 0;
	pd_t *pd_entry;
	pt_t *pt_entry; 
	
	v_addr = read_cr2();

	virt_addr = (virt_addr_t*)&v_addr;
	virt_pt_offset = virt_addr->pt_offset;
	virt_pd_offset = virt_addr->pd_offset;

	pdbr = proctab[currpid].pdbr;
	pd_entry = pdbr + virt_pd_offset * sizeof(pd_t);

	int entry_present = pd_entry->pd_pres;

	switch(entry_present){
		case 0:
			page_table_new = create_page_table();

			pd_entry->pd_pres = 1;
			pd_entry->pd_write = 1;
			pd_entry->pd_user = 0;
			pd_entry->pd_pwt = 0;
			pd_entry->pd_pcd = 0;
			pd_entry->pd_acc = 0;
			pd_entry->pd_mbz = 0;
			pd_entry->pd_fmb = 0;
			pd_entry->pd_global = 0;
			pd_entry->pd_avail = 0;
			pd_entry->pd_base = FRAME0 + page_table_new;

			frm_tab[page_table_new].fr_status = FRM_MAPPED;
			frm_tab[page_table_new].fr_pid = currpid;
			frm_tab[page_table_new].fr_type = FR_TBL;
	
			break;
		case 1:
			entry_present = 1;
			break;
		default:
			entry_present = 0;
			break;
	}

	while(pd_entry->pd_pres == 1){
		present = NOT_FOUND;
		break;
	}

	pt_entry = (pt_t*)(pd_entry->pd_base*NBPG + virt_pt_offset*sizeof(pt_t));

	int pte_present = 0;

	pte_present = pt_entry->pt_pres;

	switch(pte_present){

		case 0:
			get_frm(&frame_new_pfint);

			pt_entry->pt_pres = 1;
			pt_entry->pt_write = 1;
			pt_entry->pt_base = (FRAME0 + frame_new_pfint);

			frm_tab[pd_entry->pd_base - FRAME0].fr_refcnt++;
			frm_tab[frame_new_pfint].fr_status = FRM_MAPPED;
			frm_tab[frame_new_pfint].fr_type = FR_PAGE;
			frm_tab[frame_new_pfint].fr_pid = currpid;
			frm_tab[frame_new_pfint].fr_vpno = v_addr/NBPG;

			bsm_lookup(currpid,v_addr,&bs_number,&page_offset);
			read_bs((char*)((FRAME0+frame_new_pfint)*NBPG),bs_number,page_offset);

			//common data structure used for both SC and AGING
			if(page_replace_policy == SC){
				insert_frame_SC_AGING(frame_new_pfint);
			}
			else if(page_replace_policy == AGING){
				insert_frame_SC_AGING(frame_new_pfint);
			}

			break;
		
		case 1:
			pte_present = 1;
			break;
		
		default:
			pte_present = 0;
			break;
	}

	while(pt_entry->pt_pres == 1){
		present = NOT_FOUND;
		break;
	}

	write_cr3(pdbr);
	restore(ps);
  	return OK;
}