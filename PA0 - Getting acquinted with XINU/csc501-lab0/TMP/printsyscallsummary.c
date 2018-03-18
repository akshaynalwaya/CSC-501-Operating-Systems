#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>


int track_sys_call = 1;

int call_freq[NPROC][27];
int call_time[NPROC][27];

char sys_call_mapping[27][11]={"sys_chprio","sys_freemem","sys_getpid","sys_getprio","sys_gettime","sys_kill","sys_receive","sys_recvclr","sys_recvtim",
		"sys_resume","sys_scount","sys_sdelete","sys_send","sys_setdev","sys_setnok","sys_screate","sys_signal","sys_signaln","sys_sleep",
		"sys_sleep10","sys_sleep100","sys_sleep1000","sys_sreset","sys_stacktrace","sys_suspend","sys_unsleep","sys_wait"};

void syscallsummary_start()
{
	initialize_call_freq();
	initialize_call_time();
	track_sys_call = 1;
}

void syscallsummary_stop(){
	track_sys_call = 0;
}

void initialize_call_freq(){
	int i=0,j=0;
	for(i=0;i<NPROC;i++){
		for(j=0;j<27;j++){
			call_freq[i][j]=0;
		}
	}
}

void initialize_call_time(){
	int i=0,j=0;
	for(i=0;i<NPROC;i++){
		for(j=0;j<27;j++){
			call_time[i][j]=0;
		}
	}
}

void printsyscallsummary(){
	int i=0,j=0;
	kprintf("\nvoid printsyscallsummary()\n");
	for(i=0;i<NPROC;i++){
		for(j=0;j<27;j++){
			if(call_freq[i][j]>0){
				kprintf("Process [%d]: %s\n",i,proctab[i].pname);
				kprintf("\tSyscall: %s, Count: %d, average execution time: %d (ms)\n",sys_call_mapping[j],call_freq[i][j],(int)(call_time[i][j]/call_freq[i][j]));
			}
		}
	}
	/*kprintf("for loop completed\n");
	kprintf("%d",call_freq[currpid][18]);
	for(i=0;i<NPROC;i++){
		for(j=0;j<27;j++){
			kprintf("%d ",call_freq[i][j]);
		}
		kprintf("\n");
	}*/

}
