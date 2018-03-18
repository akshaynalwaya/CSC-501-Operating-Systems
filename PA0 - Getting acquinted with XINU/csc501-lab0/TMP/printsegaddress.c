#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

extern int etext, edata;
extern int *end;

void printsegaddress(){
	//solution to part 2
	kprintf("\nvoid printsegaddress()\n");
	//segment addresses for Xinu OS 
	kprintf("Current: etext[0x%08x]=0x%08x, ",&etext,etext);
	kprintf("edata[0x%08x]=0x%08x, ",&edata,edata);
	kprintf("ebss[0x%08x]=0x%08x\n",&end,end);
	//Contents preceeding above address
	kprintf("Previous: etext[0x%08x]=0x%08x, ",(&etext-1),(etext-1));
	kprintf("edata[0x%08x]=0x%08x, ",(&edata-1),(edata-1));
	kprintf("ebss[0x%08x]=0x%08x\n",(&end-1),(end-1));
	//Content after the current address
	kprintf("After: etext[0x%08x]=0x%08x, ",(&etext+1),(etext+1));
	kprintf("edata[0x%08x]=0x%08x, ",(&edata+1),(edata+1));
	kprintf("ebss[0x%08x]=0x%08x\n",(&end+1),(end+1));
}
