/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	short	*nxtdev;
	unsigned long start_time;
	unsigned long run_time;
    
	if(track_sys_call==1){
		start_time = ctr1000;
	}

	if (isbadpid(pid)){
		if(track_sys_call==1){
			run_time = ctr1000 - start_time;
			call_freq[currpid][13]++;
			call_time[currpid][13] = call_time[currpid][13] + run_time;
		} 
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(track_sys_call==1){
		run_time = ctr1000 - start_time;
		call_freq[currpid][13]++;
		call_time[currpid][13] = call_time[currpid][13] + run_time;
	} 
	return(OK);
}
