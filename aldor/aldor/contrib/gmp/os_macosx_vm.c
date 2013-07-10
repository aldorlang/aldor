/*****************************************************************************
 *
 * os_macosx_vm.c: Non-portable, operating system specific code for Mac OS X
 *      vm_allocate etc interface.
 *
 * based on material
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 * Copyright David Casperson 2008.
 *
 ****************************************************************************/

/*
 * This file is meant to be included in os_unix.c.
 *
 * UPDATE COMMENT HERE.
 */

#if (defined(OS_UNIX) && defined(OS_MAC_OSX))
#define OS_Has_Alloc

#include <mach/mach_types.h>
#include <mach/machine/vm_param.h>
#include <mach/mach.h>
#include <mach/vm_region.h>
#include <mach/vm_map.h>

#define OS_PAGE_SIZE 4096 /* from mach/machine/vm_param.h */

Pointer
osAlloc(ULong *pnbytes)
{
	ULong		nbytes = *pnbytes, nextra;
	Pointer		p ;
	struct osStore	*fx;

	/*
	 * Avoid creating zero-sized pieces.
	 */
	if (nbytes == 0) nbytes = 1;

	/*
	 * Consult free list and return lowest address suitable piece.
	 */
	for (fx = osStoreHd.next; fx; fx = fx->next)
		if (fx->use == OS_STORE_FREE && fx->nbytes >= nbytes) {
			fx->use	 = OS_STORE_BUSY;
			*pnbytes = fx->nbytes;
			return fx->piece;
		}
	/*
	 * Ensure there is a link for a new piece.
	 */
	if (!osFreeList	 && osAllocFreeList() == -1) {
		*pnbytes = 0;
		return 0;
	}

	/*
	 * Will allocate piece.	 We don't use sbrk, and we alway allocate
	 * multiples of 4096.
	 */

    nextra = OS_PAGE_SIZE % (alignHint==0 ? 1 : alignHint) ;
    vm_offset_t size = nbytes + nextra;
    size += (OS_PAGE_SIZE-1) ;
    size -= (size % OS_PAGE_SIZE) ;

    /* try allocating storage */

    vm_offset_t addr = 0 ;
    kern_return_t allocation_result
        = vm_allocate(mach_task_self(), &addr, size, VM_FLAGS_ANYWHERE) ;
    switch(allocation_result)
        {
        case KERN_SUCCESS:
            break ;
        case KERN_NO_SPACE:
            *pnbytes=0;return 0;break;
        default:
            assert(allocation_result==KERN_NO_SPACE ||
                   allocation_result==KERN_SUCCESS) ;
            NotReached(break;) ;
        }
    
    
	
	/*
	 * allocate piece and record it.
	 */
	p = (Pointer) (addr);

	fx	   = osFreeList;
	osFreeList = osFreeList->next;
	fx->use	   = OS_STORE_BUSY;
	fx->piece0 = ptrCanon(p);
	fx->piece  = ptrOff(ptrCanon(p),nextra);
	fx->nbytes = size-nextra ;
	fx->nbytes0= size ;

	osAllocLinkIn(fx);
	*pnbytes = fx->nbytes;
	return fx->piece;
}

void
osFree(Pointer p)
{
	struct osStore	*fp, *ff;

	/*
	 * Find the link and predecessor corresponding to this piece.
	 */
	p  = ptrCanon(p);

	for (fp = &osStoreHd, ff = fp->next; ff; fp = ff, ff = fp->next)
		if (ptrEQ(ff->piece,p)) break;

	if (!ptrEQ(ff,NULL)) return;	/* Bad free request; ignore. */


	/*
	 * Return pending memory to OS.
	 */
    kern_return_t deallocate_status
        = vm_deallocate(mach_task_self(), ptrToLong(ff->piece0), ff->nbytes0) ;
    assert(KERN_SUCCESS==deallocate_status);

    fp->next   = ff->next ;

	/*
     * Avoid confusing conservative garbage collectors
     */
    ff->use = OS_STORE_FREE;
    ff->piece0 = (Pointer) 0 ;
    ff->piece  = (Pointer) 0 ;
    ff->nbytes0=0 ;
    ff->nbytes =0 ;
    ff->next   = osFreeList;

    osFreeList = ff;
    return ;
}

/*
 * Place a link in the pieces list, preserving address order of the pieces.
 * The "piece" field is a pointer in canonical form.
 */
local void
osAllocLinkIn(struct osStore *fx)
{
	struct osStore	*fp, *ff;

	for (fp = &osStoreHd, ff = fp->next; ; fp = ff, ff = fp->next)
		if (!ff || ff->piece > fx->piece) {
			fp->next = fx;
			fx->next = ff;
			break;
		}
}

/*
 * Allocate more links into the free list.
 * Success => 0.  Failure => -1.
 */
