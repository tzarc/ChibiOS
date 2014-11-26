/*
    ChibiOS - Copyright (C) 2006,2007,2008,2009,2010,
              2011,2012,2013,2014 Giovanni Di Sirio.

    This file is part of ChibiOS 

    A limited, non exclusive, license has been granted to STMicroelectronics
    for use in SPC5xx micro controllers embedded firmware.
*/

/**
 * @file    osal.c
 * @brief   OSAL module code.
 *
 * @addtogroup OSAL
 * @{
 */

#include "osal.h"

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Dequeues and wakes up one thread from the queue, if any.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadDequeueNextI(threads_queue_t *tqp, msg_t msg) {
  semaphore_t *sp = &tqp->sem;

  if (chSemGetCounterI(&tqp->sem) < 0) {
    thread_reference_t tr = nil.threads;
    while (true) {
      /* Is this thread waiting on this semaphore?*/
      if (tr->u1.semp == sp) {
        sp->cnt++;

        chDbgAssert(NIL_THD_IS_WTSEM(tr), "not waiting");

        chSchReadyI(tr, msg);
        return;
      }
      tr++;

      chDbgAssert(tr < &nil.threads[NIL_CFG_NUM_THREADS],
                  "pointer out of range");
    }
  }
}

/**
 * @brief   Dequeues and wakes up all threads from the queue.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadDequeueAllI(threads_queue_t *tqp, msg_t msg) {
  semaphore_t *sp = &tqp->sem;
  thread_reference_t tr;
  cnt_t cnt;

  cnt = sp->cnt;
  sp->cnt = 0;
  tr = nil.threads;
  while (cnt < 0) {
    /* Is this thread waiting on this semaphore?*/
    if (tr->u1.semp == sp) {

      chDbgAssert(NIL_THD_IS_WTSEM(tr), "not waiting");

      cnt++;
      chSchReadyI(tr, msg);
    }
    tr++;

    chDbgAssert(tr < &nil.threads[NIL_CFG_NUM_THREADS],
                "pointer out of range");
  }
}

/** @} */
