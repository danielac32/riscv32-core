/* create.c - create, newpid */

#include <xinu.h>

local	int newpid();

#define	roundew(x)	( (x+3)& ~0x3)

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */



extern void *setupStack(void *stackaddr, void *procaddr,
                 void *retaddr, uint nargs, va_list ap);








pid32	create(
	  void		*procaddr,	/* procedure address		*/
	  uint32	ssize,		/* stack size in bytes		*/
	  pri16		priority,	/* process priority > 0		*/
	  char		*name,		/* name (for debugging)		*/
	  uint32	nargs,		/* number of args that follow	*/
	  ...
	)
{
	intmask 	mask;    	/* interrupt mask		*/
	pid32		pid;		/* stores new process id	*/
	struct	procent	*prptr;		/* pointer to proc. table entry */
	int32		i;
	uint32		*a;		/* points to list of args	*/
	uint32		*saddr;		/* stack address		*/
    va_list ap;  

	mask = disable();
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (uint32) roundew(ssize);
	
	//if (((saddr = (uint32 *)getstk(ssize)) ==
	if (((saddr = (uint32 *)malloc(ALIGN(4, ssize))) ==
	    (uint32 *)SYSERR ) ||
	    (pid=newpid()) == SYSERR || priority < 1 ) {
		kprintf("create error %s\n",name);
		restore(mask);
		return SYSERR;
	}
    

	prcount++;
	prptr = &proctab[pid];
    prptr->elf=FALSE;
	/* initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* initial state is suspended	*/
	prptr->prprio = priority;
	prptr->prstkbase = saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* set up initial device descriptors for the shell		*/
	prptr->prdesc[0] = CONSOLE;	/* stdin  is CONSOLE device	*/
	prptr->prdesc[1] = CONSOLE;	/* stdout is CONSOLE device	*/
	prptr->prdesc[2] = CONSOLE;	/* stderr is CONSOLE device	*/

    


	//va_start(ap, nargs);
    //prptr->prstkptr = setupStack(saddr, procaddr, INITRET, nargs, ap);
    //va_end(ap);
    
    va_start(ap, nargs);
    prptr->prstkptr = setupStack(saddr, procaddr, INITRET, nargs, ap);
    va_end(ap);
  
	restore(mask);
	return pid;
}

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
local	pid32	newpid(void)
{
	uint32	i;			/* iterate through all processes*/
//	static	pid32 nextpid = 1;	/* position in table to try or	*/
	static	pid32 nextpid = 0;	/* position in table to try or	*/
					/*  one beyond end of table	*/

	/* check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}
	return (pid32) SYSERR;
}



/*------------------------------------------------------------------------
 *  getpid  -  Return the ID of the currently executing process
 *------------------------------------------------------------------------
 */
pid32	getpid(void)
{
	return (currpid);
}

/*------------------------------------------------------------------------
 *  chprio  -  Change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
pri16	chprio(
	  pid32		pid,		/* ID of process to change	*/
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	oldprio;		/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16) SYSERR;
	}
	prptr = &proctab[pid];
	oldprio = prptr->prprio;
	prptr->prprio = newprio;
	restore(mask);
	return oldprio;
}

/*------------------------------------------------------------------------
 *  getprio  -  Return the scheduling priority of a process
 *------------------------------------------------------------------------
 */
syscall	getprio(
	  pid32		pid		/* Process ID			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	prio = proctab[pid].prprio;
	restore(mask);
	return prio;
}

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		//close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);
	//free(prptr->prstkbase);
    if(prptr->elf == TRUE){
    	//kprintf("clean space %d\n",prptr->prstklen);
        free(prptr->img);
        //freemem(prptr->img,prptr->size);
    }
	/*if(riscv1[pid].running){// si hay un vm activa
	   riscv1[pid].running=0;
       #if use_ram == 1
            free(riscv1[pid].memory);
       #endif
       riscv1[pid].prstate = PR_FREE;
       f_close(riscv1[pid].file);
	}*/

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 *  yield  -  Voluntarily relinquish the CPU (end a timeslice)
 *------------------------------------------------------------------------
 */
syscall	yield(void)
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
	resched();
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */
	//resched_cntl(DEFER_START);
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		ready(dequeue(sleepq), RESCHED_NO);
	}
    
    resched();
	//resched_cntl(DEFER_STOP);
	return;
}

