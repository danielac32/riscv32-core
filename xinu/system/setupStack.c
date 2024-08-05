/**
 * @file setupStack.c
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <rv.h>

#define CONTEXT_WORDS 23

/* First 8 arguments pass via a0-a7; the rest spill onto the stack.  */
#define MAX_REG_ARGS 8

/** Set up the context record and arguments on the stack for a new thread */
void *setupStack(void *stackaddr, void *procaddr,
                 void *retaddr, uint32 nargs, va_list ap)
{
    uint32 spilled_nargs;
    uint32 *saddr = (uint32 *)stackaddr;

    /* Determine if any arguments will spill onto the stack (outside the context
     * record).  If so, reserve space for them.  */
    if (nargs > MAX_REG_ARGS) {
        spilled_nargs = nargs - MAX_REG_ARGS;
        saddr -= spilled_nargs;
    } else {
        spilled_nargs = 0;
    }

    /* Construct the context record for the new thread.  */
    saddr -= CONTEXT_WORDS;

    /* Possibly skip a word to ensure the stack is aligned on 16-byte boundary
     * after the new thread pops off the context record.  */
    while ((uint32)saddr & 0xf)
    {
        saddr = (uint32 *)((uint32)saddr - 1);
    }
    uint32 i = 0;
    /* Arguments passed in registers (part of context record)  */
    /* program counter */
    saddr[i++] = (uint32)procaddr;
    /* x1 - return address */
    saddr[i++] = (uint32)retaddr;
    /* Interrupt enabled flag in mstatus set as well previous
     * privilege mode to machine mode and previous interrupt enabled
     * flag to true so mret could be used to properly restore the
     * machine status as well as to jump to newly created thread
     */
    saddr[i++] = MSTATUS_MIE | MSTATUS_MPP | MSTATUS_MPIE;
    /* s0 - s11 */
    for (; i < 15; i++)
    {
        saddr[i] = 0;
    }
    
    /* register arguments (as part of context) and rest of arguments
     * that did not fit into the registers  */
    for (int j = 0; j < nargs; j++)
    {
        saddr[i++] = va_arg(ap, uint32);
    }

    /* rest of a registers (if any) */
    for (; i < CONTEXT_WORDS; i++)
    {
        saddr[i] = 0;
    }

    /* Return "top" of stack (lowest address). */
    return saddr;
}
