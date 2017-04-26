/*==============================================================================
*
*            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION
*
*   Property of Texas Instruments
*   For Use by Texas Instruments and/or its Licensees Only.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*   Unauthorized reproduction and/or distribution are strictly prohibited.
*   This product is protected under copyright law and trade secret law as an
*   Unpublished work.
*   Created 2011, (C) Copyright 2011 Texas Instruments.  All rights reserved.
*
*   Component  :
*
*   Filename   : 	alg_malloc.c
*
*   Description:	This module implements an algorithm memory management "policy" in which
*  					no memory is shared among algorithm objects.  Memory is, however
*  					reclaimed when objects are deleted.
*
*  					preemption      sharing             object deletion
*  					----------      -------             ---------------
*  					   yes(*)          none                yes
*
*  					Note 1: this module uses run-time support functions malloc() and free()
*  					to allocate and free memory.  Since these functions are *not* reentrant,
*  					either all object creation and deletion must be performed by a single
*  					thread or reentrant versions or these functions must be created.
*
*  					Note 2: Memory alignment is supported for the c5000 targets with the
*  					memalign5000() function which allocates 'size + align' memory.  This
*  					is necessary since the compiler's run-time library does not
*  					support memalign().  This wastes 'align' extra memory.
*
*=============================================================================*/

#include <common/xdm_packages/ti/xdais/std.h>
#include <common/xdm_packages/ti/xdais/alg.h>
#include <common/xdm_packages/ti/xdais/ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

#if defined (_54_) || (_55_) || (_28_) || (WIN32) || (IPHONE) || (LINUX)
void *mem_align(size_t alignment, size_t size);
void mem_free(void *ptr);
#define myMemalign  mem_align
#define myFree      mem_free
#else
#define myMemalign  memalign
#define myFree      free
#endif


Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);

/*
 *  ======== ALG_activate ========
 */
Void ALG_activate(ALG_Handle alg)
{
    /* restore all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
    /* do app specific initialization of scratch memory */
    if (alg->fxns->algActivate != NULL) {
        alg->fxns->algActivate(alg);
    }
}

/*
 *  ======== ALG_deactivate ========
 */
Void ALG_deactivate(ALG_Handle alg)
{
    /* do app specific store of persistent data */
    if (alg->fxns->algDeactivate != NULL) {
        alg->fxns->algDeactivate(alg);
    }

    /* save all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
}

/*
 *  ======== ALG_exit ========
 */
Void ALG_exit(Void)
{
}

/*
 *  ======== ALG_init ========
 */
Void ALG_init(Void)
{
}


/*
 *  ======== _ALG_allocMemory ========
 */
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
        memTab[i].base = (void *)myMemalign(memTab[i].alignment, memTab[i].size);

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
        memset(memTab[i].base, 0, memTab[i].size);
    }

    return (TRUE);
}

/*
 *  ======== _ALG_freeMemory ========
 */
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
        if (memTab[i].base != NULL) {
            myFree(memTab[i].base);
        }
    }
}
#if defined (_54_) || (_55_) || (_28_)
/*
 *  ======== mem_align ========
 */
void *mem_align(size_t alignment, size_t size)
{
    void     **mallocPtr;
    void     **retPtr;

    /* return if invalid size value */ 
    if (size <= 0) {
       return (0);
    }

    /*
     * If alignment is not a power of two, return what malloc returns. This is
     * how memalign behaves on the c6x.
     */
    if ((alignment & (alignment - 1)) || (alignment <= 1)) {
        if( (mallocPtr = malloc(size + sizeof(mallocPtr))) != NULL ) {
            *mallocPtr = mallocPtr;
            mallocPtr++;
        }
        return ((void *)mallocPtr);
    }

    /* allocate block of memory */
    if ( !(mallocPtr = malloc(alignment + size)) ) { 
        return (0);
    }

    /* Calculate aligned memory address */ 
#if !defined(_55_) 
    retPtr = (void *)(((Uns)mallocPtr + alignment) & ~(alignment - 1));
#else
    retPtr = (void *)(((Uint32)mallocPtr + alignment) & ~(Uint32)(alignment - 1));
#endif
    /* Set pointer to be used in the mem_free() fxn */
    retPtr[-1] = mallocPtr;

    /* return aligned memory */
    return ((void *)retPtr);
}

/*
 *  ======== mem_free ========
 */
Void mem_free(void *ptr)
{
    free((void *)((void **)ptr)[-1]);
}
#endif
#if defined (WIN32) || (IPHONE) || (LINUX)
void *mem_align(size_t alignment, size_t size)
{
    void     **mallocPtr;
    void     **retPtr;

    /* return if invalid size value */ 
    if (size <= 0) {
       return (0);
    }

    /*
     * If alignment is not a power of two, return what malloc returns. This is
     * how memalign behaves on the c6x.
     */
    if ((alignment & (alignment - 1)) || (alignment <= 1)) {
        if( (mallocPtr = malloc(size + sizeof(mallocPtr))) != NULL ) {
            *mallocPtr = mallocPtr;
            mallocPtr++;
        }
        return ((void *)mallocPtr);
    }

    /* allocate block of memory */
    if ( !(mallocPtr = malloc(alignment + size)) ) { 
        return (0);
    }

    /* Calculate aligned memory address */ 
#if defined(__LP64__)
    retPtr = (void *)(((unsigned long int)mallocPtr + alignment) & ~(alignment - 1));
#else
    retPtr = (void *)(((Uns)mallocPtr + alignment) & ~(alignment - 1));
#endif
    /* Set pointer to be used in the mem_free() fxn */
    retPtr[-1] = mallocPtr;

    /* return aligned memory */
    return ((void *)retPtr);
}
/*
 *  ======== mem_free ========
 */
Void mem_free(void *ptr)
{
    free((void *)((void **)ptr)[-1]);
}
#endif
