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

#ifndef _TEST_H_
#define _TEST_H_

#define MAX_THREADS             5
#define MAX_TOKENS              16
#define DELAY_BETWEEN_TESTS     2000

#if defined(CH_ARCHITECTURE_AVR) || defined(CH_ARCHITECTURE_MSP430)
#define THREADS_STACK_SIZE      64
#else
#define THREADS_STACK_SIZE      128
#endif
#define STKSIZE UserStackSize(THREADS_STACK_SIZE)

struct testcase {
  char *(*gettest)(void);
  void (*setup)(void);
  void (*teardown)(void);
  void (*execute)(void);
};

#ifdef __cplusplus
extern "C" {
#endif
  msg_t TestThread(void *p);
  void test_printn(uint32_t n);
  void test_print(char *msgp);
  void test_println(char *msgp);
  void test_emit_token(char token);
  void test_fail(char * msg);
  void test_assert(bool_t condition, char * msg);
  void test_assert_sequence(char *expected);
  void test_assert_time_window(systime_t start, systime_t end);
  void test_wait_threads(void);
  systime_t test_wait_tick(void);
  void test_cpu_pulse(systime_t ms);
  void test_start_timer(systime_t time);
#ifdef __cplusplus
}
#endif

extern Thread *threads[MAX_THREADS];
extern void *wa[MAX_THREADS];
extern bool_t test_timer_done;

#endif /* _TEST_H_ */
