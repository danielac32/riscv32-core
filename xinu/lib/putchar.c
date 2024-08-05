/* putchar.c - putchar */
#if 0
#include <xinu.h>
//#include <stdio.h>

/*------------------------------------------------------------------------
 *  putchar  -  C putchar function
 *------------------------------------------------------------------------
 */
int	putchar(
	  int		c
	)
{
    return fputc(c, stdout);
}
#endif