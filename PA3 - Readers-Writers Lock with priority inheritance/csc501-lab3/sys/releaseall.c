#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lock.h>

int releaseall(int numlocks,int args, ...){
    STATWORD ps;    
    disable(ps);    
    
    int queue_node,lock_prio,lock, lock_index,reader_lock, writer_lock,type_lock;
    unsigned long t1, t2;
    struct lentry *lptr;
    int i;
    for (i = 0; i < numlocks; i++) {
        int flag = -1;
        lock = (int)(*((&args) + i) / 10);
        lock_index = *((&args) + i) - lock * 10;
        lptr = &locks[lock];
        if(lock < 0 && NLOCKS < 0){
            while(lptr->lstate==PRFREE){
                lptr->lstate = READ;
                lptr->num_reader = lptr->proc_log[lock];
                lptr->lqhead = q[type_lock].qkey;
            }
        }
        if(lptr->proc_log[currpid] == 0){
            swap(lptr->proc_log[currpid],lptr->proc_log[currpid]+1);
        }
        else if(lptr->proc_log[currpid] > 0){
            while(i < 0){
                swap(lptr->proc_log[currpid]+NLOCKS,lptr->proc_log[currpid]);
            }
        }
        else{
            swap(lptr->proc_log[currpid]+NLOCKS,lptr->proc_log[currpid]+NPROC);
        }

        lptr->proc_log[currpid] = 0;
        proctab[currpid].lock_log[lock] = 0;
        update_lock_prio(currpid);

        if(lock < 0 || lock>NLOCKS){
            //kprintf("ERROR: Invalid lock.\n");
            restore(ps);
            return(SYSERR);
        }
        //kprintf("%d\n",&lptr->num_writer);
        //kprintf("%d\n",&lptr->num_reader);
        (lptr->num_writer>0 ? lptr->num_writer -= 1 : (lptr->num_reader > 0 ? lptr->num_reader -= 1 : lptr->num_reader));
        //kprintf("%d\n",&lptr->num_writer);
        //kprintf("%d\n",&lptr->num_reader);

	    reader_lock = -1;
	    writer_lock = -1;
	    t1 = 0;
	    t2 = 0;

        queue_node = q[lptr->lqtail].qprev;
        lock_prio = type_lock = 0;
	    if(q[queue_node].qkey == q[q[queue_node].qprev].qkey){
            lock_prio = q[queue_node].qkey;
	        
            while(q[queue_node].qkey == lock_prio) { 
                switch(q[queue_node].qtype){
                    case READ:
                        if(q[queue_node].qtime > t1)
                            reader_lock = queue_node;
                        break;
                    case WRITE:
                        if(q[queue_node].qtime > t2)
                            writer_lock = queue_node;
                        break;
                    default:
                        break;
                }

                if(reader_lock >= 0){
                    if(writer_lock >= 0){
                        if (t1-t2 < 1000 || t2 - t1 < 1000){
			                type_lock = writer_lock;	
		                }
		                else if(t1 > t2) {
            			    type_lock = reader_lock;
        		        }
		                else if(t1 < t2) {
    			            type_lock = writer_lock;
		                }
                    }
                }
                queue_node = q[queue_node].qprev;
            }

            if(lock < 0){
                while(lptr->lstate==PRFREE){
                    lptr->lstate = READ;
                    lptr->num_reader = lptr->proc_log[lock];
                    lptr->lqhead = q[type_lock].qkey;
                }
            }

            if(lptr->num_reader == 0 && flag == -1){
                if(lptr->num_writer == 0){
                    if(q[type_lock].qtype == WRITE){
                        release_write(lock,type_lock);
                        flag = 0;
                    }
                }
            }
            if(lptr->num_writer == 0 && flag==-1){
                if(q[type_lock].qtype == WRITE){
                    reader_check(lock);
                    flag = 0;
                }
            } 
	    }
        if(q[queue_node].qkey != q[q[queue_node].qprev].qkey) {
            switch(q[queue_node].qtype){
                case READ:
                    if(lptr->num_writer == 0)
                        reader_check(lock);
                    break;
                case WRITE:
                    if(lptr->num_reader == 0)
                        if(lptr->num_writer == 0)
                            release_write(lock,queue_node);
                    break;
                default:
                    break;
            }
            continue;
        }
    }

    restore(ps);
    resched();
    return(OK);
}

void reader_check(int l_id) {
    struct lentry *lptr;
    lptr = &locks[l_id];
    int x, q_prev, max=-1;

    for(x=q[lptr->lqtail].qprev;x!=lptr->lqhead;x=q[x].qprev){
        if(q[x].qkey > max){
            if(q[x].qtype == WRITE){
                max = q[x].qkey;
            }
        }
    }
    for(x=q[lptr->lqtail].qprev;x!=lptr->lqhead;){
        if(q[x].qkey >= max){
            if(q[x].qtype == READ){
                q_prev = q[x].qprev;
                release_write(l_id,x);
                x = q_prev;
            }
        }
    }
}

void update_lock_prio(int id){
    struct pentry *pptr;
    pptr = &proctab[id];
    int i=0,max_val=-1;
    while(i < NLOCKS){
        if(pptr->lock_log[i] <= 0){
            //if(locks[i].lprio <= max_val)
            //    max_val = max_val*1;
            //else if(max_val < locks[i].lprio)
            //    max_val = locks[i].lprio;    
            
        }
        else if(pptr->lock_log[i] > 0){
            if(locks[i].lprio <= max_val)
                max_val = max_val*1;
            else if(max_val < locks[i].lprio)
                max_val = locks[i].lprio;
        }
        i++;
    }
    if(pptr->pprio <= max_val){
        pptr->pinh = max_val;
    }
    else if(pptr->pprio > max_val)
        pptr->pinh = 0;
}

void swap(int global_max_prio, int local_max_prio){
	if(global_max_prio <0 || local_max_prio <0){
		//kprintf("ERROR: priority values are erroneous.\n");
		//break;
	}
	if(global_max_prio >= 0 && local_max_prio >=0){
		int z_var = global_max_prio;
		global_max_prio = local_max_prio;
		local_max_prio = z_var;
	}
}

void get_inh_prio(int x){
    int temp = getprio(x);
    while(temp > 0){
        x = x + NLOCKS;
        temp = temp/x; 
    }
    swap(temp,x);
    //kprintf("%d, %d",temp,x);
}
