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

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for a generic STM8S20x board.
 */

/*
 * Board identifiers.
 */
#define BOARD_STM8S20x
#define BOARD_NAME "Generic STM8S20x"

/*
 * Board frequencies.
 */
#define HSECLK          24000000

/*
 * MCU model used on the board.
 * The available models are listed in the file ./os/hal/platforms/stm8/stm8.h
 */
#define STM8_PLATFORM   PLATFORM_STM8S208RB

#ifdef __cplusplus
extern "C" {
#endif
  void hwinit(void);
#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H_ */
