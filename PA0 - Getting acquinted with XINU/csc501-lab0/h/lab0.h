#ifndef _lab0_h_
#define _lab0_h_

#include <stdio.h>


/* part 1 */
long zfunction();

/* part 2 */
void printsegaddress(void);
extern int edata;
extern int etext;

/* part 3 */
void printtos(void);

/* part 4 */
void printprocstks(int);
						
/* part 5 */
unsigned long ctr1000;
extern int track_sys_call;
extern int call_freq[NPROC][27];
extern int call_time[NPROC][27];

void syscallsummary_start(void);
void syscallsummary_stop(void);
void initialize_call_freq(void);
void initialize_call_time(void);
void printsyscallsummary(void);

#endif
