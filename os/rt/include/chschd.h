/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio.

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
 * @file    rt/include/chschd.h
 * @brief   Scheduler macros and structures.
 *
 * @addtogroup scheduler
 * @{
 */

#ifndef CHSCHD_H
#define CHSCHD_H

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @name    Wakeup status codes
 * @{
 */
#define MSG_OK              (msg_t)0    /**< @brief Normal wakeup message.  */
#define MSG_TIMEOUT         (msg_t)-1   /**< @brief Wakeup caused by a timeout
                                             condition.                     */
#define MSG_RESET           (msg_t)-2   /**< @brief Wakeup caused by a reset
                                             condition.                     */
/** @} */

/**
 * @name    Priority constants
 * @{
 */
#define NOPRIO              (tprio_t)0      /**< @brief Ready list header
                                                 priority.                  */
#define IDLEPRIO            (tprio_t)1      /**< @brief Idle priority.      */
#define LOWPRIO             (tprio_t)2      /**< @brief Lowest priority.    */
#define NORMALPRIO          (tprio_t)128    /**< @brief Normal priority.    */
#define HIGHPRIO            (tprio_t)255    /**< @brief Highest priority.   */
/** @} */

/**
 * @name    Thread states
 * @{
 */
#define CH_STATE_READY      (tstate_t)0      /**< @brief Waiting on the
                                                  ready list.               */
#define CH_STATE_CURRENT    (tstate_t)1      /**< @brief Currently running. */
#define CH_STATE_WTSTART    (tstate_t)2      /**< @brief Just created.      */
#define CH_STATE_SUSPENDED  (tstate_t)3      /**< @brief Suspended state.   */
#define CH_STATE_QUEUED     (tstate_t)4      /**< @brief On a queue.        */
#define CH_STATE_WTSEM      (tstate_t)5      /**< @brief On a semaphore.    */
#define CH_STATE_WTMTX      (tstate_t)6      /**< @brief On a mutex.        */
#define CH_STATE_WTCOND     (tstate_t)7      /**< @brief On a cond.variable.*/
#define CH_STATE_SLEEPING   (tstate_t)8      /**< @brief Sleeping.          */
#define CH_STATE_WTEXIT     (tstate_t)9      /**< @brief Waiting a thread.  */
#define CH_STATE_WTOREVT    (tstate_t)10     /**< @brief One event.         */
#define CH_STATE_WTANDEVT   (tstate_t)11     /**< @brief Several events.    */
#define CH_STATE_SNDMSGQ    (tstate_t)12     /**< @brief Sending a message,
                                                  in queue.                 */
#define CH_STATE_SNDMSG     (tstate_t)13     /**< @brief Sent a message,
                                                  waiting answer.           */
#define CH_STATE_WTMSG      (tstate_t)14     /**< @brief Waiting for a
                                                  message.                  */
#define CH_STATE_FINAL      (tstate_t)15     /**< @brief Thread terminated. */

/**
 * @brief   Thread states as array of strings.
 * @details Each element in an array initialized with this macro can be
 *          indexed using the numeric thread state values.
 */
#define CH_STATE_NAMES                                                     \
  "READY", "CURRENT", "WTSTART", "SUSPENDED", "QUEUED", "WTSEM", "WTMTX",  \
  "WTCOND", "SLEEPING", "WTEXIT", "WTOREVT", "WTANDEVT", "SNDMSGQ",        \
  "SNDMSG", "WTMSG", "FINAL"
/** @} */

/**
 * @name    Thread flags and attributes
 * @{
 */
#define CH_FLAG_MODE_MASK   (tmode_t)3U     /**< @brief Thread memory mode
                                                 mask.                      */
#define CH_FLAG_MODE_STATIC (tmode_t)0U     /**< @brief Static thread.      */
#define CH_FLAG_MODE_HEAP   (tmode_t)1U     /**< @brief Thread allocated
                                                 from a Memory Heap.        */
#define CH_FLAG_MODE_MPOOL  (tmode_t)2U     /**< @brief Thread allocated
                                                 from a Memory Pool.        */
#define CH_FLAG_TERMINATE   (tmode_t)4U     /**< @brief Termination requested
                                                 flag.                      */
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
 * @brief   Returns the priority of the first thread on the given ready list.
 *
 * @notapi
 */
#define firstprio(rlp)  ((rlp)->next->prio)

/**
 * @brief   Current thread pointer access macro.
 * @note    This macro is not meant to be used in the application code but
 *          only from within the kernel, use @p chThdGetSelfX() instead.
 */
#define currthread      currcore->rlist.current

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/*
 * Scheduler APIs.
 */
#ifdef __cplusplus
extern "C" {
#endif
  void chSchObjectInit(os_instance_t *oip,
                       const os_instance_config_t *oicp);
  thread_t *__sch_ready_behind(os_instance_t *oip, thread_t *tp);
  thread_t *__sch_ready_ahead(thread_t *tp);
  void __sch_reschedule_behind(void);
  void __sch_reschedule_ahead(void);
  thread_t *chSchReadyI(thread_t *tp);
  void chSchGoSleepS(tstate_t newstate);
  msg_t chSchGoSleepTimeoutS(tstate_t newstate, sysinterval_t timeout);
  void chSchWakeupS(thread_t *ntp, msg_t msg);
  void chSchRescheduleS(void);
  bool chSchIsPreemptionRequired(void);
  void chSchDoPreemption(void);
#if CH_CFG_OPTIMIZE_SPEED == FALSE
  void queue_prio_insert(thread_t *tp, threads_queue_t *tqp);
  void queue_insert(thread_t *tp, threads_queue_t *tqp);
  thread_t *queue_fifo_remove(threads_queue_t *tqp);
  thread_t *queue_lifo_remove(threads_queue_t *tqp);
  thread_t *queue_dequeue(thread_t *tp);
  void list_insert(thread_t *tp, threads_list_t *tlp);
  thread_t *list_remove(threads_list_t *tlp);
#endif /* CH_CFG_OPTIMIZE_SPEED == FALSE */
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Threads list initialization.
 *
 * @param[in] tlp       pointer to the threads list object
 *
 * @notapi
 */
static inline void list_init(threads_list_t *tlp) {

  tlp->next = (thread_t *)tlp;
}

/**
 * @brief   Evaluates to @p true if the specified threads list is empty.
 *
 * @param[in] tlp       pointer to the threads list object
 * @return              The status of the list.
 *
 * @notapi
 */
static inline bool list_isempty(threads_list_t *tlp) {

  return (bool)(tlp->next == (thread_t *)tlp);
}

/**
 * @brief   Evaluates to @p true if the specified threads list is not empty.
 *
 * @param[in] tlp       pointer to the threads list object
 * @return              The status of the list.
 *
 * @notapi
 */
static inline bool list_notempty(threads_list_t *tlp) {

  return (bool)(tlp->next != (thread_t *)tlp);
}

/**
 * @brief   Threads queue initialization.
 *
 * @param[in] tqp       pointer to the threads queue object
 *
 * @notapi
 */
static inline void queue_init(threads_queue_t *tqp) {

  tqp->next = (thread_t *)tqp;
  tqp->prev = (thread_t *)tqp;
}

/**
 * @brief   Evaluates to @p true if the specified threads queue is empty.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @return              The status of the queue.
 *
 * @notapi
 */
static inline bool queue_isempty(const threads_queue_t *tqp) {

  return (bool)(tqp->next == (const thread_t *)tqp);
}

/**
 * @brief   Evaluates to @p true if the specified threads queue is not empty.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @return              The status of the queue.
 *
 * @notapi
 */
static inline bool queue_notempty(const threads_queue_t *tqp) {

  return (bool)(tqp->next != (const thread_t *)tqp);
}

/* If the performance code path has been chosen then all the following
   functions are inlined into the various kernel modules.*/
#if CH_CFG_OPTIMIZE_SPEED == TRUE
static inline void list_insert(thread_t *tp, threads_list_t *tlp) {

  tp->queue.next = tlp->next;
  tlp->next = tp;
}

static inline thread_t *list_remove(threads_list_t *tlp) {

  thread_t *tp = tlp->next;
  tlp->next = tp->queue.next;

  return tp;
}

static inline void queue_prio_insert(thread_t *tp, threads_queue_t *tqp) {

  thread_t *cp = (thread_t *)tqp;
  do {
    cp = cp->queue.next;
  } while ((cp != (thread_t *)tqp) && (cp->prio >= tp->prio));
  tp->queue.next             = cp;
  tp->queue.prev             = cp->queue.prev;
  tp->queue.prev->queue.next = tp;
  cp->queue.prev             = tp;
}

static inline void queue_insert(thread_t *tp, threads_queue_t *tqp) {

  tp->queue.next             = (thread_t *)tqp;
  tp->queue.prev             = tqp->prev;
  tp->queue.prev->queue.next = tp;
  tqp->prev                  = tp;
}

static inline thread_t *queue_fifo_remove(threads_queue_t *tqp) {
  thread_t *tp = tqp->next;

  tqp->next             = tp->queue.next;
  tqp->next->queue.prev = (thread_t *)tqp;

  return tp;
}

static inline thread_t *queue_lifo_remove(threads_queue_t *tqp) {
  thread_t *tp = tqp->prev;

  tqp->prev             = tp->queue.prev;
  tqp->prev->queue.next = (thread_t *)tqp;

  return tp;
}

static inline thread_t *queue_dequeue(thread_t *tp) {

  tp->queue.prev->queue.next = tp->queue.next;
  tp->queue.next->queue.prev = tp->queue.prev;

  return tp;
}
#endif /* CH_CFG_OPTIMIZE_SPEED == TRUE */

/**
 * @brief   Determines if the current thread must reschedule.
 * @details This function returns @p true if there is a ready thread with
 *          higher priority.
 *
 * @return              The priorities situation.
 * @retval false        if rescheduling is not necessary.
 * @retval true         if there is a ready thread at higher priority.
 *
 * @iclass
 */
static inline bool chSchIsRescRequiredI(void) {

  chDbgCheckClassI();

  return firstprio(&currcore->rlist.queue) > currthread->prio;
}

/**
 * @brief   Determines if yielding is possible.
 * @details This function returns @p true if there is a ready thread with
 *          equal or higher priority.
 *
 * @return              The priorities situation.
 * @retval false        if yielding is not possible.
 * @retval true         if there is a ready thread at equal or higher priority.
 *
 * @sclass
 */
static inline bool chSchCanYieldS(void) {

  chDbgCheckClassS();

  return firstprio(&currcore->rlist.queue) >= currthread->prio;
}

/**
 * @brief   Yields the time slot.
 * @details Yields the CPU control to the next thread in the ready list with
 *          equal or higher priority, if any.
 *
 * @sclass
 */
static inline void chSchDoYieldS(void) {

  chDbgCheckClassS();

  if (chSchCanYieldS()) {
    __sch_reschedule_behind();
  }
}

/**
 * @brief   Inline-able preemption code.
 * @note    Not a user function, it is meant to be invoked from within
 *          the port layer in the IRQ-related preemption code.
 *
 * @special
 */
static inline void chSchPreemption(void) {
  tprio_t p1 = firstprio(&currcore->rlist.queue);
  tprio_t p2 = currthread->prio;

#if CH_CFG_TIME_QUANTUM > 0
  if (currthread->ticks > (tslices_t)0) {
    if (p1 > p2) {
      __sch_reschedule_ahead();
    }
  }
  else {
    if (p1 >= p2) {
      __sch_reschedule_behind();
    }
  }
#else /* CH_CFG_TIME_QUANTUM == 0 */
  if (p1 > p2) {
    __sch_reschedule_ahead();
  }
#endif /* CH_CFG_TIME_QUANTUM == 0 */
}

#endif /* CHSCHD_H */

/** @} */
