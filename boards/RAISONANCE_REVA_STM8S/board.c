/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#include "ch.h"
#include "hal.h"

/*
 * TIM 2 clock after the prescaler.
 */
#define TIM2_CLOCK  (SYSCLK / 16)
#define TIM2_ARR    ((TIM2_CLOCK / CH_FREQUENCY) - 1)

/*
 * TIM2 interrupt handler.
 */
CH_IRQ_HANDLER(13) {

  CH_IRQ_PROLOGUE();

  chSysLockFromIsr();
  chSysTimerHandlerI();
  chSysUnlockFromIsr();

  TIM2_SR1 = 0;

  CH_IRQ_EPILOGUE();
}

/*
 * Board initialization code.
 */
void hwinit(void) {

  /*
   * HAL initialization.
   */
  halInit();

  /*
   * TIM2 initialization as system tick.
   */
  CLK_PCKENR1 |= 32;            /* PCKEN15, TIM2 clock source.*/
  TIM2_PSCR    = 4;             /* Prescaler divide by 2^4=16.*/
  TIM2_ARRH    = TIM2_ARR >> 8;
  TIM2_ARRL    = TIM2_ARR;
  TIM2_CNTRH   = 0;
  TIM2_CNTRL   = 0;
  TIM2_SR1     = 0;
  TIM2_IER     = 1;             /* UIE */
  TIM2_CR1     = 1;             /* CEN */
}
