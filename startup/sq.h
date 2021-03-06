/* KallistiOS 2.1.0

   kernel/arch/dreamcast/include/dc/sq.h
   (C)2000-2001 Andrew Kieschnick

* This program is free software; you can redistribute it and/or modify
* it under the terms of the KOS License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* KOS License for more details (DreamHAL note: it's in the LICENSE file).
*
* You should have received a copy of the KOS License along with this
* program; if not, please visit Cryptic Allusion Game Dev at:
*
*   http://gamedev.allusion.net/
*/

/** \file   dc/sq.h
    \brief  Functions to access the SH4 Store Queues.

    The store queues are a way to do efficient burst transfers from the CPU to
    external memory. They can be used in a variety of ways, such as to transfer
    a texture to PVR memory. The transfers are in units of 32-bytes, and the
    destinations must be 32-byte aligned.

    \author Andrew Kieschnick
*/

// Note 1/4/2020:
// This file has been modified in order to work in a freestanding environment.
// --Moopthehedgehog

#ifndef __DC_SQ_H
#define __DC_SQ_H

#include "types.h"

/** \brief  Store Queue 0 access register */
#define QACR0 (*(volatile unsigned int *)(void *)0xff000038)

/** \brief  Store Queue 1 access register */
#define QACR1 (*(volatile unsigned int *)(void *)0xff00003c)

/** \brief  Clear a block of memory.

    This function is similar to calling memset() with a value to set of 0, but
    uses the store queues to do its work.

    \param  dest            The address to begin clearing at (32-byte aligned).
    \param  n               The number of bytes to clear (multiple of 32).
*/
void sq_clr(void *dest, int n);

/** \brief  Copy a block of memory.

    This function is similar to memcpy4(), but uses the store queues to do its
    work.

    \param  dest            The address to copy to (32-byte aligned).
    \param  src             The address to copy from (32-bit (4-byte) aligned).
    \param  n               The number of bytes to copy (multiple of 32).
    \return                 The original value of dest.
*/
void * sq_cpy(void *dest, const void *src, int n);

/** \brief  Set a block of memory to an 8-bit value.

    This function is similar to calling memset(), but uses the store queues to
    do its work.

    \param  s               The address to begin setting at (32-byte aligned).
    \param  c               The value to set (in the low 8-bits).
    \param  n               The number of bytes to set (multiple of 32).
    \return                 The original value of dest.
*/
void * sq_set(void *s, uint32 c, int n);

/** \brief  Set a block of memory to a 16-bit value.

    This function is similar to calling memset2(), but uses the store queues to
    do its work.

    \param  s               The address to begin setting at (32-byte aligned).
    \param  c               The value to set (in the low 16-bits).
    \param  n               The number of bytes to set (multiple of 32).
    \return                 The original value of dest.
*/
void * sq_set16(void *s, uint32 c, int n);

/** \brief  Set a block of memory to a 32-bit value.

    This function is similar to calling memset4(), but uses the store queues to
    do its work.

    \param  s               The address to begin setting at (32-byte aligned).
    \param  c               The value to set (all 32-bits).
    \param  n               The number of bytes to set (multiple of 32).
    \return                 The original value of dest.
*/
void * sq_set32(void *s, uint32 c, int n);

#endif
