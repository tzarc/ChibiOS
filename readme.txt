*****************************************************************************
*** Directories structure                                                 ***
*****************************************************************************

./readme.txt           - This file.
./license.txt          - GPL3 license file.
./src/                 - ChibiOS/RT portable kernel source files.
./src/include/         - ChibiOS/RT include files.
./src/lib/             - ChibiOS/RT library code that can be included into
                         user applications but is not part of the core system.
                         The code in this directory is meant to be portable,
                         generic and architecture indipendent.
./src/templates/       - ChibiOS/RT non portable source templates, new ports
                         are started by copying the templates into a new
                         directory under ./demos/.
./ports/               - Architecture/compiler specific portable files.
./demos/               - Demo programs for specific archtectures/boards.
./test/                - Test code, used by some demos.
./docs/Doxifile        - Doxigen project file.
./docs/html/index.html - ChibiOS/RT documentation (after running doxigen).
                         The documentation is also available on the project
                         web page: http://chibios.sourceforge.net/

Current ports under ./demos:

Win32-MinGW            - ChibiOS/RT simulator and demo into a WIN32 process,
                         MinGW version.
Win32-MSVS             - ChibiOS/RT simulator and demo into a WIN32 process,
                         Visual Studio 7 or any later version should work.
ARM7-LPC214x-GCC       - ChibiOS/RT port for ARM7 LPC2148, the demo targets
                         the Olimex LPC-P2148 board. This port can be easily
                         modified for any processor into the LPC2000 family or
                         other boards. The demo can be compiled using YAGARTO
                         or any other GCC-based ARM toolchain. Full demo.
ARM7-LPC214x-GCC-min   - Minimal demo for LPC214X.
AVR-AT90CANx-GCC       - Port on AVR AT90CAN128, not complete yet.

*****************************************************************************
*** Releases                                                              ***
*****************************************************************************

*** 0.4.5 ***
- Moved the serial IRQ handlers and VIC vectors initialization inside the
  serial drivers. Now the serial-related code is all inside the driver.
- Moved all the other interrupt handlers from chcore2.s into chcore.c as
  inline asm code. The interrupt code now is faster because one less call
  level.
- Fixed a minor problem in chSysHalt() now it disables FIQ too and makes sure
  to keep the processor in the state it had when it was halted.
  Note: This is not a kernel bug but something specific with the ARM port, the
        other ports are not affected.
- Fixed the macro chThdResumeI(), it had a regression.

*** 0.4.4 ***
- Fixed a very important bug in the preemption ARM code, important enough to
  make this update *mandatory*.
  Note: This is not a kernel bug but something specific with the ARM port, the
        other ports are not affected.
- Fixed a nasty bug in the pure THUMB mode threads trampoline code (chcore2.s,
  threadstart), it failed on THUMB threads returning with a "bx" instruction.
  The bug did not affect ARM mode or THUMB with interworking mode.
  Note: This is not a kernel bug but something specific with the ARM port, the
        other ports are not affected.
- Fixed a bug in chIQGetTimeout(), interrupts were not re-enabled when exiting
  the function because a timeout. The problem affected that API only.
- Fixed a potential problem in chSysInit(), it should not affect any past
  application.
- Added a chDbgAssert() API to the debug subsystem.
- Cleaned up the kernel source code using chDbgAssert() instead of a lot of
  "#ifdef CH_USE_DEBUG", it is much more readable now.
- Now the threads working area is filled with a 0x55 when in debug mode, this
  will make easier to track stack usage using a JTAG probe.
- Added an I/O Queues benchmark to the test suite.
- Removed the chSchTimerHandlerI() routine from chschd.c and moved it into
  chinit.c renaming it chSysTimerHandlerI() because it is not part of the
  scheduler.

*** 0.4.3 ***
- Size optimization in the events code, now the chEvtWait() reuses the
  chEvtWaitTimeout() code if it is enabled.
- Size optimization in the semaphores code, now the chSemWaitTimeout() just
  invokes the chSemWaitTimeoutS() inside its system mutex zone. Same thing
  done with chSemWait() and chSemWaitS().
- Size optimization in the queues code.
- Modified the return type of chSemWait() and chSemWaitS() from void to t_msg,
  this allows to understand if the semaphore was signaled or reset without
  have to access the Thread structure.
- Added a threads create/exit/wait benchmark to the test suite, the system
  is capable of 81712 threads started/terminated per second on the reference
  LPC2148 board. The figure is inclusive of two context switch operations
  for each thread.
- Minor improvement in the LPC214x serial driver, unneeded events were
  generated in some rare cases.
