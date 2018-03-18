#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCKS 50
#define READ   0
#define WRITE  1

#define LFREE  0
#define LUSED  1


struct lentry {
    char lstate;          
    int  num_reader;             
    int  num_writer;             
    int  ltype;           
    int  lqhead;          
    int  lqtail;          
    int  lprio;           
    int  proc_log[NPROC];  
};
extern struct lentry locks[];
extern int nextlock;
extern int locks_traverse;
extern void modify_lock_prio(int lid);
extern void update_lock_prio(int pid);
extern void get_inh_prio(int);
extern unsigned long ctr1000;
#endif
