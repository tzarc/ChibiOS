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
 * @file chlists.c
 * @brief Lists and queues code.
 * @addtogroup ThreadLists
 * @{
 */
#include <ch.h>

#if !CH_OPTIMIZE_SPEED || defined(__DOXYGEN__)
/**
 * @brief Inserts a thread into a priority ordered queue.
 *
 * @param[in] tp the pointer to the thread to be inserted in the list
 * @param[in] tqp the pointer to the threads list header
 * @note The insertion is done by scanning the list from the highest priority
 *       toward the lowest.
 * @note This function is @b not an API.
 */
void prio_insert(Thread *tp, ThreadsQueue *tqp) {

  /* cp iterates over the queue */
  Thread *cp = (Thread *)tqp;
  do {
    /* iterate to next thread in queue */
    cp = cp->p_next;
    /* not end of queue? and cp has equal or higher priority than tp? */
  } while ((cp != (Thread *)tqp) && (cp->p_prio >= tp->p_prio));
  /* insert before cp, point tp to next and prev in queue */
  tp->p_prev = (tp->p_next = cp)->p_prev;
  /* make prev point to tp, and cp point back to tp */
  tp->p_prev->p_next = cp->p_prev = tp;
}

/**
 * @brief Inserts a Thread into a queue.
 *
 * @param[in] tp the pointer to the thread to be inserted in the list
 * @param[in] tqp the pointer to the threads list header
 * @note This function is @b not an API.
 */
void queue_insert(Thread *tp, ThreadsQueue *tqp) {

  tp->p_prev = (tp->p_next = (Thread *)tqp)->p_prev;
  tp->p_prev->p_next = tqp->p_prev = tp;
}

/**
 * @brief Removes the first-out Thread from a queue and returns it.
 *
 * @param[in] tqp the pointer to the threads list header
 * @return The removed thread pointer.
 * @note This function is @b not an API.
 */
Thread *fifo_remove(ThreadsQueue *tqp) {
  Thread *tp = tqp->p_next;

  (tqp->p_next = tp->p_next)->p_prev = (Thread *)tqp;
  return tp;
}

/**
 * @brief Removes the last-out Thread from a queue and returns it.
 *
 * @param[in] tqp the pointer to the threads list header
 * @return The removed thread pointer.
 * @note This function is @b not an API.
 */
Thread *lifo_remove(ThreadsQueue *tqp) {
  Thread *tp = tqp->p_next;

  (tqp->p_next = tp->p_next)->p_prev = (Thread *)tqp;
  return tp;
}

/**
 * @brief Removes a Thread from a FIFO list and returns it.
 *
 * @param[in] tp the pointer to the thread to be removed from the list
 * @return The removed thread pointer.
 * @note This function is @b not an API.
 */
Thread *dequeue(Thread *tp) {

  tp->p_prev->p_next = tp->p_next;
  tp->p_next->p_prev = tp->p_prev;
  return tp;
}
#endif /* CH_OPTIMIZE_SPEED */

/** @} */
