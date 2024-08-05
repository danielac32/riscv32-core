/* memory.h - roundmb, truncmb, freestk */
#include <kernel.h>
#define MAXADDR		0x2001f400		/* 64kB SRAM */
#define MAXADDRCC		0x10010000		/* 64kB SRAM */
#define HANDLERSTACK	4096			/* Size reserved for stack in Handler mode */
#define	PAGE_SIZE	1024 // TODO: unused?

/*----------------------------------------------------------------------
 * roundmb, truncmb - Round or truncate address to memory block size
 *----------------------------------------------------------------------
 */
#define	roundmb(x)	(char *)( (7 + (uint32)(x)) & (~7) )
#define	truncmb(x)	(char *)( ((uint32)(x)) & (~7) )

/*----------------------------------------------------------------------
 *  freestk  --  Free stack memory allocated by getstk
 *----------------------------------------------------------------------
 */
#define	freestk(p,len)	freemem((char *)((uint32)(p)		\
				- ((uint32)roundmb(len))	\
				+ (uint32)sizeof(uint32)),	\
				(uint32)roundmb(len) )

struct	memblk	{			/* See roundmb & truncmb	*/
	struct	memblk	*mnext;		/* Ptr to next free memory blk	*/
	uint32	mlength;		/* Size of blk (includes memblk)*/
	};
extern	struct	memblk	memlist;	/* Head of free memory list	*/
extern	void	*minheap;		/* Start of heap		*/
extern	void	*maxheap;		/* Highest valid heap address	*/

 

/* Added by linker */

extern	unsigned char	_text;			/* Start of text segment	*/
extern	unsigned char	_etext;			/* End of text segment		*/
extern	unsigned char	_sidata;
extern	unsigned char	_sdata;			/* Start of data segment	*/
extern	unsigned char	_edata;			/* End of data segment		*/
extern	unsigned char	_sbss;			/* Start of bss segment		*/
extern	unsigned char	_ebss;			/* End of bss segment		*/
extern	unsigned int	_end;			/* End of program		*/
uint32 heap_free(void);
uint32 heap_freecc();


#define ALIGN(to, x)  ((x+(to)-1) & ~((to)-1))