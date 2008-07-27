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

#include <ch.h>

#include "test.h"

#define ALLOWED_DELAY 5

static Mutex m1;
static CondVar c1;

static char *cond1_gettest(void) {

  return "CondVar, signal test";
}

static void cond1_setup(void) {

  chCondInit(&c1);
  chMtxInit(&m1);
}

static void cond1_teardown(void) {
}

static msg_t thread1(void *p) {
  chMtxLock(&m1);
  chCondWait(&c1, &m1);
  test_emit_token(*(char *)p);
  chMtxUnlock();
  return 0;
}

static void cond1_execute(void) {

  // Bacause priority inheritance.
  tprio_t prio = chThdGetPriority();
  threads[0] = chThdCreate(prio+1, 0, wa[0], STKSIZE, thread1, "E");
  threads[1] = chThdCreate(prio+2, 0, wa[1], STKSIZE, thread1, "D");
  threads[2] = chThdCreate(prio+3, 0, wa[2], STKSIZE, thread1, "C");
  threads[3] = chThdCreate(prio+4, 0, wa[3], STKSIZE, thread1, "B");
  threads[4] = chThdCreate(prio+5, 0, wa[4], STKSIZE, thread1, "A");
  test_assert(prio == chThdGetPriority(), "priority return failure");
  chCondSignal(&c1);
  chCondSignal(&c1);
  chCondSignal(&c1);
  chCondSignal(&c1);
  chCondSignal(&c1);
  test_wait_threads();
  test_assert_sequence("ABCDE");
}

const struct testcase testcond1 = {
  cond1_gettest,
  cond1_setup,
  cond1_teardown,
  cond1_execute
};

static char *cond2_gettest(void) {

  return "CondVar, broadcast test";
}

static void cond2_execute(void) {

  // Bacause priority inheritance.
  tprio_t prio = chThdGetPriority();
  threads[0] = chThdCreate(prio+1, 0, wa[0], STKSIZE, thread1, "E");
  threads[1] = chThdCreate(prio+2, 0, wa[1], STKSIZE, thread1, "D");
  threads[2] = chThdCreate(prio+3, 0, wa[2], STKSIZE, thread1, "C");
  threads[3] = chThdCreate(prio+4, 0, wa[3], STKSIZE, thread1, "B");
  threads[4] = chThdCreate(prio+5, 0, wa[4], STKSIZE, thread1, "A");
  test_assert(prio == chThdGetPriority(), "priority return failure");
  chCondBroadcast(&c1);
  test_wait_threads();
  test_assert_sequence("ABCDE");
}

const struct testcase testcond2 = {
  cond2_gettest,
  cond1_setup,
  cond1_teardown,
  cond2_execute
};

