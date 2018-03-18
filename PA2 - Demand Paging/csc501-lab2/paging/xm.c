/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  /* sanity check ! */
  STATWORD ps;
  disable(ps);

  int min_num_pages = 1;
  int max_num_pages = 256;
  if(npages < min_num_pages || npages > max_num_pages){
    kprintf("ERROR in xmmap: Number of pages requested is out of bounds.\n");
    return SYSERR;
  }

  if(source < 0 || source > (BS_MAX - 1)){
    kprintf("ERROR in xmmap: Value of source is out of bounds.\n");
    return SYSERR;    
  }

  if(virtpage < VPN_BASE){
    kprintf("ERROR in xmmap: Value of virtual page is out of bounds.\n");
  	return SYSERR;
  }
  //Since valid request, call bsm_map
  bsm_map(currpid, virtpage, source, npages);
  
  restore(ps);
  return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
  /* sanity check ! */
  STATWORD ps;
  disable(ps);
 
  if(virtpage < VPN_BASE){ 
    kprintf("ERROR in xmummap: Virtual page is out of bounds.\n");
    return SYSERR;
  }
  // since valid request, call bsm_unmap
  bsm_unmap(currpid, virtpage);

  restore(ps);
  return OK;
}

