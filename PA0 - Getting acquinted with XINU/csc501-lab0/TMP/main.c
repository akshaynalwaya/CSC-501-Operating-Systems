/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

void sndA(){}
void sndB(){}

int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
	long zfunc = zfunction(0xaabbccdd);
	kprintf("zfunc(0xaabbccdd) = 0x%08x\n",zfunc);
	printsegaddress();
	printtos();
	printprocstks(10);


	syscallsummary_start();
	sleep(1);
	int i = getpid();
	resume(create(sndA,1024,20,"Process A",1,1));
	resume(create(sndB,1024,20,"Process B",1,1));	
	syscallsummary_stop();
	printsyscallsummary();
	return 0;
}
