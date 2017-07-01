*****************************************************************************
*** Files Organization                                                    ***
*****************************************************************************

--{root}                  - ChibiOS directory.
  +--readme.txt           - This file.
  +--documentation.html   - Shortcut to the web documentation page.
  +--license.txt          - GPL license text.
  +--demos/               - Demo projects, one directory per platform.
  +--docs/                - Documentation.
  |  +--common/           - Documentation common build resources.
  |  +--hal/              - Builders for HAL.
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--html/          - Local HTML documentation (after rebuild).
  |  |  +--reports/       - Test reports.
  |  |  +--rsc/           - Documentation resource files (required for rebuild).
  |  |  +--src/           - Documentation source files (required for rebuild).
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--index.html     - Local documentation access (after rebuild).
  |  +--nil/              - Builders for NIL.
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--html/          - Local HTML documentation (after rebuild).
  |  |  +--reports/       - Test reports.
  |  |  +--rsc/           - Documentation resource files (required for rebuild).
  |  |  +--src/           - Documentation source files (required for rebuild).
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--index.html     - Local documentation access (after rebuild).
  |  +--rt/               - Builders for RT.
  |  |  +--html/          - Local HTML documentation (after rebuild).
  |  |  +--reports/       - Test reports.
  |  |  +--rsc/           - Documentation resource files (required for rebuild).
  |  |  +--src/           - Documentation source files (required for rebuild).
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--index.html     - Local documentation access (after rebuild).
  +--ext/                 - External libraries, not part of ChibiOS/RT.
  +--os/                  - ChibiOS components.
  |  +--common/           - Shared OS modules.
  |  |  +--abstractions/  - API emulator wrappers.
  |  |  |  +--cmsis_os/   - CMSIS OS emulation layer for RT (ARMCMx port only).
  |  |  |  +--nasa_osal/  - NASA Operating System Abstraction Layer for RT.
  |  |  +--ext/           - Vendor files used by the OS.
  |  |  +--oslib/         - RTOS modules usable by both RT and NIL.
  |  |  +--ports/         - RTOS ports usable by both RT and NIL.
  |  |  +--startup/       - Startup support for all compilers and platforms.
  |  +--ex/               - EX component.
  |  |  +--Micron/        - EX complex drivers for Micron devices.
  |  |  +--ST/            - EX complex drivers for STMicroelectronics devices.
  |  |  +--subsystems/    - EX subsystems.
  |  |  |  +--mfs/        - EX Managed Flash Storage module.
  |  +--hal/              - HAL component.
  |  |  +--boards/        - HAL board support files.
  |  |  +--dox/           - HAL documentation resources.
  |  |  +--include/       - HAL high level headers.
  |  |  +--lib/           - HAL libraries.
  |  |  |  +--fallback/   - HAL fall back software drivers.
  |  |  |  +--peripherals/- HAL peripherals interfaces.
  |  |  |  +--streams/    - HAL streams.
  |  |  +--osal/          - HAL OSAL implementations.
  |  |  |  +--lib/        - HAL OSAL common modules.
  |  |  +--src/           - HAL high level source.
  |  |  +--ports/         - HAL ports.
  |  |  +--templates/     - HAL driver template files.
  |  |     +--osal/       - HAL OSAL templates.
  |  +--nil/              - NIL RTOS component.
  |  |  +--dox/           - NIL documentation resources.
  |  |  +--include/       - NIL high level headers.
  |  |  +--src/           - NIL high level source.
  |  |  +--templates/     - NIL configuration template files.
  |  +--rt/               - RT RTOS component.
  |  |  +--dox/           - RT documentation resources.
  |  |  +--include/       - RT high level headers.
  |  |  +--src/           - RT high level source.
  |  |  +--templates/     - RT configuration template files.
  |  +--various/          - Various portable support files.
  +--test/                - Kernel test suite source code.
  |  +--lib/              - Portable test engine.
  |  +--hal/              - HAL test suites.
  |  |  +--testbuild/     - HAL build test and MISRA check.
  |  +--nil/              - NIL test suites.
  |  |  +--testbuild/     - NIL build test and MISRA check.
  |  +--rt/               - RT test suites.
  |  |  +--testbuild/     - RT build test and MISRA check.
  |  |  +--coverage/      - RT code coverage project.
  +--testhal/             - HAL integration test demos.

*****************************************************************************
*** Releases and Change Log                                               ***
*****************************************************************************

*** 17.6.1 ***
- NIL: Fixed documentation-related issues (bug #850).
- RT:  Fixed documentation-related issues (bug #850).
- RT:  Fixed leftover chcustomer.h file (bug #849).
- RT:  Fixed invalid check in chchecks.h (bug #848).
- HAL: Fixed STM32F070xB: USART invalid DMA channels (bug #847).
- VAR: Fixed CMSIS_OS issue in timers (bug #846).

*** 17.6.0 ***
- First 17.6.0 release, see release note 17.6.0.
