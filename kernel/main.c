#include <stdint.h>
#define MTIM 7
#define MSIT 3
#define MEIT 11
#define timel *(unsigned int*)0x11004000
#define timeh *(unsigned int*)0x11004004

#define CLINT_BASE_ADDR 0x11000000
#define CLINT_MTIMECMP_OFFSET 0x4000
#define CLINT_MTIME_OFFSET 0xBFF8

volatile uint64_t *mtimecmp = (uint64_t *)(CLINT_BASE_ADDR + CLINT_MTIMECMP_OFFSET);
volatile uint64_t *mtime = (uint64_t *)(CLINT_BASE_ADDR + CLINT_MTIME_OFFSET);

#define INTERVALCLOCK 100

#define kbhit() *(unsigned char*)0x10000005
#define readbyte() *(unsigned char*)0x10000000
extern unsigned int SYSCON;
extern unsigned int reset;
extern void enable_interrupt_status();
extern void enable_interrupt(unsigned int num);
extern void disable_interrupt_status();
extern void disable_interrupt(unsigned int num);
extern void clear_interrupts();
extern void soft_trap();
#define UART *(char *)0x10000000L



#define MSTATUS_MPIE        0x00000080
// Machine Status Register, mstatus
#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3) // machine-mode interrupt enable.

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software



static inline void timer_trap(unsigned int x)
{
  asm volatile("csrw 0x403, %0"
               :
               : "r"(x));
}
static inline void csr_put(unsigned int c)
{
  asm volatile("csrw 0x402, %0"
               :
               : "r"(c));
}
 
void kputs(char *str){
	while(*str){
		UART = *str++;
		///csr_put(*str++);
	}
}


static inline void upload_fs(unsigned int x)
{
  asm volatile("csrw 0x401, %0"
               :
               : "r"(x));
}
 
 
static inline void download_fs(unsigned int x)
{
  asm volatile("csrw 0x400, %0"
               :
               : "r"(x));
}
static inline unsigned int r_mhartid()
{
  unsigned int x;
  asm volatile("csrr %0, mhartid"
               : "=r"(x));
  return x;
}

static inline unsigned int r_mstatus()
{
  unsigned int x;
  asm volatile(".option arch, +zicsr");
  asm volatile("csrr %0, mstatus"
               : "=r"(x));
  return x;
}