- Fixed a chSysInit() documentation error.
- Fixed a chEvtWaitTimeout() documentation error.
- Added a new debug switch: CH_USE_TRACE, previously the trace functionality
  was associated to the CH_USE_DEBUG switch.

*** 0.4.2 ***
- Added a minimal ARM7-LPC demo, you can use this one as template in order to
  create your application. It is easier to add subsystems back to the small
  demo than remove stuff from the large one.
- Introduced support for "pure" THUMB mode, it is activated when all the
  source files are compiled in THUMB mode, the option -mthumb-interworking is
  not used in this scenario and this greatly improves both code size and
  speed.
  It is recommended to either use ARM mode or THUMB mode and not mix them
  unless you know exactly what you are doing. Mixing modes is still supported
  anyway.
- More optimizations in the scheduler, updated the performance spreadsheet.
- Fixed a problem with the thread working area declarations, the alignment to
  4 bytes boundary was not enforced. Now it is defined a new macro
  WorkingArea(name, length) that takes care of both the allocation and the
  alignment.
  Example:
    static WorkingArea(waThread1, 32);
  It is expanded as:
    ULONG32 waThread1[UserStackSpace(32) >> 2];
  Now the demos use the new declaration style.
- Fixed a small problem in sleep functions introduced in 0.4.1.

*** 0.4.1 ***
- Modified the initialization code in order to have a dedicated idle thread in
  the system, now the main() function behaves like a normal thread after
  executing chSysInit() and can use all the ChibiOS/RT APIs (it was required
  to run the idle loop in previous versions).
  Now it is also possible to use ChibiOS/RT with a single main() thread and
  just use it for the I/O capabilities, Virtual Timers and events. Now you
  don't have to use multiple threads if you don't really need to.
- Added a spreadsheet in the documentation that describes the advantages
  and disadvantages of the various optimization options (both GCC options and
  ChibiOS/RT options), very interesting read IMO.
- The GCC option +falign-functions=16 is now default in the Makefile, it is
  required because of the MAM unit into the LPC chips, without this option
  the code performance is less predictable and can change of some % points
  depending on how the code is aligned in the flash memory, unpredictability
  is bad for a RTOS. This option however increases the code size slightly
  because of the alignment gaps.
- Fine tuning in the scheduler code for improved performance, deeper
  inlining and other small changes, about 5% better scheduler performance.
- Increased the default system-mode stack size from 128 to 256 bytes because
  some optimizations and the THUMB mode seem to eat more stack space.
- Included a Makefile in the LPC2148 demo that builds in THUMB mode.
- Const-ified a parameter in the chEvtWait() and chEvtWaitTimeout() APIs.
- Removed the CPU register clearing on thread start, it was not really useful,
  it is better to maximize performance instead.
- Cleaned up the ARM port code. Now it is easier to understand.
- Cleaned up the LPC2148 demo in main.c, it is now well documented and
  explains everything, I assumed too much stuff to be "obvious".

*** 0.4.0 ***
- NEW, added a benchmark functionality to the test suite. The benchmark
  measures the kernel throughput as messages per second and context switches
  per second. The benchmark will be useful for fine tuning the compiler
  options and the kernel itself.
- NEW, implemented a debug subsystem, it supports debug messages and a context
  switch circular trace buffer. The debug code can be enabled/disabled by
  using the CH_USE_DEBUG in chconf.h.
  The trace buffer is meant to be fetched and decoded by an external tool
  (coming soon, it can be accessed using JTAG in the meanwhile).
- Added new API chThdGetPriority() as a macro.
- Implemented panic messages when CH_USE_DEBUG is enabled.
- Added a thread identifier field to the Thread structure, it is used only
  for debug.
- Global variable stime modified as volatile.
- API chSysGetTime() reimplemented as a macro.
- Fixed a regression with the CH_CURRP_REGISTER_CACHE option.
- Fixed a problem in the LPC2148 interrupt handling code, a spurious
  interrupts fix recommended on the NXP data sheet proved to be a very bad
  idea and not about real spurious interrupts also...
- Fixed an harmless warning message in buzzer.c.

*** 0.3.6 ***
- Added SSP (SPI1) and ext.interrupts definitions to the lpc214x.h file.
- Added SSP driver for the LPC2148.
- Added experimental MMC/SD block driver to the LPC2148 demo in order to
  support file systems. The driver features also events generation on card
  insert/remove, hot plugging supported.
- Added missing chThdSuspend() declararion in threads.h.

*** 0.3.5 ***
- Space optimization in events code.
- Changed the behavior of chEvtWaitTimeout() when the timeout parameter is
  set to zero, now it is consistent with all the other syscalls that have a
  timeout option.
