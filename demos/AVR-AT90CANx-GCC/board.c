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

#include <ch.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "board.h"
#include "avr_serial.h"

ISR(TIMER0_COMP_vect) {

  chSysIRQEnterI();

  chSysTimerHandlerI();

  chSysIRQExitI();
}

/*
 * Board initialization code.
 */
void hwinit(void) {

  /*
   * I/O ports setup.
   */
  DDRA   = VAL_DDRA;
  PORTA  = VAL_PORTA;
  DDRB   = VAL_DDRB;
  PORTB  = VAL_PORTB;
  DDRC   = VAL_DDRC;
  PORTC  = VAL_PORTC;
  DDRD   = VAL_DDRD;
  PORTD  = VAL_PORTD;
  DDRE   = VAL_DDRE;
  PORTE  = VAL_PORTE;
  DDRF   = VAL_DDRF;
  PORTF  = VAL_PORTF;
  DDRG   = VAL_DDRG;
  PORTG  = VAL_PORTG;

  /*
   * External interrupts setup, all disabled initially.
   */
  EICRA  = 0x00;
  EICRB  = 0x00;
  EIMSK  = 0x00;

  /*
   * Enables Idle mode for SLEEP instruction.
   */
  SMCR   = (1 << SE);

  /*
   * Timer 0 setup.
   */
  TCCR0A = (1 << WGM01) | (0 << WGM00) |                // CTC mode.
           (0 << COM0A1) | (0 << COM0A0) |              // OC0A disabled (normal I/O).
           (0 << CS02) | (1 << CS01) | (1 << CS00);     // CLK/64 clock source.
  OCR0A  = F_CPU / 64 / CH_FREQUENCY - 1;
  TCNT0  = 0;                                           // Reset counter.
  TIFR0  = (1 << OCF0A);                                // Reset pending (if any).
  TIMSK0 = (1 << OCIE0A);                               // Interrupt on compare.

  /*
   * Other initializations.
   */
  InitSerial();
}
