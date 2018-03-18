#include <kernel.h>
#include <stdio.h>
#include <q.h>
#include <proc.h>
#include <lock.h>

int lock(int ldes1, int type, int priority) {

    STATWORD ps;    
    disable(ps);
    int count = 10;
    struct lentry *lptr;
    struct pentry *pptr;
    
    int lock = (int)(ldes1/10);
    lptr = &locks[lock];
	if(lock < 0 || lock>NLOCKS){
		//kprintf("ERROR: Invalid lock.\n");
		restore(ps);
		return(SYSERR);
	}
    if(lptr->lstate == LFREE) {
        restore(ps);
        return(SYSERR);
    }

    int lock_index = ldes1;
    int iter = 0;
    while(iter<count){
        lock_index -= lock;
        iter++;
    }

    if(iter==count){
        if(locks_traverse == lock_index){
            //kprintf("valid index value assigned.\n");
        }
        else{
            restore(ps);
            return(SYSERR);
        }
    }

    int should_wait = 0;
    if(lock < 0){
        while(lptr->lstate==PRFREE){
            lptr->lstate = READ;
            lptr->num_reader = lptr->proc_log[lock];
            //lptr->lqhead = q[type_lock].qkey;
        }
    }
    int reader_count = lptr->num_reader,writer_count = lptr->num_writer;
    if(reader_count<0){
        restore(ps);
        return(SYSERR);
    }
    if(writer_count<0){
        restore(ps);
        return(SYSERR);
    }
    if(reader_count == 0){
        if(writer_count == 0){
            should_wait = 0;
        }
        else if(writer_count!=0){
            should_wait = 1;
        }
    }
    if(reader_count != 0){
        if(writer_count == 0){
            if(type == WRITE){
                should_wait = 1;
            }
            else if(type == READ){
                int lock_desc = q[lptr->lqtail].qprev;
                while(priority < q[lock_desc].qkey){
                    if(q[lock_desc].qtype == WRITE){
                        should_wait = 1;
                    }
                    lock_desc = q[lock_desc].qprev;
                }
            }
        }
    }
    int check=1;
    switch(should_wait){
        case 0:
            lptr->proc_log[currpid] = 1;
            proctab[currpid].lock_log[lock] = 1;
            update_lock_prio(currpid);
            switch(type){
                case READ:
                    lptr->num_reader++;
                    break;
                case WRITE:
                    lptr->num_writer++;
                    break;
                default:
                    break;    
            }
            restore(ps);
            return(OK);
            //break;
        case 1:
            check = set_params(currpid,priority,type,lock);
            if(check!=OK){
                //kprintf("lock.c: Error in setting the queue and proctab values.\n");
            }
            modify_lock_prio(lock);
            struct lentry *temp_lock;
            temp_lock = &locks[lock];
            int i=0;
            while(i < NPROC){
                if(temp_lock->proc_log[i] > 0)
                    update_lock_prio(i);
                i++;
            }

            resched();
            restore(ps);
            return(pptr->plockret);
            //break;
        
        default:
            break;
    }
    if(lptr->proc_log[currpid] == 0){
        swap(lptr->proc_log[currpid],lptr->proc_log[currpid]+1);
    }
    else if(lptr->proc_log[currpid] > 0){
        int i=0;
        while(i < 0){
            swap(lptr->proc_log[currpid]+NLOCKS,lptr->proc_log[currpid]);
        }
    }
    else{
        swap(lptr->proc_log[currpid]+NLOCKS,lptr->proc_log[currpid]+NPROC);
    }


    restore(ps);
    return(OK);
}

int get_max(int lock_id){
    struct lentry *lptr;
    lptr = &locks[lock_id];
    int x = q[lptr->lqtail].qprev,max=0;
    for(x = q[lptr->lqtail].qprev; x!=lptr->lqhead; x = q[x].qprev){
        if(proctab[x].pprio > max){
            max = proctab[x].pprio;
        }
    }
    return max;

}

void modify_lock_prio(int l_id){
    struct lentry *lptr;
    lptr = &locks[l_id];
    lptr->lprio = get_max(l_id);
}

int set_params(int proc_id,int prio,int type,int lock){
    struct pentry *pptr;
    struct lentry *lptr;
    lptr = &locks[lock];
    pptr = &proctab[proc_id];
    pptr->pstate = PROC_LOCK;
    pptr->l_proc = lock;
    pptr->plockret = OK;
    insert(proc_id,lptr->lqhead,prio);
    q[proc_id].qtype = type;
    q[proc_id].qtime = ctr1000;
    return(OK);
}

void release_write(int l_id, int p) {
    struct lentry *lptr;
    lptr = &locks[l_id];
    lptr->proc_log[p] = 1;
    proctab[currpid].lock_log[l_id] = 1;
    switch(q[p].qtype){
        case READ:
            lptr->num_reader += 1;
            break;
        case WRITE:
            lptr->num_writer += 1;
            break;
        default:
            break;
    }
    modify_lock_prio(l_id);
    int i=0;
    while(i < NPROC){
        if(lptr->proc_log[i] > 0)
            update_lock_prio(i);
        i++;
    }
    dequeue(p);
    ready(p, RESCHNO);
}
