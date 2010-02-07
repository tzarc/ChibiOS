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
 * @file    CF/chcore.c
 * @brief   ColdFire architecture port code.
 * @details This file is a template of the system driver functions provided by
 *          a port. Some of the following functions may be implemented as
 *          macros in chcore.h if the implementer decides that there is an
 *          advantage in doing so, as example because performance concerns.
 *
 * @addtogroup CF_CORE
 * @{
 */

#include <ch.h>

/**
 * @brief   ColdFire initialization code.
 * @note    This function is usually empty.
 */
void port_init(void){
}

/**
 * @brief   Disables all the interrupt sources.
 * @note    Of course non maskable interrupt sources are not included.
 */
void port_disable() {

  asm volatile ("move.w  #0x2700, %sr");
}

/**
 * @brief   Disables the interrupt sources that are not supposed to preempt
 *          the kernel.
 */
void port_suspend(void) {

  asm volatile ("move.w  #0x2700, %sr");
}

/**
 * @brief   Enables all the interrupt sources.
 */
void port_enable(void) {

  asm volatile ("move.w  #0x2000, %sr");
}

/**
 * @brief   Halts the system.
 * @details This function is invoked by the operating system when an
 *          unrecoverable error is detected (as example because a programming
 *          error in the application code that triggers an assertion while in
 *          debug mode).
 */
void port_halt(void) {

  port_disable();
  while (TRUE) {
  }
}

/**
 * @brief   Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 *          is responsible for the context switch between 2 threads.
 * @note    The implementation of this code affects <b>directly</b> the context
 *          switch performance so optimize here as much as you can.
 *
 * @param otp   the thread to be switched out
 * @param ntp   the thread to be switched in
 */
void port_switch(Thread *otp, Thread *ntp) {

  asm volatile (
    "movel   %sp@(4), %a0                       \n\t"
    "movel   %sp@(8), %a1                       \n\t"
    "movel   %a1@(12), %a1                      \n\t"
    "lea     %sp@(-44), %sp                     \n\t"
    "moveml  %d2-%d7/%a2-%a6, %sp@              \n\t"
    "movel   %sp, %a0@(12)                      \n\t"
    "movel   %a1, %sp                           \n\t"
    "moveml  %sp@, %d2-%d7/%a2-%a6              \n\t"
    "lea     %sp@(44), %sp                      \n\t"
   );
}

/**
 * @brief   Starts a thread by invoking its work function.
 * @details If the work function returns @p chThdExit() is automatically
 *          invoked.
 */
void _port_thread_start(void) {

  asm volatile (
    "move.w  #0x2000,%sr                        \n\t"
    "movel   %a6,-(%sp)                         \n\t"
    "jsr     %a5@                               \n\t"
    "addql   #4,%sp                             \n\t"
    "jsr     chThdExit                          \n\t"
  );
}

/** @} */
