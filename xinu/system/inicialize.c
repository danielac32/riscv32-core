#include <xinu.h>
#include <rv.h>
static	void sysinit(); 	/* Internal system initialization	*/
extern	void meminit(void);	/* Initializes the free memory list	*/
int ready_preemptive=0;



struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	memblk	memlist;

int	prcount;		/* Total number of live processes	*/
pid32	currpid;




void blink1(){
	while(1){
        printf("blink \n");
        //sleep(3);
	}
}


void nullprocess(void) {
	//resume(create(blink1, 2048, 50, "blink1", 0));
	for(;;){
     kprintf("nullprocess\n");
     //sleep(3);
	}
}


extern void load_context(context_t* ctx);
extern void save_context(context_t* ctx) ;
void	nulluser(){
struct	memblk	*memptr;	/* Ptr to memory block		*/
uint32	free_mem;	
ready_preemptive=0;
meminit();
//enable();
sysinit();
preempt = QUANTUM;
free_mem = 0;
for (memptr = memlist.mnext; memptr != NULL;
					memptr = memptr->mnext) {
	free_mem += memptr->mlength;
}
kprintf ("Build date: %s %s\n\n", __DATE__, __TIME__);
kprintf("%10d bytes of free memory.  Free list:\n", free_mem);
for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
    kprintf("           [0x%08X to 0x%08X]\n",
	(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
}

kprintf("%10d bytes of Xinu code.\n",
	(uint32)&_etext - (uint32)&_text);
kprintf("           [0x%08X to 0x%08X]\n",
	(uint32)&_text, (uint32)&_etext - 1);
kprintf("%10d bytes of data.\n",
	(uint32)&_ebss - (uint32)&_sdata);
kprintf("           [0x%08X to 0x%08X]\n\n",
	(uint32)&_sdata, (uint32)&_ebss - 1);

w_mstatus(r_mstatus() | MSTATUS_MIE);

/*
w_mstatus(r_mstatus() | MSTATUS_MIE);
w_mie(r_mie() | MIE_MSIE);
w_mie(r_mie() | MIE_MTIE);
clkinit();

ready(create((void *)nullprocess, 2048, 10, "Null process", 0), RESCHED_YES);
*/

/*
int pid = create((void *)nullprocess, 2048, 10, "Null process", 0, NULL);
struct procent * prptr = &proctab[pid];
prptr->prstate = PR_CURR;
*/


int pid = create((void *)nullprocess, 2048, 10, "Null process", 0);
//struct procent * prptr = &proctab[pid];
//prptr->prstate = PR_CURR;
int pid2 = create(blink1, 2048, 50, "blink1", 0);


struct procent * prptr = &proctab[pid];
struct procent * prptr2 = &proctab[pid2];

//ready_preemptive=1;
//ready(pid);

//nullprocess();
for(;;);
}


static	void	sysinit()
{
	int32	i;
	struct	procent	*prptr;		/* Ptr to process table entry	*/
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/


	/* Platform Specific Initialization */
    //meminit();
	  //platinit();
    
	//kprintf(CONSOLE_RESET);
	for (int i = 0; i < 10; ++i)
	{
		kprintf("\n");
	}
	/* Initialize free memory list */
	
	

	/* Initialize system variables */

	/* Count the Null process as the first process in the system */
	prcount = 0;
//	prcount = 1;

	/* Scheduling is not currently blocked */

	Defer.ndefers = 0;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}


	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	
	readylist = newqueue();

	
	for (i = 0; i < NDEVS; i++) {
		//init(i);
	}
	return;
}
