/*---------------------------------------------------------------------------*
 *
 *  msw.h:	memory manager with mark&sweep garbage collection.
 *
 *  date:	6 March 1995
 *  authors:	Pietro Iglio
 *  email:	cmm@di.unipi.it, iglio@posso.dm.unipi.it
 *  address:	Dipartimento di Informatica
 *		Corso Italia 40
 *		I-56125 Pisa, Italy
 *
 *  Copyright (C) 1995 Pietro Iglio
 *
 *  This file is part of the PoSSo Customizable Memory Manager (CMM).
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and agree to license at no charge to all parties under these terms and
 * conditions any derivative works or modified versions of this software.
 * 
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.  
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE COPYRIGHT HOLDERS DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL THE COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *---------------------------------------------------------------------------*/

#ifndef	_msw_h
#define _msw_h

#define	MSW_Automatic		1
#define MSW_OnDemand		2

#ifdef __cplusplus
extern "C" {
#endif

extern void *		mswAlloc		(unsigned long size);
extern void * 		mswAllocOpaque		(unsigned long size);
extern void		mswFree			(void * p);

unsigned long		mswGetObjSize		(void * ptr);
extern void *		mswRealloc		(void *, unsigned long);
extern void *		mswCalloc		(unsigned long, unsigned long);

extern void		mswCollect		(void);
extern void		mswCollectNow		(void);

extern void		mswInit			(unsigned);
extern void		mswShowInfo		(void);

extern void		mswCheckHeap		(int verbose);
extern void		mswCheckAllocatedObj	(void *);

extern void		mswTempHeapStart	(void);
extern void		mswTempHeapEnd		(void);
extern void		mswTempHeapFree		(void);
extern void		mswRegisterRoot		(void *);

#ifdef __cplusplus
}
#endif

#endif /* ! _msw_h */
