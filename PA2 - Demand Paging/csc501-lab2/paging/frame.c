/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
	//kprintf("To be implemented!\n");
	STATWORD ps;
	disable(ps);
	int i = 0;

	while(i < NFRAMES){
  		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = 0;
  		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_PAGE;
		frm_tab[i].fr_dirty = 0;
		
  		i++;
  	}
 	restore(ps);
  	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	//kprintf("To be implemented!\n");
  	STATWORD ps;
  	disable(ps);
  	int i = 0,frame_number = -1;
  	//checking for unmapped frame 
  	while(i < NFRAMES){
  		if(frm_tab[i].fr_status == FRM_UNMAPPED){
  			*avail = i;
  			restore(ps);
  			return OK;
  		}
  		i++;
  	}

  	//Debugging working of loop
  	if(i < 0){
  		kprintf("Error in get_frm() working.\n");
  	}

  	//calling remove function corresponding to the replacement policy
  	switch(page_replace_policy){

  		case SC:
  			//kprintf("Calling remove function for SC\n");
  			frame_number = frame_removal_SC_policy();
  			break;

  		case AGING:
  			//kprintf("Calling remove function for AGING\n");
  			frame_number = frame_removal_AGING_policy();
  			break;

  		default:
  			restore(ps);
  			return SYSERR;
  			break;
  	}
  	//free frame only if SC or AGING returned some valid frame number
  	if(frame_number > -1){
  		free_frm(frame_number);
		*avail = frame_number;
		restore(ps);
		return OK;
  	}
  	else{
  		//return System Error
  	}
	
  	restore(ps);
  	return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	//kprintf("To be implemented!\n");
	STATWORD ps;
	disable(ps);
	unsigned long v_addr,pdbr;	
	unsigned int pt_offset,pd_offset;
	
	pd_t *pd_entry; 
	pt_t *pt_entry;
	int backing_store,page_num,frame_tab_pid;

	switch(frm_tab[i].fr_type){

		case FR_PAGE:
			
			frame_tab_pid = frm_tab[i].fr_pid;
			v_addr = frm_tab[i].fr_vpno;
			
			pdbr = proctab[frame_tab_pid].pdbr;

			pd_offset = v_addr / NFRAMES;
			pt_offset = v_addr&1023;
			
			pd_entry = pdbr + (pd_offset*sizeof(pd_t));
			pt_entry = (pd_entry->pd_base*NBPG) + (pt_offset*sizeof(pt_t));
			backing_store = proctab[frm_tab[i].fr_pid].store;
			page_num = frm_tab[i].fr_vpno-proctab[frame_tab_pid].vhpno;
			
			if(i < FR_PAGE){
				kprintf("ERROR while freeing a frame: Invalid frame number passed.\n");
				restore(ps);
				return SYSERR;
			}

			write_bs((i+FRAME0)*NBPG, backing_store, page_num);
			pt_entry->pt_pres = 0;
			frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt -= 1;
			int temp = frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt;
			if(temp == 0){
				if(page_replace_policy == AGING){
					//to be done
				}
				else if(page_replace_policy == SC){
					temp = frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt;
				}
				frm_tab[pd_entry->pd_base-FRAME0].fr_pid = -1;
				frm_tab[pd_entry->pd_base-FRAME0].fr_status = FRM_UNMAPPED;
				frm_tab[pd_entry->pd_base-FRAME0].fr_type = FR_PAGE;
				frm_tab[pd_entry->pd_base-FRAME0].fr_vpno = VPN_BASE;
				pd_entry->pd_pres = 0;
			}

			break;
		case FR_TBL:
			if(i < FR_TBL){
				kprintf("ERROR while freeing a frame.\n");
				restore(ps);
				return SYSERR;
			}

			break;
		case FR_DIR:
			if(i < FR_DIR){
				kprintf("ERROR while freeing a frame.\n");
				restore(ps);
				return SYSERR;
			}
			break;

		default:
			break;
	}

 	restore(ps);
	return OK;
}

void initialize_frames_SC_AGING()
{
	int i = 0;
	while(i < NFRAMES){
		frame_queue[i].frm_id = i;
		frame_queue[i].next_frame = -1;
		frame_queue[i].frm_age = 0;
		i++;
	}
}

