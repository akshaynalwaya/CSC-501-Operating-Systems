#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

static unsigned long *ebp;

void printtos(){
	//solution to part 3
	asm("mov %ebp,ebp");	
	kprintf("\nvoid printtos()\n");
	
	//printing the address of top of run-time stack
	//before and after entering this function
	kprintf("Before [0x%08x]: 0x%08x\n",(ebp-2),*(ebp-2));
	kprintf("After  [0x%08x]: 0x%08x\n",ebp,*ebp);
	
	//creating some variables to populate stack
	int i,temp[4];
	for(i=0;i<4;i++){
		temp[i] = i;
	}
	//printing contents till 4 locations below the
	//top of stack
	for(i=1;i<5;i++){
		kprintf("\telement[0x%08x]: 0x%08x\n",(ebp-i),*(ebp-i));
	}
}
