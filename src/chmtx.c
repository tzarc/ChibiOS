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
 * @addtogroup Mutexes
 * @{
 */

#include <ch.h>

#ifdef CH_USE_MUTEXES

/**
 * Initializes s \p Mutex structure.
 * @param mp pointer to a \p Mutex structure
 */
void chMtxInit(Mutex *mp) {

  fifo_init(&mp->m_queue);
  mp->m_owner = NULL;
}

/**
 * Locks the specified mutex.
 * @param mp pointer to the \p Mutex structure
 */
void chMtxLock(Mutex *mp) {

  chSysLock();

  chMtxLockS(mp);

  chSysUnlock();
}

/**
 * Locks the specified mutex.
 *
 * @param mp pointer to the \p Mutex structure
 * @note This function must be called within a \p chSysLock() / \p chSysUnlock()
 *       block.
 */
void chMtxLockS(Mutex *mp) {
  /* the mutex is already locked? */
  if (mp->m_owner != NULL) {
    /*
     * Priority inheritance protocol; explores the thread-mutex dependencies
     * boosting the priority of all the affected threads to equal the priority
     * of the running thread requesting the mutex.
     */
    Thread *tp = mp->m_owner;
    /* { tp is the thread currently owning the mutex } */
    /* the running thread has higher priority than tp? */
    while (tp->p_prio < currp->p_prio) {
      /* make priority of thread tp match the running thread's priority */
      tp->p_prio = currp->p_prio;
      /*
       * The following states need priority queues reordering.
       */
      switch (tp->p_state) {
      /* thread tp is waiting on a mutex? */
      case PRWTMTX:
        /* requeue tp with its new priority on the mutex wait queue */
        prio_insert(dequeue(tp), &tp->p_wtmtxp->m_queue);
        /* boost the owner of this mutex if needed */
        tp = tp->p_wtmtxp->m_owner;
        continue;
#ifdef CH_USE_MESSAGES_PRIORITY
      case PRSNDMSG:
        /* requeue tp with its new priority on (?) */
        prio_insert(dequeue(tp), &tp->p_wtthdp->p_msgqueue);
        break;
#endif
      /* thread tp is ready? */
      case PRREADY:
        /* requeue tp with its new priority on the ready list */
        chSchReadyI(dequeue(tp));
      }
      break;
    }
    /* sleep on the mutex */
    prio_insert(currp, &mp->m_queue);
    /* thread remembers the mutex where it is waiting on */
    currp->p_wtmtxp = mp;
    chSchGoSleepS(PRWTMTX);
    chDbgAssert(mp->m_owner == NULL, "chmtx.c, chMtxLockS()");
  }
  /*
   * The mutex is now inserted in the owned mutexes list.
   */
  mp->m_owner = currp;
  mp->m_next = currp->p_mtxlist;
  currp->p_mtxlist = mp;
}

/**
 * Tries to lock a mutex. This function does not have any overhead related to
 * the priority inheritance mechanism because it does not try to enter a sleep
 * state on the mutex.
 * @param mp pointer to the \p Mutex structure
 * @return \p TRUE if the mutex was successfully acquired else \p FALSE
 */
bool_t chMtxTryLock(Mutex *mp) {
  bool_t b;

  chSysLock();

  b = chMtxTryLockS(mp);

  chSysUnlock();
  return b;
}

/**
 * Tries to lock a mutex. This function does not have any overhead related to
 * the priority inheritance mechanism because it does not try to enter a sleep
 * state on the mutex.
 * @param mp pointer to the \p Mutex structure
 * @return \p TRUE if the mutex was successfully acquired else \p FALSE
 * @note This function must be called within a \p chSysLock() / \p chSysUnlock()
 *       block.
 */
bool_t chMtxTryLockS(Mutex *mp) {

  if (mp->m_owner != NULL)
    return FALSE;
  mp->m_owner = currp;
  mp->m_next = currp->p_mtxlist;
  currp->p_mtxlist = mp;
  return TRUE;
}

/**
 * Unlocks the next owned mutex in reverse lock order.
 */
Mutex *chMtxUnlock(void) {
  Mutex *ump, *mp;

  chSysLock();

  chDbgAssert((currp->p_mtxlist != NULL) && (currp->p_mtxlist->m_owner == currp),
              "chmtx.c, chMtxUnlock()");

  /* remove the top Mutex from the Threads's owned mutexes list */
  ump = currp->p_mtxlist;
  currp->p_mtxlist = ump->m_next;
  /* mark the Mutex as not owned */
  ump->m_owner = NULL;
  /*
   * If a thread is waiting on the mutex then the hard part begins.
   */
  if (chMtxQueueNotEmptyS(ump)) {
    /* get the highest priority thread waiting for the unlocked mutex */
    Thread *tp = fifo_remove(&ump->m_queue);
    /*
     * Recalculates the optimal thread priority by scanning the owned mutexes list.
     */
    tprio_t newprio = currp->p_realprio;
    /* iterate mp over all the (other) mutexes the current thread still owns */
    mp = currp->p_mtxlist;
    while (mp != NULL) {
      /* mutex mp has a higher priority thread pending? */
      if (chMtxQueueNotEmptyS(mp) && (mp->m_queue.p_next->p_prio > newprio))
        /* boost current thread's priority to waiting thread */
        newprio = mp->m_queue.p_next->p_prio;
      mp = mp->m_next;
    }
    /* (possibly) boost the priority of the current thread */
    currp->p_prio = newprio;
    /* awaken the highest priority thread waiting for the unlocked mutex */
    chSchWakeupS(tp, RDY_OK);
  }
  chSysUnlock();
  return ump;
}

/**
 * Unlocks the next owned mutex in reverse lock order.
 * @note This function must be called within a \p chSysLock() / \p chSysUnlock()
 *       block.
 * @note This function does not reschedule internally.
 */
Mutex *chMtxUnlockS(void) {
  Mutex *ump, *mp;

  chDbgAssert((currp->p_mtxlist != NULL) && (currp->p_mtxlist->m_owner == currp),
              "chmtx.c, chMtxUnlockS()");

  /*
   * Removes the top Mutex from the owned mutexes list and marks it as not owned.
   */
  ump = currp->p_mtxlist;
  currp->p_mtxlist = ump->m_next;
  ump->m_owner = NULL;
  /*
   * If a thread is waiting on the mutex then the hard part begins.
   */
  if (chMtxQueueNotEmptyS(ump)) {
    Thread *tp = fifo_remove(&ump->m_queue);
    /*
     * Recalculates the optimal thread priority by scanning the owned mutexes list.
     */
    tprio_t newprio = currp->p_realprio;
    mp = currp->p_mtxlist;
    while (mp != NULL) {
      if (chMtxQueueNotEmptyS(mp) && (mp->m_queue.p_next->p_prio > newprio))
        newprio = mp->m_queue.p_next->p_prio;
      mp = mp->m_next;
    }
    currp->p_prio = newprio;
    chSchReadyI(tp);
  }
  return ump;
}

/**
 * Unlocks all the mutexes owned by the invoking thread, this is <b>MUCH MORE</b>
 * efficient than releasing the mutexes one by one and not just because the
 * call overhead, this function does not have any overhead related to the
 * priority inheritance mechanism.
 */
void chMtxUnlockAll(void) {

  chSysLock();

  if (currp->p_mtxlist != NULL) {
    do {
      Mutex *mp = currp->p_mtxlist;
      currp->p_mtxlist = mp->m_next;
      mp->m_owner = NULL;
      if (chMtxQueueNotEmptyS(mp))
        chSchReadyI(fifo_remove(&mp->m_queue));
    } while (currp->p_mtxlist != NULL);
    currp->p_prio = currp->p_realprio;
    chSchRescheduleS();
  }

  chSysUnlock();
}

#endif /* CH_USE_MUTEXES */

/** @} */