static inline void w_mstatus(unsigned int x)
{
  asm volatile("csrw mstatus, %0"
               :
               : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void w_mepc(unsigned int x)
{
  asm volatile("csrw mepc, %0"
               :
               : "r"(x));
}

static inline unsigned int r_mepc()
{
  unsigned int x;
  asm volatile("csrr %0, mepc"
               : "=r"(x));
  return x;
}

// Machine Scratch register, for early trap handler
static inline void w_mscratch(unsigned int x)
{
  asm volatile("csrw mscratch, %0"
               :
               : "r"(x));
}

// Machine-mode interrupt vector
static inline void w_mtvec(unsigned int x)
{
  asm volatile("csrw mtvec, %0"
               :
               : "r"(x));
}

static inline unsigned int csrr()
{
  unsigned int x;
  asm volatile("csrr %0, mie"
               : "=r"(x));
  return x;
}

static inline unsigned int r_mie()
{
  unsigned int x;
  asm volatile("csrr %0, mie"
               : "=r"(x));
  return x;
}

static inline unsigned int r_mip()
{
  unsigned int x;
  asm volatile("csrr %0, mip"
               : "=r"(x));
  return x;
}


static inline void w_mie(unsigned int x)
{
  asm volatile("csrw mie, %0"
               :
               : "r"(x));
}

static inline void w_mip(unsigned int x)
{
  asm volatile("csrw mip, %0"
               :
               : "r"(x));
}


static inline unsigned int get_cyc_count() {
  unsigned int ccount;
  asm volatile("csrr %0, 0xC00":"=r" (ccount));
  return ccount;
}

#define csr_read(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define csr_write(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define csr_set(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define csr_clear(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define csr_swap(reg, val) ({ \
    unsigned long __v = (unsigned long)(val); \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r" (__v) : "rK" (__v) : "memory"); \
    __v; })



#define REG_RA                         1
#define REG_SP                         2
#define REG_ARG0                       10
#define REG_RET                        REG_ARG0
#define NUM_GP_REG                     32
#define NUM_CSR_REG                    3

#define CAUSE_MISALIGNED_FETCH         0
#define CAUSE_FAULT_FETCH              1
#define CAUSE_ILLEGAL_INSTRUCTION      2
#define CAUSE_BREAKPOINT               3
#define CAUSE_MISALIGNED_LOAD          4
#define CAUSE_FAULT_LOAD               5
#define CAUSE_MISALIGNED_STORE         6
#define CAUSE_FAULT_STORE              7
#define CAUSE_ECALL_U                  8
#define CAUSE_ECALL_S                  9
#define CAUSE_ECALL_M                  11
#define CAUSE_PAGE_FAULT_INST          12
#define CAUSE_PAGE_FAULT_LOAD          13
#define CAUSE_PAGE_FAULT_STORE         15
#define CAUSE_INTERRUPT                (1 << 31)


static inline unsigned int csr_get_time(void)
{
    unsigned int value;
    asm volatile(".option arch, +zicsr");
    asm volatile ("csrr %0, time" : "=r" (value) : );
    return value;
}

struct irq_context
{
    unsigned int pc;
    unsigned int status;
    unsigned int cause;
    unsigned int reg[NUM_GP_REG];
};
void    exception_dump_ctx(struct irq_context *ctx);
struct irq_context * exceptionHandler(struct irq_context *ctx);
struct irq_context *do_syscall(struct irq_context *ctx);




static inline __attribute__((always_inline)) unsigned int irq_enable(void)
{
    /* Enable all interrupts */
    unsigned state;

    __asm__ volatile (
        "csrrs %[dest], mstatus, %[mask]"
        :[dest]    "=r" (state)
        :[mask]    "i" (MSTATUS_MIE)
        : "memory"
        );
    return state;
}


static inline __attribute__((always_inline)) unsigned int irq_disable(void)
{

    unsigned int state;

    __asm__ volatile (
        "csrrc %[dest], mstatus, %[mask]"
        :[dest]    "=r" (state)
        :[mask]    "i" (MSTATUS_MIE)
        : "memory"
        );

    return state;
}


static inline __attribute__((always_inline)) void irq_restore(
    unsigned int state)
{
    /* Restore all interrupts to given state */
    __asm__ volatile (
        "csrw mstatus, %[state]"
        : /* no outkputs */
        :[state]   "r" (state)
        : "memory"
        );
}


#define reg_t unsigned int // RISCV32: register is 32bits
#define MAX_TASK 10
#define STACK_SIZE 1024


struct context {
  reg_t ra;
  reg_t sp;

  // callee-saved
  reg_t s0;
  reg_t s1;
  reg_t s2;
  reg_t s3;
  reg_t s4;
  reg_t s5;
  reg_t s6;
  reg_t s7;
  reg_t s8;
  reg_t s9;
  reg_t s10;
  reg_t s11;
};

uint8_t task_stack[MAX_TASK][STACK_SIZE];
struct context ctx_os;
struct context ctx_tasks[MAX_TASK];
struct context *ctx_now;
int taskTop=0;  // total number of task


extern void sys_switch(struct context *ctx_old, struct context *ctx_new);

// create a new task
int task_create(void (*task)(void))
{
  int i=taskTop++;
  ctx_tasks[i].ra = (reg_t) task;
  ctx_tasks[i].sp = (reg_t) &task_stack[i][STACK_SIZE-1];
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





void init_timer(uint64_t interval) {
    uint64_t current_time = *mtime;
    *mtimecmp = current_time + interval;
  //timel=interval;
  //timeh=0;
}


/* clkcount() interface is documented in clock.h  */
uint64_t clkcount(void)
{
    return *mtime;
}

/* clkupdate() interface is documented in clock.h  */
void clkupdate(uint64_t cycles)
{
    *mtimecmp = (*mtime) + cycles;
}





void clkhandler(void){
  clkupdate(INTERVALCLOCK);
  task_os();
  //kputs("timer ok\n");
}

struct irq_context * exceptionHandler(struct irq_context *ctx)
{
   


     
	  unsigned int cause_code = ctx->cause & 0xfff;
 
		if (ctx->cause & CAUSE_INTERRUPT){
        switch (cause_code)
		    {
		    case 3:
            kputs("software interruption! \n");
		      //kprintf("software interruption!\n");
		      //ctx->pc =(uint32)&resched;
		      //ctx->pc += 4;
		      break;
		    case 7:
          /*w_mie(~((~r_mie()) | (1 << 7)));
           kputs("timer ok\n");

		     // if(get_cyc_count() >= count + 0x100000){
	           //   count = get_cyc_count();
	             // kprintf("timer ok\n");
	              //ctx->pc = (uint32)&clkhandler;
              //} 

		      
              //sys_time += 1;
              //ctx->pc = (uint32)&clkhandler;
		       w_mie(r_mie() | MIE_MTIE);*/
            asm volatile(".option arch, +zicsr");
            w_mie(~((~r_mie()) | (1 << 7)));
            ctx->pc = (unsigned int)&clkhandler;
            w_mie(r_mie() | MIE_MTIE);
		      break;
		    case 11:
		      //kprintf("external interruption!\n");
		      break;
		    default:
		      //kprintf("unknown async exception!\n");
		      break;
		    }
    	}else{
    		switch (cause_code){
    		case 1:
    			{
    				//uint32_t val;
                 //   asm volatile(".option norvc\ncsrr %0, 0x343":"=r" (val));
    				//kprintf("CAUSE_FAULT_FETCH %08x\n",val);
    				//exception_dump_ctx(ctx);
    			}
    			//ctx->pc += 1;
    			break;
		    	case 2:
		      //kprintf("Illegal instruction!\n");
		      //exception_dump_ctx(ctx);
		      break;
		    case 5:
		      //kprintf("Fault load!\n");
		      //exception_dump_ctx(ctx);
		      break;
		    case 7:
		      //kprintf("Fault store!\n");
		      //exception_dump_ctx(ctx);
		      break;
		    case 8:
		    case 11:
		        
		        //ctx=do_syscall(ctx);
		         
		      break;
		    default:
		        //kprintf("Sync exceptions! cause code: %d\n", cause_code);
		        //exception_dump_ctx(ctx);
		      break;
      }
    }
return ctx;
}


void asus2(int n){
int c,k,space=1;
space = n -1;
for (k = 1; k <= n; k++)
{
	for (c=1;c<=space;c++)
		kputs(" ");
	 
	space--;
    
    for (c=1;c<=2*(k-1);c++)
		kputs("*");

    kputs("\n");
}
space =1;
for (k = 1; k <= n-1; ++k)
{
	for (c=1;c<=space;c++)
		kputs(" ");
	 
	space++;
    
    for (c=1;c<=2*(n-k)-1;c++)
		kputs("*");

    kputs("\n");
}

}
void asus(int num){
for (int i = 1; i <= num; ++i)
{
	for (int j = 0; j <= num; ++j)
	{
		if( i==1 || i==num || j==1 || j == num){
             kputs("* ");
		}else{
             kputs("  ");
		}
	}
	kputs("\n");
}
}
void lib_delay(volatile int count)
{
  count *= 50000;
  while (count--);
}


void user_task0(void)
{
  kputs("Task0: Created!\n");
  while (1)
  {
    kputs("Task0: Running...\n");
    
  }
}

void user_task1(void)
{
  kputs("Task1: Created!\n");
  while (1)
  {
    kputs("Task1: Running...\n");
     
  }
}



int main(){
 
   kputs("daniel quintero \n");
   asus(20);
   asus2(20);
   
   kputs("riscv emulator\n");
  //init_timer(1000000);
 task_create(&user_task0);
  task_create(&user_task1);
    // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  w_mie(r_mie() | MIE_MSIE);
  w_mie(r_mie() | MIE_MTIE);
  init_timer(INTERVALCLOCK);

 

  int current_task = 0;
  while (1)
  {
    ///kputs("OS: Activate next task\n");
    task_go(current_task);
    //kputs("OS: Back to OS\n");
    current_task = (current_task + 1) % taskTop; // Round Robin Scheduling
    //kputs("\n");
  }


 // timel=1;
   //soft_trap();

   while(1){
 
   }
 
	SYSCON = 0x5555;
}