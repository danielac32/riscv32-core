//#include <stdint.h>
//#include <kernel.h>
//#include <interrupt.h>
//#include <string.h>
 
#include <xinu.h>

void clkhandler(void){
  clkupdate(10000);
  //kprintf("timer ok\n");
  count1000++;

	/* After 1 sec, increment clktime */

	if(count1000 >= 1000) {
		clktime++;
		count1000 = 0;

	}
    //if(ready_preemptive){
		if(!isempty(sleepq)) {
			/* sleepq nonempty, decrement the key of */
			/* topmost process on sleepq		 */

			if((--queuetab[firstid(sleepq)].qkey) == 0) {

				wakeup();
			}
		}else if((--preempt) == 0) {
			preempt = QUANTUM;
			// PendSV call
			//kprintf("aqui\n");
			resched();
		}
    //}

  
}
