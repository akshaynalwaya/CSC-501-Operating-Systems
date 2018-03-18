#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <sem.h>
#include <stdio.h>


void semaphore1(int s1){
        kprintf("A\n");
        int x = wait(s1);
        kprintf("A: in CS\n");
        sleep(1);
        kprintf("A: moving outside CS.\n");
        signal(s1);
}

void semaphore2(int s2){
        int i=0;
        while(i++ < 50)
                kprintf("B");
        kprintf("B\n");
        //int x = wait(s2);
        //kprintf("B: in CS\n");
        sleep(1);
        i=0;
        while(i++<50)
                kprintf("B");        
        kprintf("B\n");
        //signal(s2);
}

void semaphore3(int s3){
        kprintf("C\n");
        int x = wait(s3);
        kprintf("C: in CS\n");
        sleep(1);
        kprintf("C: moving outside CS.\n");
        signal(s3);
}

void lock1(int l1){
        kprintf("A\n");
        int x = lock(l1,WRITE,20);
        kprintf("A: Lock acquired.\n");
        sleep(1);
        kprintf("A: Releasing lock.\n");
        releaseall(1,l1);
}

void lock2(int l2){
        int i=0;
        while(i++<50)
                kprintf("B");
        kprintf("B\n");
        sleep(3);
        i=0;
        while(i++<50)
                kprintf("B");
        kprintf("B\n");
}

void lock3(int l3){
        kprintf("C\n");
        int x = lock(l3,WRITE,20);
        kprintf("C: Lock acquired\n");
        sleep(1);
        kprintf("C: Releasing lock\n");
        releaseall(1,l3);
}

int main(){

        kprintf("Testing using semaphore.\n");
        int semap = screate(1);
        int sem1 = create(semaphore1,2000,25,"A",1,semap);
        int sem2 = create(semaphore2,2000,30,"B",1,semap);
        int sem3 = create(semaphore3,2000,35,"C",1,semap);

        kprintf("Starting A.\n");
        resume(sem1);
        //sleep(1);
        kprintf("Starting B.\n");
        resume(sem2);
        sleep(1);
        kprintf("Starting C.\n");
        resume(sem3);
        sleep(10);

        kprintf("\nTesting using reader/writer locks with priority inversion.\n");

        int lock_test = lcreate();
        int lck1 = create(lock1,2000,25,"A",1,lock_test);
        int lck2 = create(lock2,2000,30,"B",1,lock_test);
        int lck3 = create(lock3,2000,35,"C",1,lock_test);
        kprintf("Starting A.\n");
        resume(lck1);
        kprintf("Starting B.\n");
        resume(lck2);
        sleep(1);
        kprintf("Starting C.\n");
        resume(lck3);

        sleep(10);
        shutdown();
        return 0;
}
