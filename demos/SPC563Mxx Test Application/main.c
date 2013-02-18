/*
 * Licensed under ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Inclusion of the main header files of all the imported components in the
   order specified in the application wizard. The file is generated
   automatically.*/
#include "components.h"

#include "shellcmd.h"

/*
 * Shell configuration structure, the first field is the serial port used by
 * the shell, the second is the array of commands accepted by the shell and
 * defined in shellcmd.c.
 */
static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SD1,
  shell_commands
};

/*
 * LEDs blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");

  while (TRUE) {
    unsigned i;

    for (i = 0; i < 4; i++) {
      palClearPad(PORT11, P11_LED1);
      chThdSleepMilliseconds(100);
      palClearPad(PORT11, P11_LED2);
      chThdSleepMilliseconds(100);
      palClearPad(PORT11, P11_LED3);
      chThdSleepMilliseconds(100);
      palClearPad(PORT11, P11_LED4);
      chThdSleepMilliseconds(100);
      palSetPad(PORT11, P11_LED1);
      chThdSleepMilliseconds(100);
      palSetPad(PORT11, P11_LED2);
      chThdSleepMilliseconds(100);
      palSetPad(PORT11, P11_LED3);
      chThdSleepMilliseconds(100);
      palSetPad(PORT11, P11_LED4);
      chThdSleepMilliseconds(300);
    }

    for (i = 0; i < 4; i++) {
      palTogglePad(PORT11, P11_LED1);
      chThdSleepMilliseconds(250);
      palTogglePad(PORT11, P11_LED1);
      palTogglePad(PORT11, P11_LED2);
      chThdSleepMilliseconds(250);
      palTogglePad(PORT11, P11_LED2);
      palTogglePad(PORT11, P11_LED3);
      chThdSleepMilliseconds(250);
      palTogglePad(PORT11, P11_LED3);
      palTogglePad(PORT11, P11_LED4);
      chThdSleepMilliseconds(250);
      palTogglePad(PORT11, P11_LED4);
    }

    palSetPort(PORT11,
    		   PAL_PORT_BIT(P11_LED1) | PAL_PORT_BIT(P11_LED2) |
               PAL_PORT_BIT(P11_LED3) | PAL_PORT_BIT(P11_LED4));
  }
  return 0;
}

/*
 * Application entry point.
 */
int main(void) {
  Thread *shelltp = NULL;

  /* Initialization of all the imported components in the order specified in
     the application wizard. The function is generated automatically.*/
  componentsInit();

  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /* Application main loop.*/
  while (1) {
    if (!shelltp)
      shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
    else if (chThdTerminated(shelltp)) {
      chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
      shelltp = NULL;           /* Triggers spawning of a new shell.        */
    }
    chThdSleepMilliseconds(1000);
  }
}
