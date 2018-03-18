#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

static unsigned long *esp;

void printprocstks(int priority){
	//solution to part 4
	int i;
	int flag[NPROC];
	kprintf("\nvoid printprocstks(int %d)\n",priority);

	for(i=0;i<NPROC;i++){
		flag[i]=0;
	}

	for(i=0;i<NPROC;i++){
		//checking if process has higher priority
		if(proctab[i].pprio > priority)
			flag[i] = 1;
		//checking if process slot is not free
		if(proctab[i].pstate==PRFREE)
			flag[i]=0;
	}
	
	for(i=0;i<NPROC;i++){
		//printing information for only those processes which
		//satisfy above two conditions	
		if(flag[i]==1){
	
			kprintf("Process [%s]\n",proctab[i].pname);
			kprintf("\tpid: %d\n",getpid());
			kprintf("\tpriority: %d\n",proctab[i].pprio);
			kprintf("\tbase: 0x%08x\n",proctab[i].pbase);
			kprintf("\tlimit: 0x%08x\n",proctab[i].plimit);
			kprintf("\tlen: %d\n",proctab[i].pstklen);
			//stack pointer for current process is
			//stored differently than rest processes
			if(i==currpid){
				asm("movl %esp,esp");
				kprintf("\tpointer: 0x%08x\n",esp);
			}
			else
				kprintf("\tpointer: 0x%08x\n",proctab[i].pesp);
	
		}
	}	
}
