
#include <malloc.h>

/* Quick hack (martin@nag.co.uk) */

int main(void)
{
   long *p;
   long i, block, bsize, size, blocks;

   /*
    * We want to allocate 140Mb of store because my laptop has 160Mb
    * and the OS will be using around 20Mb. No sense in getting swapped.
    */
   size = 140*1024*1024;
   p = (long *)malloc(size);


   /* See what we can manage */
   while (!p)
   {
      /* Try for half as much */
      size = size >> 1;
      p = (long *)malloc(size);
   }


   /* Tell the user how much we got */
   (void)printf("Allocated: %dMb\n\n", size/(1024*1024));


   /* How many 10Mb blocks is this? */
   bsize  = 10*1024*1024;
   blocks = size/bsize;


   /* Initialise this store with zeros */
   for (block = 0;block < blocks;block++)
   {
      /* Clear the whole block */
      for (i = 0;i < bsize;i += sizeof(long))
         *p++ = 0L;


      /* Work out how much is left to clear */
      size -= bsize;


      /* Show progress every block */
      (void)printf("Cleaned:   %dMb\n", (block + 1)*10);
   }


   /* Clear the remaining partial block (if any) */
   for (i = 0;i < size;i += sizeof(long))
      *p++ = 0L;


   /* Finished */
   return 0;
}

