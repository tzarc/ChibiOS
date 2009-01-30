/*
    ChibiOS/RT - Copyright (C) 2009 Giovanni Di Sirio.

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

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @addtogroup Mutexes
 * @{
 */

#ifndef _MUTEXES_H_
#define _MUTEXES_H_

#ifdef CH_USE_MUTEXES

/**
 * Mutex structure.
 */
typedef struct Mutex {
  /** Queue of the threads sleeping on this Mutex.*/
  ThreadsQueue          m_queue;
  /** Owner \p Thread pointer or \p NULL.*/
  Thread                *m_owner;
  /** Next \p Mutex into an owner-list, \p NULL if none.*/
  struct Mutex          *m_next;
} Mutex;

#ifdef __cplusplus
extern "C" {
#endif
  void chMtxInit(Mutex *mp);
  void chMtxLock(Mutex *mp);
  void chMtxLockS(Mutex *mp);
  bool_t chMtxTryLock(Mutex *mp);
  bool_t chMtxTryLockS(Mutex *mp);
  Mutex *chMtxUnlock(void);
  Mutex *chMtxUnlockS(void);
  void chMtxUnlockAll(void);
#ifdef __cplusplus
}
#endif

/**
 * Returns \p TRUE if the mutex queue contains at least a waiting thread.
 */
#define chMtxQueueNotEmptyS(mp) notempty(&(mp)->m_queue)

#endif /* CH_USE_MUTEXES */

#endif /* _MUTEXES_H_ */

/** @} */
