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
        kprintf("blink 1\n");
        sleep(100);
	}
}
void blink2(){
	while(1){
        kprintf("blink 2 \n");
        //sleep(3);
	}
}

void nullprocess(void) {
	ready(create(blink1, 2048, 50, "blink1", 0),RESCHED_NO);
	ready(create(blink2, 2048, 50, "blink2", 0),RESCHED_NO);
	for(;;){
    
	}
}


extern void load_context(context_t* ctx);
extern void save_context(context_t* ctx);
extern void ctxsw(void *, void *, uint8);
/*
extern void sys_switch(struct context *ctx_old, struct context *ctx_new);



#define MAX_TASK 10
#define STACK_SIZE 1024

uint8_t task_stack[MAX_TASK][STACK_SIZE];
struct context ctx_os;
struct context ctx_tasks[MAX_TASK];
struct context *ctx_now;
int taskTop=0;  // total number of task

// create a new task
int task_create(void (*task)(void))
{
	int i=taskTop++;
	ctx_tasks[i].ra = (uint32) task;
	ctx_tasks[i].sp = (uint32) &task_stack[i][STACK_SIZE-1];
	return i;
}

// switch to task[i]
void task_go(int i) {
	ctx_now = &ctx_tasks[i];
	sys_switch(&ctx_os, &ctx_tasks[i]);
}

// switch back to os
void task_os() {
	struct context *ctx = ctx_now;
	ctx_now = &ctx_os;
	sys_switch(ctx, &ctx_os);
}

void lib_delay(volatile int count)
{
	count *= 50000;
	while (count--);
}
void os_kernel() {
	task_os();
}


void user_task0(void)
{
	kprintf("Task0: Created!\n");
	kprintf("Task0: Now, return to kernel mode\n");
	//os_kernel();
	while (1) {
		kprintf("Task0: Running...\n");
		lib_delay(1000);
		//os_kernel();
	}
}

void user_task1(void)
{
	kprintf("Task1: Created!\n");
	kprintf("Task1: Now, return to kernel mode\n");
	os_kernel();
	while (1) {
		kprintf("Task1: Running...\n");
		lib_delay(1000);
		//os_kernel();
	}
}

 */

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

//w_mstatus(r_mstatus() | MSTATUS_MIE);

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
/*

int pid = create((void *)nullprocess, 2048, 10, "Null process", 0);
//struct procent * prptr = &proctab[pid];
//prptr->prstate = PR_CURR;
int pid2 = create(blink1, 2048, 50, "blink1", 0);


struct procent * prptr = &proctab[pid];
struct procent * prptr2 = &proctab[pid2];


load_context(&prptr->prstkptr);
//ready_preemptive=1;
//ready(pid);*/

//nullprocess();


/*w_mstatus(r_mstatus() | MSTATUS_MIE);
w_mie(r_mie() | MIE_MSIE);
w_mie(r_mie() | MIE_MTIE);
clkinit();*/




/*
int pid = create((void *)nullprocess, 2048, 10, "Null process", 0, NULL);
struct procent * prptr = &proctab[pid];
prptr->prstate = PR_CURR;
 
int pid2 = create((void *)blink1, 2048, 50, "blink1", 0);
struct procent * prptr2 = &proctab[pid2];

save_context(&prptr->prstkptr);
load_context(&prptr2->prstkptr);
//ctxsw(&prptr->prstkptr, &prptr2->prstkptr, 0);
//load_context(&prptr2->prstkptr);
//ready(create((void *)nullprocess, 2048, 10, "Null process", 0), RESCHED_YES);
nullprocess();
*/
//nullprocess();


/*
 task_create(&user_task0);
	task_create(&user_task1);
int current_task = 0;
	while (1) {
		kprintf("OS: Activate next task\n");
		task_go(current_task);
		kprintf("OS: Back to OS\n");
		current_task = (current_task + 1) % taskTop; // Round Robin Scheduling
		kprintf("\n");
	}
*/

w_mstatus(r_mstatus() | MSTATUS_MIE);
w_mie(r_mie() | MIE_MSIE);
w_mie(r_mie() | MIE_MTIE);
clkinit();
ready(create((void *)nullprocess, 2048, 10, "Null process", 0), RESCHED_YES);
ready(create((void *)blink1, INITSTK, INITPRIO, "MAIN", 0), RESCHED_YES);
for(;;);//
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
