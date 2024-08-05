/* process.h - isbadpid */

/* Maximum number of processes in the system */

#include <stdbool.h>

#ifndef NPROC
#define	NPROC		8
#endif		

#define MAX_ARG 4

struct context {
  uint32 ra;
  uint32 sp;

  // callee-saved
  uint32 s0;
  uint32 s1;
  uint32 s2;
  uint32 s3;
  uint32 s4;
  uint32 s5;
  uint32 s6;
  uint32 s7;
  uint32 s8;
  uint32 s9;
  uint32 s10;
  uint32 s11;
};

typedef struct rt_hw_stack_frame
{
    uint32 epc;        /* epc - epc    - program counter                     */
    uint32 ra;         /* x1  - ra     - return address for jumps            */
    uint32 mstatus;    /*              - machine status register             */
    uint32 gp;         /* x3  - gp     - global pointer                      */
    uint32 tp;         /* x4  - tp     - thread pointer                      */
    uint32 t0;         /* x5  - t0     - temporary register 0                */
    uint32 t1;         /* x6  - t1     - temporary register 1                */
    uint32 t2;         /* x7  - t2     - temporary register 2                */
    uint32 s0_fp;      /* x8  - s0/fp  - saved register 0 or frame pointer   */
    uint32 s1;         /* x9  - s1     - saved register 1                    */
    uint32 a0;         /* x10 - a0     - return value or function argument 0 */
    uint32 a1;         /* x11 - a1     - return value or function argument 1 */
    uint32 a2;         /* x12 - a2     - function argument 2                 */
    uint32 a3;         /* x13 - a3     - function argument 3                 */
    uint32 a4;         /* x14 - a4     - function argument 4                 */
    uint32 a5;         /* x15 - a5     - function argument 5                 */
    uint32 a6;         /* x16 - a6     - function argument 6                 */
    uint32 a7;         /* x17 - a7     - function argument 7                 */
    uint32 s2;         /* x18 - s2     - saved register 2                    */
    uint32 s3;         /* x19 - s3     - saved register 3                    */
    uint32 s4;         /* x20 - s4     - saved register 4                    */
    uint32 s5;         /* x21 - s5     - saved register 5                    */
    uint32 s6;         /* x22 - s6     - saved register 6                    */
    uint32 s7;         /* x23 - s7     - saved register 7                    */
    uint32 s8;         /* x24 - s8     - saved register 8                    */
    uint32 s9;         /* x25 - s9     - saved register 9                    */
    uint32 s10;        /* x26 - s10    - saved register 10                   */
    uint32 s11;        /* x27 - s11    - saved register 11                   */
    uint32 t3;         /* x28 - t3     - temporary register 3                */
    uint32 t4;         /* x29 - t4     - temporary register 4                */
    uint32 t5;         /* x30 - t5     - temporary register 5                */
    uint32 t6;         /* x31 - t6     - temporary register 6                */
}rt_hw_stack_frame_t;

#include <stdint.h>

typedef struct {
    uint32_t a7;
    uint32_t a6;
    uint32_t a5;
    uint32_t a4;
    uint32_t a3;
    uint32_t a2;
    uint32_t a1;
    uint32_t a0;
    uint32_t s11;
    uint32_t s10;
    uint32_t s9;
    uint32_t s8;
    uint32_t s7;
    uint32_t s6;
    uint32_t s5;
    uint32_t s4;
    uint32_t s3;
    uint32_t s2;
    uint32_t s1;
    uint32_t s0;
    uint32_t mstatus;
    uint32_t ra;
    uint32_t sp;
} context_t;



#define RESCHED_YES 1           /**< tell ready to reschedule           */
#define RESCHED_NO  0           /**< tell ready not to reschedule       */


/* Process state constants */

#define	PR_FREE		0	/* Process table entry is unused	*/
#define	PR_CURR		1	/* Process is currently running		*/
#define	PR_READY	2	/* Process is on ready queue		*/
#define	PR_RECV		3	/* Process waiting for message		*/
#define	PR_SLEEP	4	/* Process is sleeping			*/
#define	PR_SUSP		5	/* Process is suspended			*/
#define	PR_WAIT		6	/* Process is on semaphore queue	*/
#define	PR_RECTIM	7	/* Process is receiving with timeout	*/

/* Miscellaneous process definitions */

#define	PNMLEN		20	/* Length of process "name"		*/
#define	NULLPROC	0	/* ID of the null process		*/

/* Process initialization constants */

#define	INITSTK		4096/2	/* Initial process stack size		*/
#define	INITPRIO	20	/* Initial process priority		*/
#define	INITRET		userret	/* Address to which process returns	*/

/* Inline code to check process ID (assumes interrupts are disabled)	*/

#define	isbadpid(x)	( ((pid32)(x) < 0) || \
			  ((pid32)(x) >= NPROC) || \
			  (proctab[(x)].prstate == PR_FREE))

/* Number of device descriptors a process can have open */

#define NDESC		3	/* must be odd to make procent 4N bytes	*/

/* Definition of the process table (multiple of 32 bits) */

struct procent {		/* Entry in the process table		*/
	uint16	prstate;	/* Process state: PR_CURR, etc.		*/
	pri16	prprio;		/* Process priority			*/
	void	*prstkptr;	/* Saved stack pointer			*/
	void	*prstkbase;	/* Base of run time stack		*/
	uint32	prstklen;	/* Stack length in bytes		*/
	char	prname[PNMLEN];	/* Process name				*/
	sid32	prsem;		/* Semaphore on which process waits	*/
	pid32	prparent;	/* ID of the creating process		*/
	umsg32	prmsg;		/* Message sent to this process		*/
	bool8	prhasmsg;	/* Nonzero iff msg is valid		*/
	int16	prdesc[NDESC];	/* Device descriptors for process	*/
  bool8 	elf;
  void *img;
  uint32 size;
  //struct context ctx_task;
  //char parg[5][16];//
  void *parg[MAX_ARG]; 
};

/* Marker for the top of a process stack (used to help detect overflow)	*/
#define	STACKMAGIC	0x0A0AAAA9

extern	struct	procent proctab[];
extern	int32	prcount;	/* Currently active processes		*/
extern	pid32	currpid;	/* Currently executing process		*/
extern bool start_null;