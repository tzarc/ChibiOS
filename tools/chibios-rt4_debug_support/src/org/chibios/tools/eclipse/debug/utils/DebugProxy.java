/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

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

package org.chibios.tools.eclipse.debug.utils;

import java.util.HashMap;
import java.util.LinkedHashMap;

import org.eclipse.cdt.debug.core.cdi.model.ICDITarget;
import org.eclipse.cdt.debug.internal.core.model.CDebugTarget;
import org.eclipse.cdt.debug.mi.core.MIException;
import org.eclipse.cdt.debug.mi.core.MIFormat;
import org.eclipse.cdt.debug.mi.core.MISession;
import org.eclipse.cdt.debug.mi.core.cdi.model.Target;
import org.eclipse.cdt.debug.mi.core.command.CommandFactory;
import org.eclipse.cdt.debug.mi.core.command.MIDataEvaluateExpression;
import org.eclipse.cdt.debug.mi.core.command.MIDataReadMemory;
import org.eclipse.cdt.debug.mi.core.output.MIDataEvaluateExpressionInfo;
import org.eclipse.cdt.debug.mi.core.output.MIDataReadMemoryInfo;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.model.IDebugTarget;

@SuppressWarnings("restriction")
public class DebugProxy {

  private CommandFactory cmd_factory;
  private MISession mi_session;

  protected final static String[] threadStates = {
    "READY",
    "CURRENT",
    "STARTED",
    "SUSPENDED",
    "QUEUED",
    "WTSEM",
    "WTMTX",
    "WTCOND",
    "SLEEPING",
    "WTEXIT",
    "WTOREVT",
    "WTANDEVT",
    "SNDMSGQ",
    "SNDMSG",
    "WTMSG",
    "FINAL",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown"
  };

  private void getSession(CDebugTarget target)
      throws DebugProxyException {
    ICDITarget[] targets = target.getCDISession().getTargets();
    ICDITarget cdi_target = null;
    for (int i = 0; i < targets.length; i++) {
      if (targets[i] instanceof Target) {
        cdi_target = targets[i];
        break;
      }
    }
    if (cdi_target == null)
      throw new DebugProxyException("no CDI session found");
    mi_session = ((Target)cdi_target).getMISession();
    cmd_factory = mi_session.getCommandFactory();
  }

  public DebugProxy()
      throws DebugProxyException {
    IDebugTarget[] targets = DebugPlugin.getDefault().getLaunchManager().getDebugTargets();
    for (IDebugTarget target:targets) {
      if(target instanceof CDebugTarget) {
        getSession((CDebugTarget)target);
        return;
      }
    }
  }

  public DebugProxy(CDebugTarget target)
      throws DebugProxyException {
    getSession(target);
  }

  public String evaluateExpression(String expression)
      throws DebugProxyException {
    if (mi_session.getMIInferior().isRunning())
      return null;
    MIDataEvaluateExpression expr = cmd_factory.createMIDataEvaluateExpression(expression);
    try {
      mi_session.postCommand(expr);
      MIDataEvaluateExpressionInfo info = expr.getMIDataEvaluateExpressionInfo();
      if (info != null)
        return info.getExpression();
    } catch (MIException e) {}
    throw new DebugProxyException("error evaluating the expression: '" +
                                  expression + "'");
  }

  public long scanStack(long base, long end, long pattern)
      throws DebugProxyException {
    if (mi_session.getMIInferior().isRunning())
      return -1;
    if (end > base) {
      MIDataReadMemory mem = cmd_factory.createMIDataReadMemory(0,
          Long.toString(base),
          MIFormat.HEXADECIMAL,
          4,
          1,
          (int)(end - base),
          '.');
      try {
        mi_session.postCommand(mem);
         MIDataReadMemoryInfo info = mem.getMIDataReadMemoryInfo();
         if (info != null) {
           long[] data = info.getMemories()[0].getData();
           int i = 0;
           while ((i < data.length) && (data[i] == pattern))
               i++;
           return i * 4;
         }
      } catch (MIException e) {}
      throw new DebugProxyException("error reading memory at " +
          base);
    }
    return 0;
  }

