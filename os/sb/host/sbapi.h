/*
    ChibiOS - Copyright (C) 2006..2019 Giovanni Di Sirio.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    sb/sbapi.h
 * @brief   ARMv7-M sandbox host API macros and structures.
 *
 * @addtogroup ARMV7M_SANDBOX_HOSTAPI
 * @{
 */

#ifndef SBAPI_H
#define SBAPI_H

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

#define SB_SVC1_HANDLER         sb_api_exit
#define SB_SVC2_HANDLER         sb_api_systime
#define SB_SVC3_HANDLER         sb_api_sleep
#define SB_SVC4_HANDLER         sb_api_sleep_until_windowed

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  uint32_t sb_api_exit(struct port_extctx *ctxp);
  uint32_t sb_api_systime(struct port_extctx *ctxp);
  uint32_t sb_api_sleep(struct port_extctx *ctxp);
  uint32_t sb_api_sleep_until_windowed(struct port_extctx *ctxp);
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

#endif /* SBAPI_H */

/** @} */
