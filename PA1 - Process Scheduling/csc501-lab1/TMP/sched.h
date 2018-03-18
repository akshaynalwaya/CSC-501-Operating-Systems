#ifndef _SCHED_H_
#define _SCHED_H_

#define EXPDISTSCHED 1
#define LINUXSCHED 2

extern int scheduler_class;

//getter and setter functions for scheduler type
void setschedclass(int sched_class);
int getschedclass();

//functions for exponential scheduler
int expdistsched_getproc(int expdist_proc);
int expdistsched_nextproc(int random_priority);

void preemption_required(int a, int b, int c);

#endif

