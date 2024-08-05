 

/* RISC-V specific values and macros
 *
 * Derived from the RISC-V specification and from the spike (riscv-isa-sim) sources
 * (the latter Copyright (c) 2010-2017, The Regents of the University of California)
 *
 */
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software

/* Machine status register bits */
#define MSTATUS_UIE         0x00000001
#define MSTATUS_SIE         0x00000002
#define MSTATUS_HIE         0x00000004
#define MSTATUS_MIE         0x00000008
#define MSTATUS_UPIE        0x00000010
#define MSTATUS_SPIE        0x00000020
#define MSTATUS_HPIE        0x00000040
#define MSTATUS_MPIE        0x00000080
#define MSTATUS_SPP         0x00000100
#define MSTATUS_HPP         0x00000600
#define MSTATUS_MPP         0x00001800
#define MSTATUS_FS          0x00006000
#define MSTATUS_XS          0x00018000
#define MSTATUS_MPRV        0x00020000
#define MSTATUS_SUM         0x00040000
#define MSTATUS_MXR         0x00080000
#define MSTATUS_TVM         0x00100000
#define MSTATUS_TW          0x00200000
#define MSTATUS_TSR         0x00400000
#define MSTATUS32_SD        0x80000000
#define MSTATUS_UXL         0x0000000300000000
#define MSTATUS_SXL         0x0000000C00000000
#define MSTATUS64_SD        0x8000000000000000

#if __riscv_xlen == 64
# define MSTATUS_SD MSTATUS64_SD
#else
# define MSTATUS_SD MSTATUS32_SD
#endif


/* MIP and MIE registers bits */
#define MIP_SSIP            (1 << IRQ_S_SOFT)
#define MIP_MSIP            (1 << IRQ_M_SOFT)
#define MIP_STIP            (1 << IRQ_S_TIMER)
#define MIP_MTIP            (1 << IRQ_M_TIMER)
#define MIP_SEIP            (1 << IRQ_S_EXT)
#define MIP_MEIP            (1 << IRQ_M_EXT)

/* Privilege modes */
#define PRV_U 0
#define PRV_S 1
#define PRV_H 2
#define PRV_M 3

/* Interrupt bits for MIE, MIP and delegation registers */
#define IRQ_S_SOFT   1
#define IRQ_M_SOFT   3
#define IRQ_S_TIMER  5
#define IRQ_M_TIMER  7
#define IRQ_S_EXT    9
#define IRQ_M_EXT    11

/* CSR access macros */
#define write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define read_csr(reg) ({ unsigned long __tmp;	 \
      asm volatile ("csrr %0, " #reg : "=r"(__tmp));	\
      __tmp; })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
  asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "rK"(val)); \
  __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

/*  Size of context to be saved: number of registers * bytes per reg */
#define REGBYTES (__riscv_xlen / 8)
#define  RISCV_CONTEXT_SIZE (32*REGBYTES)

#define MTIM 7
#define MSIT 3
#define MEIT 11
#define timel *(unsigned int*)0x11004000
#define timeh *(unsigned int*)0x11004004

#define CLINT_BASE_ADDR 0x11000000
#define CLINT_MTIMECMP_OFFSET 0x4000
#define CLINT_MTIME_OFFSET 0xBFF8
//extern unsigned int SYSCON;
//extern unsigned int reset;
 

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


#ifndef RISCV_MACROS_H
#define RISCV_MACROS_H

#define csr_put(c) \
    asm volatile("csrw 0x402, %0" : : "r"(c))

#define UART (*(volatile char *)0x10000000L)

#define r_mhartid() ({ \
    unsigned int x; \
    asm volatile("csrr %0, mhartid" : "=r"(x)); \
    x; \
})

#define r_mstatus() ({ \
    unsigned int x; \
    asm volatile(".option arch, +zicsr"); \
    asm volatile("csrr %0, mstatus" : "=r"(x)); \
    x; \
})

#define w_mstatus(x) \
    asm volatile("csrw mstatus, %0" : : "r"(x))

#define w_mepc(x) \
    asm volatile("csrw mepc, %0" : : "r"(x))

#define r_mepc() ({ \
    unsigned int x; \
    asm volatile("csrr %0, mepc" : "=r"(x)); \
    x; \
})

#define w_mscratch(x) \
    asm volatile("csrw mscratch, %0" : : "r"(x))

#define w_mtvec(x) \
    asm volatile("csrw mtvec, %0" : : "r"(x))

#define csrr() ({ \
    unsigned int x; \
    asm volatile("csrr %0, mie" : "=r"(x)); \
    x; \
})

#define r_mie() ({ \
    unsigned int x; \
    asm volatile("csrr %0, mie" : "=r"(x)); \
    x; \
})

#define r_mip() ({ \
    unsigned int x; \
    asm volatile("csrr %0, mip" : "=r"(x)); \
    x; \
})

#define w_mie(x) \
    asm volatile("csrw mie, %0" : : "r"(x))

#define w_mip(x) \
    asm volatile("csrw mip, %0" : : "r"(x))

#define get_cyc_count() ({ \
    asm volatile(".option arch, +zicsr"); \
    unsigned int ccount; \
    asm volatile("csrr %0, 0xC00" : "=r"(ccount)); \
    ccount; \
})

#endif // RISCV_MACROS_H
 