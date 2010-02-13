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
 * @file    templates/chcore.c
 * @brief   Port related template code.
 * @details This file is a template of the system driver functions provided by
 *          a port. Some of the following functions may be implemented as
 *          macros in chcore.h if the implementer decides that there is an
 *          advantage in doing so, as example because performance concerns.
 *
 * @addtogroup core
 * @{
 */

//Required for inline assembly:
#pragma SRC

#include "ch.h"
#include <intrins.h>

/**
 * @brief   Port-related initialization code.
 * @note    This function is usually empty.
 */
void port_init(void) {
}

/**
 * @brief   Kernel-lock action.
 * @details Usually this function just disables interrupts but may perform more
 *          actions.
 */
void port_lock(void) {
    _sim_();
}

/**
 * @brief   Kernel-unlock action.
 * @details Usually this function just disables interrupts but may perform more
 *          actions.
 */
void port_unlock(void) {
    _rim_();
}

/**
 * @brief   Kernel-lock action from an interrupt handler.
 * @details This function is invoked before invoking I-class APIs from
 *          interrupt handlers. The implementation is architecture dependent,
 *          in its simplest form it is void.
 */
void port_lock_from_isr(void) {
}

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 * @details This function is invoked after invoking I-class APIs from interrupt
 *          handlers. The implementation is architecture dependent, in its
 *          simplest form it is void.
 */
void port_unlock_from_isr(void) {
}

/**
 * @brief   Disables all the interrupt sources.
 * @note    Of course non maskable interrupt sources are not included.
 */
void port_disable() {
    _sim_();
}

/**
 * @brief   Disables the interrupt sources that are not supposed to preempt
 *          the kernel.
 */
void port_suspend(void) {
    _sim_();
}

/**
 * @brief   Enables all the interrupt sources.
 */
void port_enable(void) {
    _rim_();
}

/**
 * @brief   Enters an architecture-dependent halt mode.
 * @details The function is meant to return when an interrupt becomes pending.
 *          The simplest implementation is an empty function or macro but this
 *          would not take advantage of architecture-specific power saving
 *          modes.
 */
void port_wait_for_interrupt(void) {
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
#pragma ASM

#pragma ENDASM

    //Swap Stack Pointers
    otp->p_ctx.sp = (struct intctx*)_getSP_();
    _setSP_((unsigned int)(ntp->p_ctx.sp));

#pragma ASM
    
#pragma ENDASM
}

/** @} */

