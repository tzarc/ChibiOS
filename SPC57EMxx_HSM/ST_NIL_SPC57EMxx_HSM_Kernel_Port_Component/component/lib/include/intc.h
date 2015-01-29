/*
    ChibiOS - Copyright (C) 2006,2007,2008,2009,2010,
              2011,2012,2013,2014 Giovanni Di Sirio.

    This file is part of ChibiOS 

    A limited, non exclusive, license has been granted to STMicroelectronics
    for use in SPC5xx micro controllers embedded firmware.
*/

 /* parasoft suppress item MISRA2012-DIR-4_9 "MACRO are used to define
    peripheral register addresses with best performances" */

/**
 * @file    SPC57EMxx_HSM/intc.h
 * @brief   SPC57EMxx_HSM INTC module header.
 *
 * @addtogroup INTC
 * @{
 */

#ifndef _INTC_H_
#define _INTC_H_

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @name    INTC addresses
 * @{
 */
#define INTC_BASE           0xA3F48000
#define INTC_IACKR_ADDR     (INTC_BASE + 0x20)
#define INTC_EOIR_ADDR      (INTC_BASE + 0x30)
/** @} */

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

/**
 * @name    INTC-related macros
 * @{
 */
#define INTC_BCR            (*((volatile uint32_t *)(INTC_BASE + 0)))
#define INTC_MPROT          (*((volatile uint32_t *)(INTC_BASE + 4)))
#define INTC_CPR(n)         (*((volatile uint32_t *)(INTC_BASE + 0x10 + ((n) * sizeof (uint32_t)))))
#define INTC_IACKR(n)       (*((volatile uint32_t *)(INTC_BASE + 0x20 + ((n) * sizeof (uint32_t)))))
#define INTC_EOIR(n)        (*((volatile uint32_t *)(INTC_BASE + 0x30 + ((n) * sizeof (uint32_t)))))
#define INTC_PSR(n)         (*((volatile uint16_t *)(INTC_BASE + 0x60 + ((n) * sizeof (uint16_t)))))
/** @} */

/**
 * @brief   Core selection macros for PSR register.
 */
#define INTC_PSR_CORE4      0x8000

/**
 * @brief   PSR register content helper
 */
#define INTC_PSR_ENABLE(cores, prio) ((uint32_t)(cores) | (uint32_t)(prio))

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

#endif /* _INTC_H_ */

/** @} */
