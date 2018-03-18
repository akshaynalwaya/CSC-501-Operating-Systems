/* pfintr.S - pfintr */

           .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
    popl pferrcode  
    pushfl          
    cli             /* clear the interrupt flag, and disable interrutps */
    pushal        
    call pfint    
    popal         
    popfl         
    iret            /* to resume the interrupted routine*/