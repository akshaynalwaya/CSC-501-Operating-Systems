/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	unsigned long start_time;
	unsigned long run_time;
    
	if(track_sys_call==1){
		start_time = ctr1000;
	}
	
	if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][2]++;
		call_time[currpid][2] = call_time[currpid][2] + run_time;
	}
	return(currpid);
}