void insert_frame_SC_AGING(int frame_num)
{
	STATWORD ps;
	disable(ps);
	int nxt_frm = -1;
	int curr_frm = -1;
	
	//when queue is empty
	if(queue_head == -1){
		queue_head = frame_num;
		restore(ps);
		return OK;
	}
	//if queue has some elements already
	else{
		nxt_frm = frame_queue[queue_head].next_frame;
		curr_frm = queue_head;
	}
	//finding the last frame/element in the queue
	while(nxt_frm != -1){
		curr_frm = nxt_frm;
		nxt_frm = frame_queue[nxt_frm].next_frame;
	}
	
	frame_queue[curr_frm].next_frame = frame_num;
	frame_queue[frame_num].next_frame = -1;
	
	restore(ps);
	return OK;
}

int frame_removal_SC_policy()
{
	STATWORD ps;
	disable(ps);

	int frame_number=0,curr_frm=0,nxt_frm = -1,prev_frm = -1,page_table_new,page_offset;
	unsigned int virt_pt_offset,virt_pd_offset;
	unsigned long pdbr,v_addr; 
	
	virt_addr_t *virt_addr; 
	
	pd_t *pd_entry;
	pt_t *pt_entry; 
	
	while(curr_frm != -1){

		v_addr = frm_tab[curr_frm].fr_vpno;

		virt_addr = (virt_addr_t*)&v_addr;
		virt_pt_offset = virt_addr->pt_offset;
		virt_pd_offset = virt_addr->pd_offset;

		pdbr = proctab[currpid].pdbr;
		pd_entry = pdbr + virt_pd_offset * sizeof(pd_t);
		pt_entry = (pt_t*)(pd_entry->pd_base*NBPG + virt_pt_offset*sizeof(pt_t));
		frame_number = queue_head;
		
		if(pt_entry->pt_acc == 0){
			//when queue head has pt_acc = 0 and hence should be removed
			if(prev_frm==-1){
				queue_head = frame_queue[curr_frm].next_frame;
				frame_queue[curr_frm].next_frame = -1;
				if(debugging_mode == 1)
					kprintf("Removing frame %d\n", frame_number);
				restore(ps);
				return (frame_number);
			}
			//some node after head has to be removed
			else{
				frame_queue[prev_frm].next_frame = frame_queue[curr_frm].next_frame;
				frame_queue[curr_frm].next_frame = -1;
				if(debugging_mode == 1){
					kprintf("Removing frame %d\n", frame_number);
				}
				restore(ps);
				return(frame_number);
			}

		}
		else{
			//setting access bit to 0 if previously set to 1
			pt_entry->pt_acc = 0;
		}
		prev_frm = curr_frm;
		curr_frm = frame_queue[curr_frm].next_frame;		
	}

	//if the head of queue is to be removed
	queue_head = frame_queue[curr_frm].next_frame;
	frame_queue[curr_frm].next_frame = -1;
	if(debugging_mode == 1)
		kprintf("SC Policy: Removing frame %d\n", frame_number);
	restore(ps);
	return (frame_number);

}

int frame_removal_AGING_policy(){
	STATWORD ps;
	disable(ps);
	
	int frame_number=0,curr_frm,nxt_frm = -1,prev_frm = -1,page_table_new,page_offset;
	unsigned int virt_pt_offset,virt_pd_offset;
	unsigned long pdbr,v_addr; 
	
	virt_addr_t *virt_addr; 
	
	pd_t *pd_entry;
	pt_t *pt_entry; 

	while(curr_frm != -1){

		v_addr = frm_tab[curr_frm].fr_vpno;

		virt_addr = (virt_addr_t*)&v_addr;
		virt_pt_offset = virt_addr->pt_offset;
		virt_pd_offset = virt_addr->pd_offset;

		pdbr = proctab[currpid].pdbr;
		pd_entry = pdbr + virt_pd_offset * sizeof(pd_t);
		pt_entry = (pt_t*)(pd_entry->pd_base*NBPG + virt_pt_offset*sizeof(pt_t));
		frame_number = queue_head;
		
		//decrementing the age of each element by half
		frame_queue[curr_frm].frm_age = frame_queue[curr_frm].frm_age/2;

		if(pt_entry->pt_acc == 1){
			//incrementing age of element and comparing with 255
			int temp = frame_queue[curr_frm].frm_age + 128;
			if(temp < 255)
				frame_queue[curr_frm].frm_age = temp;
			else
				frame_queue[curr_frm].frm_age = 255;
		}
		//finding the frame with minimum age value
		if(frame_queue[curr_frm].frm_age < frame_queue[frame_number].frm_age)
			frame_number = curr_frm;

		prev_frm = curr_frm;
		curr_frm = frame_queue[curr_frm].next_frame;		
	}

	if(debugging_mode == 1)
		kprintf("AGING Policy: Removing frame %d\n", frame_number);
	restore(ps);
	return (frame_number);
}