- Reorganized all the kernel inline definitions into a single file (inline.h).
- Fixed a minor problem in the interrupt initialization code for the LPC214x
  demo, regrouped the VIC-specific code into vic.c/vic.h.
- Fixed a bug into the LPC2148 serial driver (limited to the serial port 2).
- Implemented HW transmit FIFO preloading in the LPC2148 serial driver in
  order to minimize the number of interrupts generated, it is possible to
  disable the feature and return to the old code which is a bit smaller, see
  the configuration parameters in ./ARM7-LPC214x/GCC/lpc214x_serial.h.
- Some more work done on the AVR port, it is almost complete but not tested
  yet because my JTAG probe broke...

*** 0.3.4 ***
- Fixed a problem in chVTSetI().
- New API, chVTIsArmedI(), it is a macro in delta.h.
- New API, chThdResumeI(), it is a macro in threads.h. This function is just
  an alias for chSchReadyI() but makes the code more readable.
- New API, chThdSuspend(). New switch CH_USE_SUSPEND added to chconf.h.

*** 0.3.3 ***
- Modified the chVTSetI(), now for the "time" parameter can have value zero
  with meaning "infinite". This allows all the APIs with timeout parameters
  to be invoked with no timeout.
- Fixes in the documentation.
- Renamed some APIs in the "Sch" group to have an S suffix instead of I.

*** 0.3.2 ***
- Modified the chSysInit() to give the idle thread absolute priority, the
  priority is then lowered to the minimum value into the chSysPause(). This
  is done in order to ensure that the initializations performed into the
  main() procedure are finished before any thread starts.
- Added chThdSetPriority() new API.
- Added a generic events generator timer modulee to the library code.
- Modified the ARM7-LPC214x-GCC demo to show the use of the event timer.
- Added the "#ifdef __cplusplus" stuff to the header files.
- Removed an obsolete definition in ./src/templates/chtypes.h.

*** 0.3.1 ***
- Test program added to the demos. Telnet the MinGW and MSVS demos and type
  "test" at the "ch>" prompt. On the LPC214x demo the test is activated by
  pressing both the board buttons. The test performs tests on the ChibiOS/RT
  functionalities.
  The test code is also a good example of APIs usage and ChibiOS/RT behavior.
- Fixed bug in chEvtWaitTimeout(), the timeout code performed an useless
  dequeue operation.
- Fixed a bug in chSemWaitTimeoutS() and chSemWaitTimeout(), the semaphore
  counter was not atomically updated on a timeout condition.
- Fixed bug on RT semaphores, the priority queuing was broken.
- Fixed a bug in the MinGW demo, the chThdExit() code was not correctly
  reported to the thread waiting in chThdWait().
- Fixed a function declaration in semaphores.h.
- Lists code moved into chlists.c from various other places optimized and
  reorganized.
- The list of the threads waiting in chThdWait() is now a single link list,
  this saves some space.
- Cleaned the template files code, the files contained some obsolete
  declarations.
- Code optimization in chSemWaitTimeoutS(), chSemWaitTimeout() and
  chSemSignalWait().
- Code optimization in chEvtSend().
- Code optimization in chVTDoTickI().
- Added a Semaphore pointer to the Thread structure, this allows to know on
  which semaphore a thread is waiting on. It takes no space because it is
  located in the union inside the Thread structure. This also allowed a minor
  optimization inside chSemWaitTimeout() and chSemWaitTimeoutS().
- Changed the priority type to unsigned in order to make it compatible
  with a byte value, this is very important for 8 bits architectures.
- Modified the MinGW and MSVS demos to use 1ms ticks instead of 10ms as
  before.

*** 0.3.0 ***
- ChibiOS/RT goes beta.
- Diet for the threads code, some simple APIs become macros.
- Thread Local Storage implemented as a single API: chThdLS().
  The API simply returns a pointer into the thread working area, see the
  documentation on the web site.
- Moved some documentation and images from the web site into the Doxigen
  generated HTMLs.

*** 0.2.1 ***
- Optimizations in the RT semaphores subsystem. The support for this
  subsystem should still be considered experimental and further changes may
  happen in future versions.
- Bug fix in the virtual timers handling code, now the timers can be re-armed
  from within the callback code in order to create periodic virtual timers.
- Modified the t_prio type in the demos to be 32bits wide instead of 16bits,
  this results in a better code in critical sections of the kernel.

*** 0.2.0 ***
- Introduced support for ARM in thumb mode.
- Optimized context switching when thumb-interworking is not required, one
  less instruction.
- Minor fixes to the ARM demo.

*** 0.1.1 ***
- Some fixes into the documentation
- Renamed the makefiles to Makefile, upper case M.

*** 0.1.0 ***
- First alpha release
