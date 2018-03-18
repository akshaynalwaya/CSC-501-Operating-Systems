/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
#include <lab0.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
	unsigned long start_time;
	unsigned long run_time;
    
	if(track_sys_call==1){
		start_time = ctr1000;
	}
    /* long	now; */

	/* FIXME -- no getutim */
	
	if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][4]++;
		call_time[currpid][4] = call_time[currpid][4] + run_time;
	}
    return OK;
}
