#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

struct lentry locks[NLOCKS];
int nextlock;
int locks_traverse;

void linit(){
    locks_traverse = 0;
    struct lentry *lptr;
    int i=0, j=0;
    nextlock = NLOCKS - 1;

    while(i < NLOCKS){
        lptr = &locks[i];
        lptr->lstate = LFREE;
        lptr->lqhead = newqueue();
        int temp = lptr->lqhead;
	    lptr->lprio  = -1;	
        lptr->lqtail = 1 + temp;
        j=0;
        while(j<NPROC){
            lptr->proc_log[j] = 0;
            j++;
        }
        i++;
    }
}
