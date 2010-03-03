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

#include "ch.h"
#include "hal.h"
#include "test.h"

/*
 * LEDs blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 64);
static msg_t Thread1(void *arg) {

  (void)arg;
  while (TRUE) {
    palClearPad(IOPORT2, PB_LED(7));
    chThdSleepMilliseconds(500);
    palSetPad(IOPORT2, PB_LED(7));
    chThdSleepMilliseconds(500);
  }
  return 0;
}

/*
 * Entry point.
 */
void main(void) {

  /*
   * Board/HAL initialization.
   */
  hwinit();

  /*
   * OS initialization.
   */
  chSysInit();

  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity.
   */
  while (TRUE) {
/*    volatile msg_t result;
    result = TestThread(&SD1);*/
    sdWriteTimeout(&SD1, "Hello World!\r\n", 14, TIME_INFINITE);
    chThdSleepMilliseconds(1000);
  }
}
