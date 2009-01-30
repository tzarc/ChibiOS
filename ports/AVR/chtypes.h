/*
    ChibiOS/RT - Copyright (C) 2009 Giovanni Di Sirio.

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

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#ifndef _CHTYPES_H_
#define _CHTYPES_H_

#define __need_NULL
#define __need_size_t
#define __need_ptrdiff_t
#include <stddef.h>

#if !defined(_STDINT_H) && !defined(__STDINT_H_)
#include <stdint.h>
#endif

typedef int8_t          bool_t;
typedef uint8_t         tmode_t;
typedef uint8_t         tstate_t;
typedef uint8_t         tid_t;
typedef uint8_t         tprio_t;
typedef int16_t         msg_t;
typedef uint8_t         eventid_t;
typedef uint8_t         eventmask_t;
typedef uint16_t        systime_t;
typedef int8_t          cnt_t;

#define INLINE      inline
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#endif /* _CHTYPES_H_ */