local int
osAllocFreeList(void)
{
	struct osStore	*fx;
	struct osStore  f0; 
    Pointer		p;
	ULong		i;

    vm_address_t addr = 0 ;
    vm_size_t    size = OS_PAGE_SIZE ;
    kern_return_t allocation_result
        = vm_allocate(mach_task_self(), &addr, size, VM_FLAGS_ANYWHERE) ;
    switch(allocation_result)
        {
        case KERN_SUCCESS:
            break ;
        case KERN_NO_SPACE:
            return -1;break;
        default:
            assert(allocation_result==KERN_NO_SPACE ||
                   allocation_result==KERN_SUCCESS) ;
            NotReached(break;) ;
        }
	p  = (Pointer) addr;

	fx = (struct osStore *) p;

    /* Avoid needlessly confusing conservative gc's. */
    f0.use    = OS_STORE_FREE ;
    f0.nbytes = f0.nbytes0 = 0 ;
    f0.next   = f0.piece   = f0.piece0  = (Pointer) 0 ;

	for (i = 0; i < OS_PAGE_SIZE/sizeof(struct osStore); i++) {
        fx[i] = f0 ;
		fx[i].next = osFreeList;
		osFreeList = &fx[i];
	}

	fx	   = osFreeList;
	osFreeList = osFreeList->next;
	fx->use	   = OS_STORE_ADMIN;
	fx->nbytes = OS_PAGE_SIZE ;
    fx->nbytes0= OS_PAGE_SIZE ;
    fx->piece0 = 
	fx->piece  = ptrCanon(p);

	osAllocLinkIn(fx);
	return 0;
}

void
osAllocShow(void)
{
	struct osStore *ff;

	for (ff = osStoreHd.next; ff; ff = ff->next) {
		char	*s;
		switch (ff->use) {
		case OS_STORE_ADMIN: s = "Ad"; break;
		case OS_STORE_BUSY:  s = "Us"; break;
		case OS_STORE_FREE:  s = "Fr"; break;
		default:	     s = "??"; break;
		}
	}
}

#endif /* OS_UNIX && OS_MAC_OSX */


/****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/

#if defined(OS_MAC_OSX)

#define OS_Has_MemMap

extern char	**environ;

/* !!!!!!!!!!! */
#define MAX_MMAPS 1024 /* this is arbitrary.
                        * find out if there is any way of anticipating this.
                        */

struct os_vm_region_info
{
    Pointer lo ;
    Pointer hi ;
    int flags ;
} ;

typedef struct os_vm_region_info * os_vm_region_t ;
typedef struct os_vm_region_info   os_vm_region_data_t ;

/*
 * next_os_vm_region modifes its argument to point to
 * the next vm_region.  If there is no next region it
 * leaves its argument unchanged and returns (os_vm_region_t) (0)
 */
local os_vm_region_t next_os_vm_region(os_vm_region_t region) ;
local os_vm_region_t set_os_vm_region(os_vm_region_t region, Pointer p) ;

local os_vm_region_t
next_os_vm_region(os_vm_region_t region)
{
    vm_region_basic_info_data_t data ;
    vm_map_t this_task = mach_task_self() ;
    vm_address_t address = ptrToLong(region->hi) ;
    vm_size_t    size ;
    vm_region_flavor_t flavour = VM_REGION_BASIC_INFO ;
    unsigned int info_count = sizeof(data)/sizeof(int) ;
    mach_port_t object_name ;

    kern_return_t response =
        vm_region(
            this_task,
            &address,
            &size,
            flavour,
            (int *) ((void *) (&data)),
            &info_count,
            &object_name) ;
    switch (response)
        {
        case KERN_SUCCESS:
            region->lo = ptrFrLong(address) ;
            region->hi = ptrFrLong(address + size) ;
            region->flags = data.protection ;
            break ;
        case KERN_INVALID_ADDRESS:
            region = 0 ;
            break ;

        default:
            assert("Unexpected return from vm_region" && 0) ;
        }
    return region ;
}

/*
 * next_os_vm_chunk modifes its argument to point to
 * the next vm chunk, where a chunk is a the concatenation of a
 * sequence of contiguous vm regions with identical flags.
 * If there is no next region it
 * leaves its argument unchanged and returns (os_vm_region_t) (0)
 */
local os_vm_region_t
next_os_vm_chunk(os_vm_region_t region)
{
    os_vm_region_data_t temp_region_data2 ;
    os_vm_region_t region2 ;

    region2 = &temp_region_data2 ;
    *region2 = *region ;
    if (ptrEQ(NULL,next_os_vm_region(region)))
        return NULL ;

    for(region2 = set_os_vm_region(&temp_region_data2, region->hi) ;
        (region2 && region2->flags==region->flags
         && ptrEQ(region2->lo,region->hi));
        region2 = next_os_vm_region(region2))
        {
        region->hi = region2->hi ;
        if (ptrEQ(NULL,region->hi))
            break ;
        }
    return region ;
}



local os_vm_region_t
set_os_vm_region(os_vm_region_t region, Pointer p)
{
    region->hi = p ;
    return next_os_vm_region(region) ;
}

local os_vm_region_t
set_os_vm_chunk(os_vm_region_t region, Pointer p)
{
    region->hi = p ;
    return next_os_vm_chunk(region) ;
}