/*------------------------------------------------------------------------
 *  wait  -  Cause current process to wait on a semaphore
 *------------------------------------------------------------------------
 */
syscall	wait(
	  sid32		sem		/* Semaphore on which to wait  */
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	struct	sentry *semptr;		/* Ptr to sempahore table entry	*/

	mask = disable();
	if (isbadsem(sem)) {
		restore(mask);
		return SYSERR;
	}

	semptr = &semtab[sem];
	if (semptr->sstate == S_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--(semptr->scount) < 0) {		/* If caller must block	*/
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;	/* Set state to waiting	*/
		prptr->prsem = sem;		/* Record semaphore ID	*/
		enqueue(currpid,semptr->squeue);/* Enqueue on semaphore	*/
		resched();
	}

	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  userret  -  Called when a process returns from the top-level function
 *------------------------------------------------------------------------
 */
void	userret(void)
{
	kill(getpid());			/* Force process to exit */
}

/*------------------------------------------------------------------------
 *  unsleep  -  Internal function to remove a process from the sleep
 *		    queue prematurely.  The caller must adjust the delay
 *		    of successive processes.
 *------------------------------------------------------------------------
 */
status	unsleep(
	  pid32		pid		/* ID of process to remove	*/
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
        struct	procent	*prptr;		/* Ptr to process's table entry	*/

        pid32	pidnext;		/* ID of process on sleep queue	*/
					/*   that follows the process	*/
					/*   which is being removed	*/

	mask = disable();

	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	/* Verify that candidate process is on the sleep queue */

	prptr = &proctab[pid];
	if ((prptr->prstate!=PR_SLEEP) && (prptr->prstate!=PR_RECTIM)) {
		restore(mask);
		return SYSERR;
	}

	/* Increment delay of next process if such a process exists */

	pidnext = queuetab[pid].qnext;
	if (pidnext < NPROC) {
		queuetab[pidnext].qkey += queuetab[pid].qkey;
	}

	getitem(pid);			/* Unlink process from queue */
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  suspend  -  Suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
syscall	suspend(
	  pid32		pid		/* ID of process to suspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)) {
		restore(mask);
		return SYSERR;
	}

	/* Only suspend a process that is current or ready */

	prptr = &proctab[pid];
	if ((prptr->prstate != PR_CURR) && (prptr->prstate != PR_READY)) {
		restore(mask);
		return SYSERR;
	}
	if (prptr->prstate == PR_READY) {
		getitem(pid);		    /* Remove a ready process	*/
					    /*   from the ready list	*/
		prptr->prstate = PR_SUSP;
	} else {
		prptr->prstate = PR_SUSP;   /* Mark the current process	*/
		resched();
	}
	prio = prptr->prprio;
	restore(mask);
	return prio;
}


#define	MAXSECONDS	2147483		/* Max seconds per 32-bit msec	*/

/*------------------------------------------------------------------------
 *  sleep  -  Delay the calling process n seconds
 *------------------------------------------------------------------------
 */
syscall	sleep(
	  int32	delay		/* Time to delay in seconds	*/
	)
{
	if ( (delay < 0) || (delay > MAXSECONDS) ) {
		return SYSERR;
	}
	sleepms(1000*delay);
	return OK;
}

/*------------------------------------------------------------------------
 *  sleepms  -  Delay the calling process n milliseconds
 *------------------------------------------------------------------------
 */
syscall	sleepms(
	  int32	delay			/* Time to delay in msec.	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/

	if (delay < 0) {
		return SYSERR;
	}

	if (delay == 0) {
		yield();
		return OK;
	}

	/* Delay calling process */

	mask = disable();
	if (insertd(currpid, sleepq, delay) == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	proctab[currpid].prstate = PR_SLEEP;
	resched();	
	
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{

	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];
	if (prptr->prstate != PR_SUSP) {
		restore(mask);
		return (pri16)SYSERR;
	}
	prio = prptr->prprio;		/* Record priority to return	*/
	ready(pid,RESCHED_YES);
	restore(mask);
	return prio;
}

struct	defer	Defer;

/* Reschedule is implemented on Cortex's PendSV call
 * The __attribute__((naked)) makes sure the compiler doesn't
 * places registers on the stack
 *
 * A PendSV is similar to an exception or interrupt
 * A stack frame is pushed on entry and popped
 * on exit BY PROCESSOR. */

/* A pendSV call before entering this handler
 * places the following on the stack 
 * 
 * R0 <- args[0]
 * R1 <- args[1]
 * R2 
 * R3
 * R12
 * LR <- Not a real LR, A certain 'mask'
 * PC 
 * PSR <- Status of processor before call
 *
 * */

void save_context(context_t* ctx) {
	asm volatile(".option arch, +zicsr");
    asm volatile (
        "addi sp, sp, -4*23\n"
        "sw a7, 4*22(sp)\n"
        "sw a6, 4*21(sp)\n"
        "sw a5, 4*20(sp)\n"
        "sw a4, 4*19(sp)\n"
        "sw a3, 4*18(sp)\n"
        "sw a2, 4*17(sp)\n"
        "sw a1, 4*16(sp)\n"
        "sw a0, 4*15(sp)\n"
        "sw s11, 4*14(sp)\n"
        "sw s10, 4*13(sp)\n"
        "sw s9, 4*12(sp)\n"
        "sw s8, 4*11(sp)\n"
        "sw s7, 4*10(sp)\n"
        "sw s6, 4*9(sp)\n"
        "sw s5, 4*8(sp)\n"
        "sw s4, 4*7(sp)\n"
        "sw s3, 4*6(sp)\n"
        "sw s2, 4*5(sp)\n"
        "sw s1, 4*4(sp)\n"
        "sw s0, 4*3(sp)\n"
        "csrr t2, mstatus\n"
        "sw t2, 4*2(sp)\n"
        "sw ra, 4*1(sp)\n"
        "sw sp, 0(%0)\n"
        : : "r" (ctx)
    );
}

void load_context(context_t* ctx) {
	asm volatile(".option arch, +zicsr");
    asm volatile (
        "lw sp, 0(%0)\n"
        "lw t0, 0(sp)\n"
        "lw ra, 4*1(sp)\n"
        "lw t2, 4*2(sp)\n"
        "lw s0, 4*3(sp)\n"
        "lw s1, 4*4(sp)\n"
        "lw s2, 4*5(sp)\n"
        "lw s3, 4*6(sp)\n"
        "lw s4, 4*7(sp)\n"
        "lw s5, 4*8(sp)\n"
        "lw s6, 4*9(sp)\n"
        "lw s7, 4*10(sp)\n"
        "lw s8, 4*11(sp)\n"
        "lw s9, 4*12(sp)\n"
        "lw s10, 4*13(sp)\n"
        "lw s11, 4*14(sp)\n"
        "lw a0, 4*15(sp)\n"
        "lw a1, 4*16(sp)\n"
        "lw a2, 4*17(sp)\n"
        "lw a3, 4*18(sp)\n"
        "lw a4, 4*19(sp)\n"
        "lw a5, 4*20(sp)\n"
        "lw a6, 4*21(sp)\n"
        "lw a7, 4*22(sp)\n"
        "addi sp, sp, 4*23\n"
        "andi t3, t2, 0x8\n"  // MSTATUS_MIE = 0x8
        "beq t3, x0, 1f\n"
        "andi t2, t2, ~0x8\n"
        "csrw mstatus, t2\n"
        "csrw mepc, t0\n"
        "mret\n"
        "1:\n"
        "jalr x0, t0, 0\n"
        : : "r" (ctx)
    );
}


extern void ctxsw(void *, void *, uint8);
int resdefer; 


void  resched(void){
    
    struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
    intmask mask;
    uint8 asid;
	/*if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}*/
	if (resdefer > 0)
    {                           /* if deferred, increase count & return */
        resdefer++;
        return;
    }
   
    ptold = &proctab[currpid];
    mask = disable();
    if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			restore(mask);
			return;
		}
		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
    asid = currpid & 0xff;
    ctxsw(&ptold->prstkptr, &ptnew->prstkptr, asid);
    /* old thread returns here when resumed */
    restore(mask);
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
#if 0
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
#endif
qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid,		/* ID of process to make ready	*/
	 bool resch
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	
	if (resch == RESCHED_YES)
    {
        resched();
    }

	return OK;
}

/*------------------------------------------------------------------------
 *  exit  -  Cause the calling process to exit
 *------------------------------------------------------------------------
 */
void	exit(void)
{
	kill(getpid());		/* Kill the current process */
}
