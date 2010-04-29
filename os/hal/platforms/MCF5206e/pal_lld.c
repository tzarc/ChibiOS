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
 * @file COLDFIRE/pal_lld.c
 * @brief COLDFIRE GPIO low level driver template.
 * @addtogroup COLDFIRE_LLD
 * @{
 */

#include "ch.h"
#include "hal.h"

#if CH_HAL_USE_PAL || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief COLDFIRE GPIO ports configuration.
 * @details GPIO registers initialization.
 *
 * @param[in] config the COLDFIRE ports configuration
 */
void _pal_lld_init(const MCF5206eGPIOConfig *config) {

  sim->pp.PPDDR = config->P0data.dir;
  sim->pp.PPDAT = config->P0data.out;
}

/**
 * @brief Pads mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 *
 * @param[in] port the port identifier
 * @param[in] mask the group mask
 * @param[in] mode the mode
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 * @note @p PAL_MODE_UNCONNECTED is implemented as output as recommended by
 *       the MSP430x1xx Family User's Guide. Unconnected pads are set to
 *       high logic state by default.
 * @note This function does not alter the @p PxSEL registers. Alternate
 *       functions setup must be handled by device-specific code.
 */
// TODO: implement PAL_MODE_UNCONNECTED mode recommended for coldfire
void _pal_lld_setgroupmode(ioportid_t port, ioportmask_t mask, uint_fast8_t mode) {

  switch (mode) {
  case PAL_MODE_RESET:
  case PAL_MODE_INPUT:
    port->PPDDR &= ~mask;
    break;
//  case PAL_MODE_UNCONNECTED:
//    port->iop_common.out.reg_p |= mask;
  case PAL_MODE_OUTPUT_PUSHPULL:
    port->PPDDR |= mask;
    break;
  }
}

#endif /* CH_HAL_USE_PAL */

/** @} */
