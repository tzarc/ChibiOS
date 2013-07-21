/*
    Nil RTOS - Copyright (C) 2012 Giovanni Di Sirio.

    This file is part of Nil RTOS.

    Nil RTOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Nil RTOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    STM32F0xx/niltimer.h
 * @brief   Nil RTOS STM32F0 system timer header file.
 *
 * @addtogroup STM32F0_TIMER
 * @{
 */

#ifndef _NILTIMER_H_
#define _NILTIMER_H_

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

typedef struct {
  volatile uint16_t     CR1;
  uint16_t              _resvd0;
  volatile uint16_t     CR2;
  uint16_t              _resvd1;
  volatile uint16_t     SMCR;
  uint16_t              _resvd2;
  volatile uint16_t     DIER;
  uint16_t              _resvd3;
  volatile uint16_t     SR;
  uint16_t              _resvd4;
  volatile uint16_t     EGR;
  uint16_t              _resvd5;
  volatile uint16_t     CCMR1;
  uint16_t              _resvd6;
  volatile uint16_t     CCMR2;
  uint16_t              _resvd7;
  volatile uint16_t     CCER;
  uint16_t              _resvd8;
  volatile uint32_t     CNT;
  volatile uint16_t     PSC;
  uint16_t              _resvd9;
  volatile uint32_t     ARR;
  volatile uint16_t     RCR;
  uint16_t              _resvd10;
  volatile uint32_t     CCR[4];
  volatile uint16_t     BDTR;
  uint16_t              _resvd11;
  volatile uint16_t     DCR;
  uint16_t              _resvd12;
  volatile uint16_t     DMAR;
  uint16_t              _resvd13;
  volatile uint16_t     OR;
  uint16_t              _resvd14;
} stm32f0_tim_t;

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

#define STM32F0_TIM2    ((stm32f0_tim_t *)0x40000000)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Timer unit initialization.
 *
 * @notapi
 */
static inline void port_timer_init(void) {

  STM32F0_TIM2->ARR     = 0xFFFFFFFF;
  STM32F0_TIM2->CCMR1   = 0;
  STM32F0_TIM2->CCR[0]  = 0;
  STM32F0_TIM2->DIER    = 0;
  STM32F0_TIM2->CR2     = 0;
  STM32F0_TIM2->EGR     = 1;            /* UG, CNT initialized.             */
  STM32F0_TIM2->CR1     = 1;            /* CEN */
}

/**
 * @brief   Returns the system time.
 *
 * @return              The system time.
 *
 * @notapi
 */
static inline systime_t port_timer_get_time(void) {

  return STM32F0_TIM2->CNT;
}

/**
 * @brief   Starts the alarm.
 * @note    Makes sure that no spurious alarms are triggered after
 *          this call.
 *
 * @param[in] time      the time to be set for the first alarm
 *
 * @notapi
 */
static inline void port_timer_start_alarm(systime_t time) {

  nilDbgAssert((STM32F0_TIM2->DIER & 2) == 0,
               "port_timer_start_alarm(), #1",
               "already started");

  STM32F0_TIM2->CCR[0]  = time;
  STM32F0_TIM2->SR      = 0;
  STM32F0_TIM2->DIER    = 2;            /* CC1IE */
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
static inline void port_timer_stop_alarm(void) {

  nilDbgAssert((STM32F0_TIM2->DIER & 2) != 0,
               "port_timer_stop_alarm(), #1",
               "not started");

  STM32F0_TIM2->DIER    = 0;
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] time      the time to be set for the next alarm
 *
 * @notapi
 */
static inline void port_timer_set_alarm(systime_t time) {

  nilDbgAssert((STM32F0_TIM2->DIER & 2) != 0,
               "port_timer_set_alarm(), #1",
               "not started");

  STM32F0_TIM2->CCR[0]  = time;
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
static inline systime_t port_timer_get_alarm(void) {

  nilDbgAssert((STM32F0_TIM2->DIER & 2) != 0,
               "port_timer_get_alarm(), #1",
               "not started");

  return STM32F0_TIM2->CCR[0];
}

#endif /* _NILTIMER_H_ */

/** @} */
