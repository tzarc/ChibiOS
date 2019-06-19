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
 * @file    chobjects.h
 * @brief   Operating System Objects macros and structures.
 *
 * @addtogroup os_structures
 * @{
 */

#ifndef CHOBJECTS_H
#define CHOBJECTS_H

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

/**
 * @brief   Type of a Virtual Timer callback function.
 */
typedef void (*vtfunc_t)(void *p);

/**
 * @extends virtual_timers_list_t
 *
 * @brief   Type of a Virtual Timer structure.
 */
typedef struct ch_virtual_timer virtual_timer_t;

/**
 * @brief   Virtual Timer descriptor structure.
 */
struct ch_virtual_timer {
  virtual_timer_t       *next;      /**< @brief Next timer in the list.     */
  virtual_timer_t       *prev;      /**< @brief Previous timer in the list. */
  sysinterval_t         delta;      /**< @brief Time delta before timeout.  */
  vtfunc_t              func;       /**< @brief Timer callback function
                                                pointer.                    */
  void                  *par;       /**< @brief Timer callback function
                                                parameter.                  */
};

/**
 * @brief   Type of virtual timers list header.
 * @note    The timers list is implemented as a double link bidirectional list
 *          in order to make the unlink time constant, the reset of a virtual
 *          timer is often used in the code.
 */
typedef struct ch_virtual_timers_list {
  virtual_timer_t       *next;      /**< @brief Next timer in the delta
                                                list.                       */
  virtual_timer_t       *prev;      /**< @brief Last timer in the delta
                                                list.                       */
  sysinterval_t         delta;      /**< @brief Must be initialized to -1.  */
#if (CH_CFG_ST_TIMEDELTA == 0) || defined(__DOXYGEN__)
  volatile systime_t    systime;    /**< @brief System Time counter.        */
#endif
#if (CH_CFG_ST_TIMEDELTA > 0) || defined(__DOXYGEN__)
  /**
   * @brief   System time of the last tick event.
   */
  systime_t             lasttime;   /**< @brief System time of the last
                                                tick event.                 */
#endif
} virtual_timers_list_t;

/**
 * @brief   Type of a thread reference.
 */
typedef thread_t * thread_reference_t;

/**
 * @brief   Type of a generic threads single link list, it works like a stack.
 */
typedef struct ch_threads_list {
  thread_t              *next;      /**< @brief Next in the list/queue.     */
} threads_list_t;

/**
 * @extends threads_list_t
 *
 * @brief   Type of a generic threads bidirectional linked list header and
 *          element.
 */
typedef struct ch_threads_queue {
  thread_t              *next;      /**< @brief Next in the list/queue.     */
  thread_t              *prev;      /**< @brief Previous in the queue.      */
} threads_queue_t;

/**
 * @brief   Structure representing a thread.
 * @note    Not all the listed fields are always needed, by switching off some
 *          not needed ChibiOS/RT subsystems it is possible to save RAM space
 *          by shrinking this structure.
 */
struct ch_thread {
  threads_queue_t       queue;      /**< @brief Threads queue header.       */
  tprio_t               prio;       /**< @brief Thread priority.            */
  struct port_context   ctx;        /**< @brief Processor context.          */
#if (CH_CFG_USE_REGISTRY == TRUE) || defined(__DOXYGEN__)
  thread_t              *newer;     /**< @brief Newer registry element.     */
  thread_t              *older;     /**< @brief Older registry element.     */
#endif
  /* End of the fields shared with the ReadyList structure. */
#if (CH_CFG_LOOSE_INSTANCES == FALSE) || defined(__DOXYGEN__)
  /**
   * @brief   OS instance owner of this thread.
   */
  ch_instance_t         *owner;
#endif
#if (CH_CFG_USE_REGISTRY == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Thread name or @p NULL.
   */
  const char            *name;
#endif
#if (CH_DBG_ENABLE_STACK_CHECK == TRUE) || (CH_CFG_USE_DYNAMIC == TRUE) ||  \
    defined(__DOXYGEN__)
  /**
   * @brief   Working area base address.
   * @note    This pointer is used for stack overflow checks and for
   *          dynamic threading.
   */
  stkalign_t            *wabase;
#endif
  /**
   * @brief   Current thread state.
   */
  tstate_t              state;
  /**
   * @brief   Various thread flags.
   */
  tmode_t               flags;
#if (CH_CFG_USE_REGISTRY == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   References to this thread.
   */
  trefs_t               refs;
#endif
  /**
   * @brief   Number of ticks remaining to this thread.
   */
#if (CH_CFG_TIME_QUANTUM > 0) || defined(__DOXYGEN__)
  tslices_t             ticks;
#endif
#if (CH_DBG_THREADS_PROFILING == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Thread consumed time in ticks.
   * @note    This field can overflow.
   */
  volatile systime_t    time;
#endif
  /**
   * @brief   State-specific fields.
   * @note    All the fields declared in this union are only valid in the
   *          specified state or condition and are thus volatile.
   */
  union {
    /**
     * @brief   Thread wakeup code.
     * @note    This field contains the low level message sent to the thread
     *          by the waking thread or interrupt handler. The value is valid
     *          after exiting the @p chSchWakeupS() function.
     */
    msg_t               rdymsg;
    /**
     * @brief   Thread exit code.
     * @note    The thread termination code is stored in this field in order
     *          to be retrieved by the thread performing a @p chThdWait() on
     *          this thread.
     */
    msg_t               exitcode;
    /**
     * @brief   Pointer to a generic "wait" object.
     * @note    This field is used to get a generic pointer to a synchronization
     *          object and is valid when the thread is in one of the wait
     *          states.
     */
    void                *wtobjp;
    /**
     * @brief   Pointer to a generic thread reference object.
     * @note    This field is used to get a pointer to a synchronization
     *          object and is valid when the thread is in @p CH_STATE_SUSPENDED
     *          state.
     */
    thread_reference_t  *wttrp;
#if (CH_CFG_USE_MESSAGES == TRUE) || defined(__DOXYGEN__)
    /**
     * @brief   Thread sent message.
     */
    msg_t               sentmsg;
#endif
#if (CH_CFG_USE_SEMAPHORES == TRUE) || defined(__DOXYGEN__)
    /**
     * @brief   Pointer to a generic semaphore object.
     * @note    This field is used to get a pointer to a synchronization
     *          object and is valid when the thread is in @p CH_STATE_WTSEM
     *          state.
     */
    struct ch_semaphore *wtsemp;
#endif
#if (CH_CFG_USE_MUTEXES == TRUE) || defined(__DOXYGEN__)
    /**
     * @brief   Pointer to a generic mutex object.
     * @note    This field is used to get a pointer to a synchronization
     *          object and is valid when the thread is in @p CH_STATE_WTMTX
     *          state.
     */
    struct ch_mutex     *wtmtxp;
#endif
#if (CH_CFG_USE_EVENTS == TRUE) || defined(__DOXYGEN__)
    /**
     * @brief   Enabled events mask.
     * @note    This field is only valid while the thread is in the
     *          @p CH_STATE_WTOREVT or @p CH_STATE_WTANDEVT states.
     */
    eventmask_t         ewmask;
#endif
  }                     u;
#if (CH_CFG_USE_WAITEXIT == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Termination waiting list.
   */
  threads_list_t        waiting;
#endif
#if (CH_CFG_USE_MESSAGES == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Messages queue.
   */
  threads_queue_t       msgqueue;
#endif
#if (CH_CFG_USE_EVENTS == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Pending events mask.
   */
  eventmask_t           epending;
#endif
#if (CH_CFG_USE_MUTEXES == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   List of the mutexes owned by this thread.
   * @note    The list is terminated by a @p NULL in this field.
   */
  struct ch_mutex       *mtxlist;
  /**
   * @brief   Thread's own, non-inherited, priority.
   */
  tprio_t               realprio;
#endif
#if ((CH_CFG_USE_DYNAMIC == TRUE) && (CH_CFG_USE_MEMPOOLS == TRUE)) ||      \
    defined(__DOXYGEN__)
  /**
   * @brief   Memory Pool where the thread workspace is returned.
   */
  void                  *mpool;
#endif
#if (CH_DBG_STATISTICS == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Thread statistics.
   */
  time_measurement_t    stats;
#endif
#if defined(CH_CFG_THREAD_EXTRA_FIELDS)
  /* Extra fields defined in chconf.h.*/
  CH_CFG_THREAD_EXTRA_FIELDS
#endif
};

/**
 * @extends threads_queue_t
 *
 * @brief   Type of a ready list header.
 */
typedef struct ch_ready_list {
  threads_queue_t       queue;      /**< @brief Threads queue.              */
  tprio_t               prio;       /**< @brief This field must be
                                                initialized to zero.        */
  struct port_context   ctx;        /**< @brief Not used, present because
                                                offsets.                    */
#if (CH_CFG_USE_REGISTRY == TRUE) || defined(__DOXYGEN__)
  thread_t              *newer;     /**< @brief Newer registry element.     */
  thread_t              *older;     /**< @brief Older registry element.     */
#endif
  /* End of the fields shared with the thread_t structure.*/
  thread_t              *current;   /**< @brief The currently running
                                                thread.                     */
} ready_list_t;

/**
 * @brief   Type of an system instance configuration.
 */
typedef struct ch_instance_config {
  /**
   * @brief   Instance name.
   */
  const char            *name;
#if (CH_DBG_ENABLE_STACK_CHECK == TRUE) || (CH_CFG_USE_DYNAMIC == TRUE) ||  \
    defined(__DOXYGEN__)
  /**
   * @brief   Lower limit of the main function thread stack.
   */
  stkalign_t            *mainthread_base;
  /**
   * @brief   Upper limit of the main function thread stack.
   */
  stkalign_t            *mainthread_end;
#endif
#if (CH_CFG_NO_IDLE_THREAD == FALSE) || defined(__DOXYGEN__)
  /**
   * @brief   Lower limit of the dedicated idle thread stack.
   */
  stkalign_t            *idlethread_base;
  /**
   * @brief   Upper limit of the dedicated idle thread stack.
   */
  stkalign_t            *idlethread_end;
#endif
} ch_instance_config_t;

/**
 * @brief   System instance data structure.
 */
struct ch_instance {
  /**
   * @brief   Ready list header.
   */
  ready_list_t          rlist;
  /**
   * @brief   Virtual timers delta list header.
   */
  virtual_timers_list_t vtlist;
  /**
   * @brief   Main thread descriptor.
   */
  thread_t              mainthread;
  /**
   * @brief   System debug.
   */
  system_debug_t        dbg;
#if (CH_DBG_TRACE_MASK != CH_DBG_TRACE_MASK_DISABLED) || defined(__DOXYGEN__)
  /**
   * @brief   Trace buffer.
   */
  trace_buffer_t        trace_buffer;
#endif
#if (CH_CFG_USE_TM == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Time measurement calibration data.
   */
  tm_calibration_t      tmc;
#endif
#if (CH_DBG_STATISTICS == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Global kernel statistics.
   */
  kernel_stats_t        kernel_stats;
#endif
#if defined(PORT_INSTANCE_EXTRA_FIELDS) || defined(__DOXYGEN__)
  /* Extra fields from port layer.*/
  PORT_INSTANCE_EXTRA_FIELDS
#endif
  /* Extra fields from configuration.*/
  CH_CFG_INSTANCE_EXTRA_FIELDS
};

/**
 * @brief   Type of system data structure.
 */
typedef struct ch_system {
#if defined(PORT_SYSTEM_EXTRA_FIELDS) || defined(__DOXYGEN__)
  /* Extra fields from port layer.*/
  PORT_SYSTEM_EXTRA_FIELDS
#endif
  /* Extra fields from configuration.*/
  CH_CFG_SYSTEM_EXTRA_FIELDS
  /**
   * @brief   OS instances.
   */
  ch_instance_t         instance[CH_CFG_INSTANCES_NUMBER];
} ch_system_t;

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

#endif /* CHOBJECTS_H */

/** @} */
