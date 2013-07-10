/*
 * cside.c:  A main C program calling the A# function `lcm'.
 */
#include "foam_c.h"

extern FiSInt   lcm     (FiSInt, FiSInt);

int
main()
{
    printf("The lcm of 6 and 4 is %d\n", lcm(6,4));
    return 0;
}
