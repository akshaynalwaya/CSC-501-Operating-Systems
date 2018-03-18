#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
int lcreate()
{
	STATWORD ps;    
	disable(ps);

	int x=-1,index,i=0;
	while(i < NLOCKS && x==-1){
		index = nextlock--;
		if(nextlock<0){
			nextlock = NLOCKS - 1;
			locks_traverse += 1;
		}
		if(locks[index].lstate != LFREE){
			x = SYSERR;
		}
		else if(locks[index].lstate == LFREE){
			locks[index].lstate = LUSED;
			locks[index].num_reader = 0;
			locks[index].num_writer = 0;
			x = index*10 + locks_traverse;
			break;
		}
		i++;
	}
	
	if(x==-1){
		restore(ps);
		return(SYSERR);
	}
	else{
		restore(ps);
		return(x);
	}
	
	restore(ps);
	return(x);
}