  public String readCString(long address, int max)
      throws DebugProxyException {
    if (mi_session.getMIInferior().isRunning())
      return null;
    MIDataReadMemory mem = cmd_factory.createMIDataReadMemory(0,
                                                              Long.toString(address),
                                                              MIFormat.HEXADECIMAL,
                                                              1,
                                                              1,
                                                              max,
                                                              '.');
    try {
      mi_session.postCommand(mem);
       MIDataReadMemoryInfo info = mem.getMIDataReadMemoryInfo();
       if (info != null) {
          String s = info.getMemories()[0].getAscii();
          int i = s.indexOf('.');
          if (i >= 0)
            return s.substring(0, s.indexOf('.'));
          else
            return s;
       }
    } catch (MIException e) {}
    throw new DebugProxyException("error reading memory at " +
        address);
  }

  private boolean checkChibiOS() throws DebugProxyException {

    // Check on current thread in order to decide if the kernel has already
    // been initialized.
    String r_current;
    try {
      r_current = evaluateExpression("(uint32_t)ch.rlist.current");
      if (r_current == null)
        return true;
    } catch (DebugProxyException e) {
      throw new DebugProxyException("ChibiOS/RT not found on target");
    } catch (Exception e) {
      return true;
    }
    if (r_current.compareTo("0") == 0)
      throw new DebugProxyException("ChibiOS/RT not yet initialized");

    return false;
  }