local Pointer min(Pointer a, Pointer b)
{
    /*
     * WARNING : using ptrLT(a,b) was not equivalent to (a<b)
     *      in older versions of cport.h  Make sure that this
     *      uses unsigned arithmetic comparisons.
     */
    return ptrLT(a,b) ? a : b ;
}


local Pointer max(Pointer a, Pointer b)
{
    
    /*
     * WARNING : using ptrGE(a,b) was not equivalent to (a>=b)
     *      in older versions of cport.h
     */
    return ptrGE(a,b) ? a : b ;
}


struct osMemMap **osMemMap(int mask)
{
    static struct osMemMap	mmv[MAX_MMAPS];
    static struct osMemMap*	mmvp[MAX_MMAPS];
    struct osMemMap * mm = &mmv[int0];
    os_vm_region_data_t temp_region_data ;
    os_vm_region_t region = &temp_region_data;
    int use ;
    static Pointer stack_begin = NULL ;
    Pointer stack_end, stack_bottom, stack_top ;

    if (!stack_begin)
        stack_begin = environ ;
    stack_end = &stack_end ;
    stack_bottom = min(stack_begin,stack_end) ;
    stack_top    = max(stack_begin,stack_end) ;


    for(region=set_os_vm_chunk(&temp_region_data,NULL);
        ptrNE(region,NULL);
        region= (ptrNE(region->hi,NULL)) ? next_os_vm_chunk(region) : NULL)
        {
            os_vm_region_data_t below_stack_data ;
            os_vm_region_data_t in_stack_data ;
            os_vm_region_data_t above_stack_data ;
            os_vm_region_t temp_ptr ;
            
            /* ignore read only regions */
            if (!(region->flags&VM_PROT_WRITE))
                continue ;

            /*
             *  At this point *region represents a contiguous section
             *  of virtual memory all of which has the same virtual
             *  memory flags, and that memory is writable by us.
             *
             *  We now split that region into three pieces:  one below
             *  the bottom of the stack, one in the stack, and one above
             *  the top of the stack.  For each of those pieces that is
             *  non-empty we add an appropriate entry into the memory
             *  map.  Should we run out of memory map regions we punt
             *  and return NULL.
             */

            temp_ptr = &below_stack_data ;
            temp_ptr->lo = min(region->lo, stack_bottom) ;
            temp_ptr->hi  = min(region->hi , stack_bottom) ;
            use =
                ((ptrLT(temp_ptr->lo,&environ) &&
                  (ptrLT(&environ,temp_ptr->hi)))
                 ? OSMEM_IDATA : OSMEM_DDATA) ;

            if (!ptrEQ(temp_ptr->lo,temp_ptr->hi) && (use & mask))
                {
                   if (ptrDiff(mm,&mmv[int0])>=MAX_MMAPS) break ;
                    mm->lo  = temp_ptr->lo ;
                    mm->hi  = temp_ptr->hi ;
                    mm->use = use ;
                    ++mm ;
                }

            temp_ptr = &in_stack_data ;
            temp_ptr->lo = max(min(region->lo,stack_top), stack_bottom) ;
            temp_ptr->hi  = min(max(region->hi,stack_bottom) , stack_top) ;
            use = OSMEM_STACK ;
            
            if (!ptrEQ(temp_ptr->lo,temp_ptr->hi) && (use & mask))
                {
                    if (ptrDiff(mm,&mmv[int0])>=MAX_MMAPS) break ;
                    mm->lo  = temp_ptr->lo ;
                    mm->hi  = temp_ptr->hi ;
                    mm->use = use ;
                    ++mm ;
                }

            temp_ptr = &above_stack_data ;
            temp_ptr->lo = max(region->lo,stack_top) ;
            temp_ptr->hi  = max(region->hi,stack_top) ;
            use = OSMEM_DDATA ;
            
            if (!ptrEQ(temp_ptr->lo,temp_ptr->hi) && (use & mask))
                {
                    if (ptrDiff(mm,&mmv[int0])>=MAX_MMAPS) break ;
                    mm->lo  = temp_ptr->lo ;
                    mm->hi  = temp_ptr->hi ;
                    mm->use = use ;
                    ++mm ;
                }
        }
    if (mm-&mmv[int0]>=MAX_MMAPS)
        return NULL ;
    mm->use = OSMEM_END ;
    mm = mmv;
    int i = int0;
    /* print what we found */
    while (mm->use != OSMEM_END) {
#if 0
        char *desc = "";
        if (mm->use == OSMEM_DDATA) desc = "Data";
        if (mm->use == OSMEM_STACK) desc = "Stack";
#if EDIT_1_0_n1_07
        printf("%s: %lx --> %lx\n", desc, mm->lo, mm->hi);
#else
        printf("%s: %p --> %p\n", desc, mm->lo, mm->hi);
#endif
#endif
        mmvp[i]=mm;
        mm++;
        i++;
    }
    mmvp[i]=mm;
    return mmvp;
}
/* !!!!!!!!!!! */


#endif /* OS_MAC_OSX */

