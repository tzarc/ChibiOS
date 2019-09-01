/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

#include "sbhost.h"

/*
 * LEDs blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(50);
    palClearLine(LINE_LED_RED);
    chThdSleepMilliseconds(200);
    palSetLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(50);
    palSetLine(LINE_LED_RED);
    chThdSleepMilliseconds(200);
  }
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    if (palReadLine(LINE_JOY_CENTER)) {
//      test_execute((BaseSequentialStream *)&SD2, &rt_test_suite);
//      test_execute((BaseSequentialStream *)&SD2, &oslib_test_suite);
      extern uint32_t __flash7_start__, __flash7_end__,
                      __ram7_start__, __ram7_end__;
      static const sb_regions_t regions = {
        .r1_base = (uint32_t)&__flash7_start__,
        .r1_end  = (uint32_t)&__flash7_end__,
        .r2_base = (uint32_t)&__ram7_start__,
        .r2_end  = (uint32_t)&__ram7_end__
      };

      mpuConfigureRegion(MPU_REGION_0,
                         regions.r1_base,
                         MPU_RASR_ATTR_AP_RO_RO |
                         MPU_RASR_ATTR_CACHEABLE_WT_NWA |
                         MPU_RASR_SIZE_16K |
                         MPU_RASR_ENABLE);

      mpuConfigureRegion(MPU_REGION_1,
                         regions.r2_base,
                         MPU_RASR_ATTR_AP_RW_RW |
                         MPU_RASR_ATTR_CACHEABLE_WB_WA |
                         MPU_RASR_SIZE_4K |
                         MPU_RASR_ENABLE);

      sbStart((const sb_header_t *)&__flash7_start__, &regions);
      chSysHalt("it returned");
    }
    chThdSleepMilliseconds(500);
  }
}
