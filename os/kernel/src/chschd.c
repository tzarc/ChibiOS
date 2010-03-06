/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

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
 * @file    chschd.c
 * @brief   Scheduler code.
 *
 * @addtogroup scheduler
 * @{
 */

#include "ch.h"

/**
 * @brief   Ready list header.
 */
ReadyList rlist;

/**
 * @brief   Scheduler initialization.
 * @note    Internally invoked by the @p chSysInit(), not an API.
 */
void scheduler_init(void) {

  queue_init(&rlist.r_queue);
  rlist.r_prio = NOPRIO;
#if CH_TIME_QUANTUM > 0
  rlist.r_preempt = CH_TIME_QUANTUM;
#endif
#if CH_USE_REGISTRY
  rlist.p_newer = rlist.p_older = (Thread *)&rlist;
#endif
}

/**
 * @brief   Inserts a thread in the Ready List.
 * @note    The function does not reschedule, the @p chSchRescheduleS() should
 *          be called soon after.
 *
 * @param[in] tp        the Thread to be made ready
 * @return              The Thread pointer.
 */
#if CH_OPTIMIZE_SPEED
/* NOTE: it is inlined in this module only.*/
INLINE Thread *chSchReadyI(Thread *tp) {
#else
Thread *chSchReadyI(Thread *tp) {
#endif
  Thread *cp;

  tp->p_state = THD_STATE_READY;
  cp = (Thread *)&rlist.r_queue;
  do {
    cp = cp->p_next;
  } while (cp->p_prio >= tp->p_prio);
  /* Insertion on p_prev.*/
  tp->p_prev = (tp->p_next = cp)->p_prev;
  tp->p_prev->p_next = cp->p_prev = tp;
  return tp;
}

/**
 * @brief   Puts the current thread to sleep into the specified state.
 * @details The thread goes into a sleeping state. The @ref thread_states are
 *          described into @p threads.h.
 *
 * @param[in] newstate  the new thread state
 * @return              The @p currp value.
 */
Thread *chSchGoSleepS(tstate_t newstate) {
  Thread *otp;

  (otp = currp)->p_state = newstate;
  (currp = fifo_remove(&rlist.r_queue))->p_state = THD_STATE_CURRENT;
#if CH_TIME_QUANTUM > 0
  rlist.r_preempt = CH_TIME_QUANTUM;
#endif
  chDbgTrace(currp, otp);
  return chSysSwitchI(currp, otp);
}

/*
 * Timeout wakeup callback.
 */
static void wakeup(void *p) {
  Thread *tp = (Thread *)p;

#if CH_USE_SEMAPHORES || CH_USE_MUTEXES || CH_USE_CONDVARS
  switch (tp->p_state) {
#if CH_USE_SEMAPHORES
  case THD_STATE_WTSEM:
    chSemFastSignalI((Semaphore *)tp->p_u.wtobjp);
    /* Falls into, intentional. */
#endif
#if CH_USE_MUTEXES
  case THD_STATE_WTMTX:
#endif
#if CH_USE_CONDVARS
  case THD_STATE_WTCOND:
#endif
    /* States requiring dequeuing.*/
    dequeue(tp);
  }
#endif
  chSchReadyI(tp)->p_u.rdymsg = RDY_TIMEOUT;
}

/**
 * @brief   Puts the current thread to sleep into the specified state with
 *          timeout specification.
 * @details The thread goes into a sleeping state, if it is not awakened
 *          explicitly within the specified timeout then it is forcibly
 *          awakened with a @p RDY_TIMEOUT low level message. The @ref
 *          thread_states are described into @p threads.h.
 *
 * @param[in] newstate  the new thread state
 * @param[in] time      the number of ticks before the operation timeouts, the
 *                      special values are handled as follow:
 *                      - @a TIME_INFINITE the thread enters an infinite sleep
 *                        state, this is equivalent to invoking
 *                        @p chSchGoSleepS() but, of course, less efficient.
 *                      - @a TIME_IMMEDIATE this value is accepted but
 *                        interpreted as a normal time specification not as an
 *                        immediate timeout specification.
 *                      .
 * @return              The wakeup message.
 * @retval RDY_TIMEOUT if a timeout occurs.
 */
msg_t chSchGoSleepTimeoutS(tstate_t newstate, systime_t time) {

  if (TIME_INFINITE != time) {
    VirtualTimer vt;

    chVTSetI(&vt, time, wakeup, currp);
    chSchGoSleepS(newstate);
    if (chVTIsArmedI(&vt))
      chVTResetI(&vt);
  }
  else
    chSchGoSleepS(newstate);
  return currp->p_u.rdymsg;
}

/**
 * @brief   Wakes up a thread.
 * @details The thread is inserted into the ready list or immediately made
 *          running depending on its relative priority compared to the current
 *          thread.
 * @note    It is equivalent to a @p chSchReadyI() followed by a
 *          @p chSchRescheduleS() but much more efficient.
 * @note    The function assumes that the current thread has the highest
 *          priority.
 *
 * @param[in] ntp       the Thread to be made ready
 * @param[in] msg       message to the awakened thread
 * @return              The waken thread.
 */
Thread *chSchWakeupS(Thread *ntp, msg_t msg) {
  Thread *otp = currp;

  ntp->p_u.rdymsg = msg;
  /* If the waken thread has a not-greater priority than the current
     one then it is just inserted in the ready list else it made
     running immediately and the invoking thread goes in the ready
     list instead.*/
  if (ntp->p_prio <= otp->p_prio)
    return chSchReadyI(ntp);
  chSchReadyI(otp);
#if CH_TIME_QUANTUM > 0
  rlist.r_preempt = CH_TIME_QUANTUM;
#endif
  (currp = ntp)->p_state = THD_STATE_CURRENT;
  chDbgTrace(ntp, otp);
  chSysSwitchI(ntp, otp);
  return ntp;
}

/**
 * @brief   Switches to the first thread on the runnable queue.
 * @note    It is intended to be called if @p chSchRescRequiredI() evaluates
 *          to @p TRUE.
 */
void chSchDoRescheduleI(void) {
  Thread *otp = currp;

  /* Pick the first thread from the ready queue and makes it current.*/
  (currp = fifo_remove(&rlist.r_queue))->p_state = THD_STATE_CURRENT;
  chSchReadyI(otp);
#if CH_TIME_QUANTUM > 0
  rlist.r_preempt = CH_TIME_QUANTUM;
#endif
  chDbgTrace(currp, otp);
  chSysSwitchI(currp, otp);
}

/**
 * @brief   Performs a reschedulation if a higher priority thread is runnable.
 * @details If a thread with a higher priority than the current thread is in
 *          the ready list then make the higher priority thread running.
 */
void chSchRescheduleS(void) {

  if (chSchIsRescRequiredI())
    chSchDoRescheduleI();
}

/**
 * @brief   Evaluates if a reschedulation is required.
 * @details The decision is taken by comparing the relative priorities and
 *          depending on the state of the round robin timeout counter.
 * @note    This function is meant to be used in the timer interrupt handler
 *          where @p chVTDoTickI() is invoked.
 *
 * @retval TRUE         if there is a thread that should go in running state.
 * @retval FALSE        if a reschedulation is not required.
 */
bool_t chSchIsRescRequiredExI(void) {
  tprio_t p1 = firstprio(&rlist.r_queue);
  tprio_t p2 = currp->p_prio;
#if CH_TIME_QUANTUM > 0
  /* If the running thread has not reached its time quantum, reschedule only
     if the first thread on the ready queue has a higher priority.
     Otherwise, if the running thread has used up its time quantum, reschedule
     if the first thread on the ready queue has equal or higher priority.*/
  return rlist.r_preempt ? p1 > p2 : p1 >= p2;
#else
  /* If the round robin preemption feature is not enabled then performs a
     simpler comparison.*/
  return p1 > p2;
#endif
}

/**
 * @brief   Yields the time slot.
 * @details Yields the CPU control to the next thread in the ready list with
 *          equal priority, if any.
 */
void chSchDoYieldS(void) {

  if (chSchCanYieldS()) {
    Thread *cp = (Thread *)&rlist.r_queue;
    Thread *otp = currp;

    /* Note, the following insertion code works because we know that on the
       ready list there is at least one thread with priority equal or higher
       than the current one.*/
    otp->p_state = THD_STATE_READY;
    do {
      cp = cp->p_prev;
    } while (cp->p_prio < otp->p_prio);
    /* Insertion on p_next.*/
    otp->p_next = (otp->p_prev = cp)->p_next;
    otp->p_next->p_prev = cp->p_next = otp;

    /* Pick the first thread from the ready queue and makes it current.*/
    (currp = fifo_remove(&rlist.r_queue))->p_state = THD_STATE_CURRENT;
#if CH_TIME_QUANTUM > 0
    rlist.r_preempt = CH_TIME_QUANTUM;
#endif
    chDbgTrace(currp, otp);
    chSysSwitchI(currp, otp);
  }
}

/** @} */
