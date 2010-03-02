/*
    ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    templates/chtypes.h
 * @brief   System types template.
 * @details The types defined in this file may change depending on the target
 *          architecture. You may also try to optimize the size of the various
 *          types in order to privilege size or performance, be careful in
 *          doing so.
 *
 * @addtogroup types
 * @{
 */

#ifndef _CHTYPES_H_
#define _CHTYPES_H_

#define __need_NULL
#define __need_size_t
#include <stddef.h>

//#if !defined(_STDINT_H) && !defined(__STDINT_H_)
//#include <stdint.h>
//#endif

typedef unsigned char   uint8_t;
typedef signed char     int8_t;
typedef unsigned int    uint16_t;
typedef signed int      int16_t;
typedef unsigned long   uint32_t;
typedef signed long     int32_t;
typedef uint8_t         uint_fast8_t;
typedef uint16_t        uint_fast16_t;
typedef uint32_t        uint_fast32_t;

/**
 * @brief   Boolean, recommended the fastest signed.
 */
typedef int8_t          bool_t;

/**
 * @brief   Thread mode flags, uint8_t is ok.
 */
typedef uint8_t         tmode_t;

/**
 * @brief   Thread state, uint8_t is ok.
 */
typedef uint8_t         tstate_t;

/**
 * @brief   Thread references counter, uint8_t is ok.
 */
typedef uint8_t         trefs_t;

/**
 * @brief   Priority, use the fastest unsigned type.
 */
typedef uint8_t         tprio_t;

/**
 * @brief   Message, use signed pointer equivalent.
 */
typedef int16_t         msg_t;

/**
 * @brief   Event Id, use fastest signed.
 */
typedef int16_t         eventid_t;

/**
 * @brief   Event Mask, recommended fastest unsigned.
 */
typedef uint16_t        eventmask_t;

/**
 * @brief   System Time, recommended fastest unsigned.
 */
typedef uint16_t        systime_t;

/**
 * @brief   Counter, recommended fastest signed.
 */
typedef int16_t         cnt_t;

/**
 * @brief   Inline function modifier.
 */
#define INLINE inline

/**
 * @brief   Packed structure modifier (within).
 */
#define PACK_STRUCT_STRUCT

/**
 * @brief   Packed structure modifier (before).
 */
#define PACK_STRUCT_BEGIN

/**
 * @brief   Packed structure modifier (after).
 */
#define PACK_STRUCT_END

#endif /* _CHTYPES_H_ */

/** @} */
