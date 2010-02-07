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

/**
 * @file ports/ColdFire/chcore.c
 * @brief ColdFire architecture port code.
 * @addtogroup ColdFire_CORE
 * @{
 */

#include <ch.h>


/*
 * This file is a template of the system driver functions provided by a port.
 * Some of the following functions may be implemented as macros in chcore.h if
 * the implementer decides that there is an advantage in doing so, as example
 * because performance concerns.
 */

/**
 * @brief ColdFire initialization code.
 *
 * @note This function is usually empty.
 */
void port_init(void){
}

/**
 * @brief Kernel-unlock action.
 * @details Usually this function just disables interrupts but may perform more
 * actions.
 */
/*
void port_lock(void) {

	asm volatile ("move.w		#0x2700,%sr			\n\t");
}
*/

/**
 * @brief Kernel-unlock action.
 * @details Usually this function just disables interrupts but may perform more
 * actions.
 */
/*
void port_unlock(void) {

	asm volatile ("move.w		#0x2000,%sr			\n\t");
}
*/

/**
 * @brief Kernel-lock action from an interrupt handler.
 * @details This function is invoked before invoking I-class APIs from
 * interrupt handlers. The implementation is architecture dependent, in its
 * simplest form it is void.
 */
void port_lock_from_isr(void) {
}

/**
 * @brief Kernel-unlock action from an interrupt handler.
 * @details This function is invoked after invoking I-class APIs from interrupt
 * handlers. The implementation is architecture dependent, in its simplest form
 * it is void.
 */
void port_unlock_from_isr(void) {
}

/**
 * @brief Disables all the interrupt sources.
 *
 * @note Of course non maskable interrupt sources are not included.
 */
void port_disable() {

	asm volatile ("move.w		#0x2700,%sr			\n\t");
}

/**
 * @brief Disables the interrupt sources that are not supposed to preempt the kernel.
 */
void port_suspend(void) {
}

/**
 * @brief Enables all the interrupt sources.
 */
void port_enable(void) {

	asm volatile ("move.w		#0x2000,%sr			\n\t");
}


/**
 * @brief Enters an architecture-dependent halt mode.
 * @details The function is meant to return when an interrupt becomes pending.
 * The simplest implementation is an empty function but this will not take
 * advantage of architecture-specific power saving modes.
 */
void port_wait_for_interrupt(void) {
}

/**
 * @brief Halts the system.
 * @details This function is invoked by the operating system when an
 * unrecoverable error is detected (as example because a programming error in
 * the application code that triggers an assertion while in debug mode).
 */
void port_halt(void) {

	port_disable();
	while (TRUE) {
	}
}

/**
 * @brief Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 * is responsible for the context switch between 2 threads.
 *
 * @param otp the thread to be switched out
 * @param ntp the thread to be switched in
 * @note The implementation of this code affects <b>directly</b> the context
 *       switch performance so optimize here as much as you can.
 */
void port_switch(Thread *otp, Thread *ntp) {

	asm volatile (
		"movel		%sp@(4),%a0						\n\t"
		"movel		%sp@(8),%a1						\n\t"
		"movel		%a1@(14),%a1					\n\t"
		"lea		%sp@(-44),%sp		 			\n\t"
		"moveml		%d2-%d7/%a2-%a6,%sp@			\n\t"
		"movel		%sp,%a0@(14)					\n\t"
		"movel		%a1,%sp							\n\t"
		"moveml		%sp@,%d2-%d7/%a2-%a6			\n\t"
		"lea		%sp@(44),%sp		 			\n\t"
 	);
}

/**
 * Start a thread by invoking its work function.
 * If the work function returns @p chThdExit() is automatically invoked.
 */
void threadstart(void) {

	asm volatile (
		"move.w		#0x2000,%sr						\n\t"
		"movel		%a6,-(%sp)						\n\t"
		"jsr		%a5@							\n\t"
		"addql		#4,%sp							\n\t"
		"jsr		chThdExit						\n\t"
	);
}

/** @} */
