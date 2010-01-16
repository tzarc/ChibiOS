/*
    ChibiOS/RT - Copyright (C) 2010 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @file mailboxes.h
 * @brief Mailboxes macros and structures.
 * @addtogroup mailboxes
 * @{
 */

#ifndef _MAILBOXES_H_
#define _MAILBOXES_H_

#if CH_USE_MAILBOXES

/*
 * Module dependencies check.
 */
#if !CH_USE_SEMAPHORES
#error "CH_USE_MAILBOXES requires CH_USE_SEMAPHORES"
#endif

typedef struct {
  msg_t                 *mb_buffer;     /**< Pointer to the mailbox buffer.*/
  msg_t                 *mb_top;        /**< Pointer to the first location
                                             after the buffer.*/
  msg_t                 *mb_wrptr;      /**< Write pointer.*/
  msg_t                 *mb_rdptr;      /**< Read pointer.*/
  Semaphore             mb_fullsem;     /**< Full counter @p Semaphore.*/
  Semaphore             mb_emptysem;    /**< Empty counter @p Semaphore.*/
} Mailbox;

#ifdef __cplusplus
extern "C" {
#endif
  void chMBInit(Mailbox *mbp, msg_t *buf, cnt_t n);
  void chMBReset(Mailbox *mbp);
  msg_t chMBPost(Mailbox *mbp, msg_t msg, systime_t timeout);
  msg_t chMBPostS(Mailbox *mbp, msg_t msg, systime_t timeout);
  msg_t chMBPostAhead(Mailbox *mbp, msg_t msg, systime_t timeout);
  msg_t chMBPostAheadS(Mailbox *mbp, msg_t msg, systime_t timeout);
  msg_t chMBFetch(Mailbox *mbp, msg_t *msgp, systime_t timeout);
  msg_t chMBFetchS(Mailbox *mbp, msg_t *msgp, systime_t timeout);
#ifdef __cplusplus
}
#endif

/**
 * Returns the mailbox buffer size.
 * @param[in] mbp the pointer to an initialized Mailbox object
 */
#define chMBSize(mbp)                                                   \
        ((mbp)->mb_top - (mbp)->mb_buffer)

/**
 * Returns the free space into the mailbox.
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @return The number of empty message slots.
 * @note Can be invoked in any system state but if invoked out of a locked
 *       state then the returned value may change after reading.
 * @note The returned value can be less than zero when there are waiting
 *       threads on the internal semaphore.
 */
#define chMBGetEmpty(mbp) chSemGetCounterI(&(mbp)->mb_emptysem)

/**
 * Returns the number of messages into the mailbox.
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @return The number of queued messages.
 * @note Can be invoked in any system state but if invoked out of a locked
 *       state then the returned value may change after reading.
 * @note The returned value can be less than zero when there are waiting
 *       threads on the internal semaphore.
 */
#define chMBGetFull(mbp) chSemGetCounterI(&(mbp)->mb_fullsem)

/**
 * Returns the next message in the queue without removing it.
 * @note A message must be waiting in the queue for this function to work or
 *       it would return garbage. The correct way to use this macro is to
 *       use @p chMBGetFull() and then use this macro, all within a lock state.
 */
#define chMBPeek(mbp) (*(mbp)->mb_rdptr)

/**
 * @brief Data part of a static mailbox initializer.
 * @details This macro should be used when statically initializing a
 *          mailbox that is part of a bigger structure.
 * @param name the name of the mailbox variable
 * @param buffer pointer to the mailbox buffer area
 * @param size size of the mailbox buffer area
 */
#define _MAILBOX_DATA(name, buffer, size) {                             \
  (msg_t *)(buffer),                                                    \
  (msg_t *)(buffer) + size,                                             \
  (msg_t *)(buffer),                                                    \
  (msg_t *)(buffer),                                                    \
  _SEMAPHORE_DATA(name.mb_fullsem, 0),                                  \
  _SEMAPHORE_DATA(name.mb_emptysem, size),                              \
}

/**
 * @brief Static mailbox initializer.
 * @details Statically initialized mailboxes require no explicit
 *          initialization using @p chMBInit().
 * @param name the name of the mailbox variable
 * @param buffer pointer to the mailbox buffer area
 * @param size size of the mailbox buffer area
 */
#define MAILBOX_DECL(name, buffer, size)                                \
  Mailbox name = _MAILBOX_DATA(name, buffer, size)

#endif /* CH_USE_MAILBOXES */

#endif /* _MAILBOXES_H_ */

/** @} */