  /**
   * @brief   Return the list of threads.
   * @details The threads list is fetched from memory by scanning the
   *          registry.
   *
   * @return  A @p LinkedHashMap object whose keys are the threads addresses
   *          as decimal strings, the value is an @p HashMap of the thread
   *          fields:
   *          - stack
   *          - stklimit
   *          - name
   *          - state
   *          - state_s
   *          - flags
   *          - prio
   *          - refs
   *          - time
   *          - wtobjp
   *          - stats_n
   *          - stats_worst
   *          - stats_cumulative
   *          .
   *          Missing fields are set to "-".
   * @retval null                   If the debugger encountered an error or
   *                                the target is running.
   *
   * @throws DebugProxyException    If the debugger is active but the registry
   *                                is not found, not initialized or corrupted.
   */
  public LinkedHashMap<String, HashMap<String, String>> readThreads()
      throws DebugProxyException {

	if (checkChibiOS())
      return null;

    // rlist structure address.
    String rlist;
    try {
      rlist = evaluateExpression("(uint32_t)&ch.rlist");
      if (rlist == null)
        return null;
    } catch (DebugProxyException e) {
      throw new DebugProxyException("ready list not found on target");
    } catch (Exception e) {
      return null;
    }

    // Scanning registry.
    LinkedHashMap<String, HashMap<String, String>> lhm =
        new LinkedHashMap<String, HashMap<String, String>>(10);
    String current = rlist;
    String previous = rlist;
    while (true) {
      
      // Fetching next thread in the registry (newer link). This fetch fails
      // if the register is not enabled in the kernel and the "newer" field
      // does not exist.
      try {
        current = evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->newer");
      } catch (DebugProxyException e1) {
        throw new DebugProxyException("ChibiOS/RT registry not enabled in kernel");
      }

      // This can happen if the kernel is not initialized yet or if the
      // registry is corrupted.
      if (current.compareTo("0") == 0)
        throw new DebugProxyException("ChibiOS/RT registry integrity check failed, NULL pointer");

      // TODO: integrity check on the pointer value (alignment, range).

      // The previous thread in the list is fetched as a integrity check.
      String older = evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->older");
      if (older.compareTo("0") == 0)
        throw new DebugProxyException("ChibiOS/RT registry integrity check failed, NULL pointer");
      if (previous.compareTo(older) != 0)
        throw new DebugProxyException("ChibiOS/RT registry integrity check failed, double linked list violation");

      // End of the linked list condition.
      if (current.compareTo(rlist) == 0)
        break;

      // Hash of threads fields.
      HashMap<String, String> map = new HashMap<String, String>(16);

      // Fetch of the various fields in the thread_t structure. Some fields
      // are optional so are placed within try-catch.
      long stklimit;
      try {
        stklimit = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->wabase"));
        map.put("stklimit", Long.toString(stklimit));
      } catch (DebugProxyException e) {
        map.put("stklimit", "-");
        stklimit = -1;
      }

      long stack;
      try {
        stack = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->ctx.r13"));
        map.put("stack", Long.toString(stack));
      } catch (DebugProxyException e) {
        try {
          stack = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->ctx.sp"));
          map.put("stack", Long.toString(stack));
        } catch (DebugProxyException ex) {
          map.put("stack", "-");
          stack = -1;
        }
      }

      if ((stklimit <= 0) || (stack <= 0))
          map.put("stkunused", "-");
      else {
        if ((stack < 0) || (stack < stklimit))
            map.put("stkunused", "overflow");
        else {
          long stkunused = scanStack(stklimit, stack, 0x55555555);
          map.put("stkunused", Long.toString(stkunused));
        }
      }

      long n;
      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->name"));
        if (n == 0)
          map.put("name", "<no name>");
        else
          map.put("name", readCString(n, 16));
      } catch (DebugProxyException e) {
        map.put("name", "-");
      }

      n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->state"));
      map.put("state", Long.toString(n));
      if ((n >= 0) && (n < threadStates.length)) {
        map.put("state_s", threadStates[(int)n]);
      }
      else
        map.put("state_s", "unknown");

      n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->flags"));
      map.put("flags", Long.toString(n));

      n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->prio"));
      map.put("prio", Long.toString(n));

      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->refs"));
        map.put("refs", Long.toString(n));
      } catch (DebugProxyException e) {
        map.put("refs", "-");
      }

      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->time"));
        map.put("time", Long.toString(n));
      } catch (DebugProxyException e) {
        map.put("time", "-");
      }

      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->u.wtobjp"));
        map.put("wtobjp", Long.toString(n));
      } catch (DebugProxyException e) {
        map.put("wtobjp", "-");
      }

      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->stats.n"));
        map.put("stats_n", Long.toString(n));
      } catch (DebugProxyException e) {
        map.put("stats_n", "-");
      }

      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + current + ")->stats.worst"));
        map.put("stats_worst", Long.toString(n));
      } catch (DebugProxyException e) {
        map.put("stats_worst", "-");
      }

      try {
        n = HexUtils.parseNumber(evaluateExpression("(uint64_t)((struct ch_thread *)" + current + ")->stats.cumulative"));
        map.put("stats_cumulative", Long.toString(n));
      } catch (DebugProxyException e) {
        map.put("stats_cumulative", "-");
      }

      // Inserting the new thread map into the threads list.
      lhm.put(current, map);

      previous = current;
    }
    return lhm;
  }

  /**
   * @brief   Return the list of timers.
   * @details The timers list is fetched from memory by scanning the
   *          @p vtlist structure.
   *
   * @return  A @p LinkedHashMap object whose keys are the timers addresses
   *          as decimal strings, the value is an @p HashMap of the timers
   *          fields:
   *          - delta
   *          - func
   *          - par
   *          .
   * @retval null                   If the debugger encountered an error or
   *                                the target is running.
   *
   * @throws DebugProxyException    If the debugger is active but the structure
   *                                @p vtlist is not found, not initialized or
   *                                corrupted.
   */
  public LinkedHashMap<String, HashMap<String, String>> readTimers()
      throws DebugProxyException {

    if (checkChibiOS())
      return null;

    // Delta list structure address.
    String vtlist;
    try {
      vtlist = evaluateExpression("(uint32_t)&ch.vtlist");
      if (vtlist == null)
        return null;
    } catch (DebugProxyException e) {
      throw new DebugProxyException("virtual timers list not found on target");
    } catch (Exception e) {
      return null;
    }

    // Scanning delta list.
    LinkedHashMap<String, HashMap<String, String>> lhm =
        new LinkedHashMap<String, HashMap<String, String>>(10);
    String current = vtlist;
    String previous = vtlist;
    while (true) {
      
      // Fetching next timer in the delta list (next link).
      current = evaluateExpression("(uint32_t)((struct ch_virtual_timer *)" + current + ")->next");

      // This can happen if the kernel is not initialized yet or if the
      // delta list is corrupted.
      if (current.compareTo("0") == 0)
        throw new DebugProxyException("ChibiOS/RT delta list integrity check failed, NULL pointer");

      // TODO: integrity check on the pointer value (alignment, range).

      // The previous timer in the delta list is fetched as a integrity check.
      String prev = evaluateExpression("(uint32_t)((struct ch_virtual_timer *)" + current + ")->prev");
      if (prev.compareTo("0") == 0)
        throw new DebugProxyException("ChibiOS/RT delta list integrity check failed, NULL pointer");
      if (previous.compareTo(prev) != 0)
        throw new DebugProxyException("ChibiOS/RT delta list integrity check failed, double linked list violation");

      // End of the linked list condition.
      if (current.compareTo(vtlist) == 0)
        break;

      // Hash of timers fields.
      HashMap<String, String> map = new HashMap<String, String>(16);

      // Fetch of the various fields in the virtual_timer_t structure. Some fields
      // are optional so are placed within try-catch.
      long n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_virtual_timer *)" + current + ")->delta"));
      map.put("delta", Long.toString(n));

      n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_virtual_timer *)" + current + ")->func"));
      map.put("func", Long.toString(n));

      n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_virtual_timer *)" + current + ")->par"));
      map.put("par", Long.toString(n));

      // Inserting the new thread map into the threads list.
      lhm.put(current, map);

      previous = current;
    }
    return lhm;
  }

  /**
   * @brief   Return the list of trace buffer entries.
   * @details The trace buffer is fetched from memory by scanning the
   *          @p dbg_trace_buffer array.
   *
   * @return  A @p LinkedHashMap object whose keys are the timers addresses
   *          as decimal strings, the value is an @p HashMap of the timers
   *          fields:
   *          - time
   *          - ntp
   *          - wtobjp
   *          - state
   *          - state_s
   *          .
   * @retval null                   If the debugger encountered an error or
   *                                the target is running.
   *
   * @throws DebugProxyException    If the debugger is active but the structure
   *                                @p dbg_trace_buffer is not found, not
   *                                initialized or corrupted.
   */
  public LinkedHashMap<String, HashMap<String, String>> readTraceBuffer()
      throws DebugProxyException {

    if (checkChibiOS())
      return null;

    // Trace buffer size.
    String s;
    try {
      s = evaluateExpression("(uint32_t)ch.dbg.trace_buffer.size");
      if (s == null)
        return null;
    } catch (DebugProxyException e) {
      throw new DebugProxyException("trace buffer not found on target");
    } catch (Exception e) {
      return null;
    }

    int tbsize = (int)HexUtils.parseNumber(s);
    int tbrecsize = (int)HexUtils.parseNumber(evaluateExpression("(uint32_t)sizeof (ch_trace_event_t)"));
    long tbstart = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.dbg.trace_buffer.buffer"));
    long tbend = HexUtils.parseNumber(evaluateExpression("(uint32_t)&ch.dbg.trace_buffer.buffer[" + tbsize + "]"));
    long tbptr = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.dbg.trace_buffer.ptr"));

    // Scanning the trace buffer from the oldest event to the newest.
    LinkedHashMap<String, HashMap<String, String>> lhm =
        new LinkedHashMap<String, HashMap<String, String>>(64);
    int n = tbsize;
    int i = -tbsize + 1;
    while (n > 0) {
      // Hash of timers fields.
      HashMap<String, String> map = new HashMap<String, String>(16);

      // This is the record type, fields change according to this.
      String type = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->type)");
      map.put("type", type);

      // Fields common to all types.
      long state = HexUtils.parseNumber(evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->state)"));
      map.put("state", Long.toString(state));
      if ((state >= 0) && (state < threadStates.length))
        map.put("state_s", threadStates[(int)state]);
      else
        map.put("state_s", "unknown");

      String rtstamp = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->rtstamp)");
      map.put("rtstamp", rtstamp);

      String time = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->time)");
      map.put("time", time);

      // Fields specific to a CH_TRACE_TYPE_SWITCH event.
      if (type.compareTo("1") == 0) {
        String ntp = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->u.sw.ntp)");
        map.put("sw_ntp", ntp);

        String wtobjp = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->u.sw.wtobjp)");
        map.put("sw_wtobjp", wtobjp);
      }

      // Fields specific to a CH_TRACE_TYPE_ISR_ENTER and CH_TRACE_TYPE_ISR_LEAVE events.
      if ((type.compareTo("2") == 0) || (type.compareTo("3") == 0)) {
        long name = HexUtils.parseNumber(evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->u.isr.name)"));
        String name_s = readCString(name, 16);
        map.put("isr_name_s", name_s);
      }

      // Fields specific to a CH_TRACE_TYPE_HALT event.
      if (type.compareTo("4") == 0) {
        long reason = HexUtils.parseNumber(evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->u.halt.reason)"));
        String reason_s = readCString(reason, 16);
        map.put("halt_reason_s", reason_s);
      }

      // Fields specific to a CH_TRACE_TYPE_USER event.
      if (type.compareTo("5") == 0) {
        String up1 = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->u.user.up1)");
        map.put("user_up1", up1);

        String up2 = evaluateExpression("(uint32_t)(((ch_trace_event_t *)" + tbptr + ")->u.user.up2)");
        map.put("user_up2", up2);
      }

      // Inserting the new event map into the events list if it is not CH_TRACE_TYPE_UNUSED.
      if (type.compareTo("0") != 0)
        lhm.put(Integer.toString(i), map);

      tbptr += tbrecsize;
      if (tbptr >= tbend)
        tbptr = tbstart;
      n--;
      i++;
    }
    return lhm;
  }
  
  /**
   * @brief   Return the list of the system global variables.
   *
   * @return  A @p LinkedHashMap object whose keys are the variable names and
   *          the values are the variable values.
   *
   * @retval null                   If the debugger encountered an error or
   *                                the target is running.
   *
   * @throws DebugProxyException    If the debugger is active but the structure
   *                                @p dbg_trace_buffer is not found, not
   *                                initialized or corrupted.
   */
  public LinkedHashMap<String, String> readGlobalVariables()
      throws DebugProxyException {

    if (checkChibiOS())
      return null;

    LinkedHashMap<String, String> map = new LinkedHashMap<String, String>(16);

    try {
        String vt_lasttime = evaluateExpression("(uint32_t)ch.vtlist.delta");
        if (vt_lasttime == null)
          return null;
        map.put("vt_lasttime", vt_lasttime);
    } catch (DebugProxyException e1) {
      throw new DebugProxyException("virtual timers list not found on target");
    } catch (Exception e) {
      return null;
    }

    try {
      String vt_systime = evaluateExpression("(uint32_t)ch.vtlist.systime");
      if (vt_systime == null)
        return null;
      map.put("vt_systime", vt_systime);
    } catch (DebugProxyException e) {}

    try {
        String vt_lasttime = evaluateExpression("(uint32_t)ch.vtlist.lasttime");
        if (vt_lasttime == null)
          return null;
        map.put("vt_lasttime", vt_lasttime);
    } catch (DebugProxyException e) {}

    try {
      long r_current = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.rlist.current"));
      if (r_current != 0) {
        String name;
        try {
          long n = HexUtils.parseNumber(evaluateExpression("(uint32_t)((struct ch_thread *)" + r_current + ")->name"));
          if (n == 0)
            name = "<no name>";
          else
            name = readCString(n, 16);
        } catch (DebugProxyException e) {
          name = "-";
        }
        map.put("r_current", HexUtils.dword2HexString((int)r_current) + " \"" + name + "\"");
      }
      else
        map.put("r_current", "0");
    } catch (DebugProxyException e) {}

    try {
      String r_preempt = evaluateExpression("(uint32_t)ch.rlist.preempt");
      map.put("r_preempt", r_preempt);
    } catch (DebugProxyException e) {}

    try {
      Long addr = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.dbg.panic_msg"));
      if (addr == 0)
        map.put("dbg_panic_msg", "<NULL>");
      else
        map.put("dbg_panic_msg", readCString(addr, 32));
    } catch (DebugProxyException e) {
      map.put("dbg_panic_msg", "<not enabled>");
    }

    try {
      Long isr_cnt = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.dbg.isr_cnt"));
      if (isr_cnt == 0)
        map.put("dbg_isr_cnt", "not within ISR");
      else
        map.put("dbg_isr_cnt", "within ISR");
    } catch (DebugProxyException e) {
      map.put("dbg_isr_cnt", "<not enabled>");
    }

    try {
      Long lock_cnt = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.dbg.lock_cnt"));
      if (lock_cnt == 0)
        map.put("dbg_lock_cnt", "not within lock");
      else
        map.put("dbg_lock_cnt", "within lock");
    } catch (DebugProxyException e) {
      map.put("dbg_lock_cnt", "<not enabled>");
    }

    return map;
  }

  /**
   * @brief   Return the list of statistics variables.
   * @details The statistics info are fetched from memory by accessing the
   *          kernel_stats structure.
   *
   * @return  A @p LinkedHashMap object whose keys are the variable names,
   *          the value is an @p HashMap statistics counters:
   *          - best
   *          - worst
   *          - cumulative
   *          .
   * @retval null                   If the debugger encountered an error or
   *                                the target is running.
   *
   * @throws DebugProxyException    If the debugger is active but the structure
   *                                @p dbg_trace_buffer is not found, not
   *                                initialized or corrupted.
   */
  public LinkedHashMap<String, HashMap<String, String>> readStatistics()
      throws DebugProxyException {

    if (checkChibiOS())
      return null;

    // Statistics structure.
    String s;
    try {
      s = evaluateExpression("(uint32_t)&ch.kernel_stats");
      if (s == null)
        return null;
    } catch (DebugProxyException e) {
      throw new DebugProxyException("statistics info structure not found on target");
    } catch (Exception e) {
      return null;
    }

    LinkedHashMap<String, HashMap<String, String>> lhm =
            new LinkedHashMap<String, HashMap<String, String>>(64);

    try {
        long n_irq = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.n_irq"));
        HashMap<String, String> map = new HashMap<String, String>(4);
        map.put("best", "");
        map.put("worst", "");
        map.put("n", Long.toString(n_irq));
        map.put("cumulative", "");
        lhm.put("Number of IRQs", map);
    } catch (DebugProxyException e) {}

    try {
        long n_ctxswc = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.n_ctxswc"));
        HashMap<String, String> map = new HashMap<String, String>(4);
        map.put("best", "");
        map.put("worst", "");
        map.put("n", Long.toString(n_ctxswc));
        map.put("cumulative", "");
        lhm.put("Number of Context Switches", map);
    } catch (DebugProxyException e) {}

    try {
        long m_crit_thd_best = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.m_crit_thd.best"));
        long m_crit_thd_worst = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.m_crit_thd.worst"));
        long m_crit_thd_n = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.m_crit_thd.n"));
        String m_crit_thd_cumulative = evaluateExpression("(uint64_t)ch.kernel_stats.m_crit_thd.cumulative");
        HashMap<String, String> map = new HashMap<String, String>(4);
        map.put("best", Long.toString(m_crit_thd_best));
        map.put("worst", Long.toString(m_crit_thd_worst));
        map.put("n", Long.toString(m_crit_thd_n));
        map.put("cumulative", m_crit_thd_cumulative);
        lhm.put("Threads Critical Zones", map);
    } catch (DebugProxyException e) {}

    try {
        long m_crit_isr_best = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.m_crit_isr.best"));
        long m_crit_isr_worst = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.m_crit_isr.worst"));
        long m_crit_isr_n = HexUtils.parseNumber(evaluateExpression("(uint32_t)ch.kernel_stats.m_crit_isr.n"));
        String m_crit_isr_cumulative = evaluateExpression("(uint64_t)ch.kernel_stats.m_crit_isr.cumulative");
        HashMap<String, String> map = new HashMap<String, String>(4);
        map.put("best", Long.toString(m_crit_isr_best));
        map.put("worst", Long.toString(m_crit_isr_worst));
        map.put("n", Long.toString(m_crit_isr_n));
        map.put("cumulative", m_crit_isr_cumulative);
        lhm.put("ISRs Critical Zones", map);
    } catch (DebugProxyException e) {}

    return lhm;
  }
}
