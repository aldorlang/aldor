/* memory.c -- Memory areas */

/* 
   Copyright (C) 1996 Giuseppe Attardi.

   This file is part of the POSSO Customizable Memory Manager (CMM).

   CMM is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   See file 'Copyright' for full details.

*/

#include <stdio.h>
#include "machine.h"
/*#include "memory.h"*/
#include <assert.h>

Word stackBottom;	/* The base of the stack	*/

void
CmmSetStackBottom(Word bottom)
{
#   ifdef STACKBOTTOM
	stackBottom = (Word) STACKBOTTOM;
#   else
#     define STACKBOTTOM_ALIGNMENT_M1 0xffffff
#     ifdef STACK_GROWS_DOWNWARD
      stackBottom = (bottom + STACKBOTTOM_ALIGNMENT_M1)
	& ~STACKBOTTOM_ALIGNMENT_M1;
#     else
      stackBottom = bottom & ~STACKBOTTOM_ALIGNMENT_M1;
#     endif
#   endif
}

/*---------------------------------------------------------------------------*
 * -- MS Windows
 *---------------------------------------------------------------------------*/
#ifdef __WIN32__

/* Code contributed by H. Boehm of Xerox PARC */
/*
 * Copyright (c) 1991-1994 by Xerox Corporation.  All rights reserved.
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

#include <windows.h>


/* Get the page size.	*/
unsigned long CmmPageSize = 0;

unsigned long
CmmGetPageSize(void)
{
    SYSTEM_INFO sysinfo;

    if (CmmPageSize == 0) {
        GetSystemInfo(&sysinfo);
        CmmPageSize = sysinfo.dwPageSize;
    }
    return(CmmPageSize);
}

# define is_writable(prot) ((prot) == PAGE_READWRITE \
			    || (prot) == PAGE_WRITECOPY \
			    || (prot) == PAGE_EXECUTE_READWRITE \
			    || (prot) == PAGE_EXECUTE_WRITECOPY)
/* Return the number of bytes that are writable starting at p.	*/
/* The pointer p is assumed to be page aligned.			*/
unsigned long
CmmGetWritableLength(char *p)
{
    MEMORY_BASIC_INFORMATION buf;
    unsigned long result;
    unsigned long protect;
    
    result = VirtualQuery(p, &buf, sizeof(buf));
    if (result != sizeof(buf)) {
      fprintf(stderr, "Weird VirtualQuery result\n");
      exit(-1);
    }
    protect = (buf.Protect & ~(PAGE_GUARD | PAGE_NOCACHE));
    if (!is_writable(protect)) {
        return(0);
    }
    if (buf.State != MEM_COMMIT) return(0);
    return(buf.RegionSize);
}

Ptr
CmmGetStackBase(void)
{
    int dummy;
    char *sp = (char *)(&dummy);
    char *trunc_sp = (char *)((unsigned long)sp & ~(CmmGetPageSize() - 1));
    unsigned long size = CmmGetWritableLength(trunc_sp);
   
    return (Ptr) (trunc_sp + size);
}


/* Unfortunately, we have to handle win32s very differently from NT, 	*/
/* Since VirtualQuery has very different semantics.  In particular,	*/
/* under win32s a VirtualQuery call on an unmapped page returns an	*/
/* invalid result.  Under NT CmmExamineStaticAreas is a noop and   	*/
/* all real work is done by GC_register_dynamic_libraries.  Under	*/
/* win32s, we cannot find the data segments associated with dll's.	*/
/* We examine the main data segment here.				*/

/* Return the smallest address p such that VirtualQuery			*/
/* returns correct results for all addresses between p and start.	*/
/* Assumes VirtualQuery returns correct information for start.		*/
char *
CmmLeastDescribedAddress(char * start)
{  
  MEMORY_BASIC_INFORMATION buf;
  SYSTEM_INFO sysinfo;
  DWORD result;
  LPVOID limit;
  char * p;
  LPVOID q;
  
  GetSystemInfo(&sysinfo);
  limit = sysinfo.lpMinimumApplicationAddress;
  p = (char *)((word)start & ~(CmmGetPageSize() - 1));
  for (;;) {
  	q = (LPVOID)(p - CmmGetPageSize());
  	if ((char *)q > (char *)p /* underflow */ || q < limit) break;
  	result = VirtualQuery(q, &buf, sizeof(buf));
  	if (result != sizeof(buf)) break;
  	p = (char *)(buf.AllocationBase);
  }
  return(p);
}

void
CmmExamineStaticAreas(void (*ExamineArea)(GCP, GCP))
{
  static char dummy;
  char * static_root = &dummy;
  MEMORY_BASIC_INFORMATION buf;
  SYSTEM_INFO sysinfo;
  DWORD result;
  DWORD protect;
  LPVOID p;
  char *base, *limit, *new_limit;
  static char *mallocHeapBase = 0;
  
  if (!(GetVersion() & 0x80000000))
    /* Windows NT */
    return 0;
  /* find base of region used by malloc()	*/
  if (mallocHeapBase == 0) {
    extern int  firstHeapPage;
    result = VirtualQuery((LPCVOID)firstHeapPage, &buf, sizeof(buf));
    if (result != sizeof(buf)) {
      fprintf(stderr, "Weird VirtualQuery result\n");
      exit(-1);
    }
    mallocHeapBase = (char *)(buf.AllocationBase);
  }
  p = base = limit = CmmLeastDescribedAddress(static_root);
  GetSystemInfo(&sysinfo);
  while (p < sysinfo.lpMaximumApplicationAddress) {
    result = VirtualQuery(p, &buf, sizeof(buf));
    if (result != sizeof(buf) || buf.AllocationBase == mallocHeapBase)
      break;
    new_limit = (char *)p + buf.RegionSize;
    protect = buf.Protect;
    if (buf.State == MEM_COMMIT
	&& is_writable(protect)) {
      if ((char *)p == limit)
	limit = new_limit;
      else {
	if (base != limit)
	  (*ExamineArea)((GCP)base, (GCP)limit);
	base = (char*) p;
	limit = new_limit;
      }
    }
    if (p > (LPVOID)new_limit	/* overflow */) break;
    p = (LPVOID)new_limit;
  }
  if (base != limit)
    (*ExamineArea)((GCP)base, (GCP)limit);
}

#else

void
CmmExamineStaticAreas(void (*ExamineArea)(GCP, GCP))
{
  extern int end;
  (*ExamineArea)((GCP)DATASTART, (GCP)&end);
}

#endif /* __WIN32__ */

#if defined(__mips)|| defined(__hpux)||(defined(__sparc) && !defined(__SVR4)) 
extern etext[];
#endif
#if defined(__alpha)
extern unsigned long etext;
#endif

char *
CmmSVR4DataStart(int max_page_size)
{
  Word text_end = ((Word)(&etext) + sizeof(Word) - 1) & ~(sizeof(Word) - 1);
  /* etext rounded to word boundary	*/
  Word next_page = (text_end + (Word)max_page_size - 1)
    & ~((Word)max_page_size - 1);
  Word page_offset = (text_end & ((Word)max_page_size - 1));
  char * result = (char *)(next_page + page_offset);
  /* Note that this isn't equivalent to just adding
   * max_page_size to &etext if &etext is at a page boundary
   */
  return(result);
}
