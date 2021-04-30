/********************************************************************************
 * Copyright (c) 2019 Giovanni Di Sirio.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

package org.chibios.tools.eclipse.debug.views;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map.Entry;
import java.util.Set;

import org.chibios.tools.eclipse.debug.utils.HexUtils;
import org.chibios.tools.eclipse.debug.utils.DebugProxyException;
import org.chibios.tools.eclipse.debug.utils.KernelObjects;
import org.eclipse.ui.internal.IWorkbenchThemeConstants;
import org.eclipse.ui.part.*;
import org.eclipse.ui.themes.ITheme;
import org.eclipse.jface.action.*;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;

import org.eclipse.wb.swt.SWTResourceManager;
import org.eclipse.wb.swt.ResourceManager;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;

/**
 * This sample class demonstrates how to plug-in a new workbench view. The view
 * shows data obtained from the model. The sample creates a dummy model on the
 * fly, but a real implementation would connect to the model available either in
 * this or another plug-in (e.g. the workspace). The view is connected to the
 * model using a content provider.
 * <p>
 * The view uses a label provider to define how model objects should be
 * presented in the view. Each view can present the same model objects using
 * different labels and icons, if needed. Alternatively, a single label provider
 * can be shared between views in order to ensure that objects of the same type
 * are presented in the same way everywhere.
 * <p>
 */

@SuppressWarnings("restriction")
public class ChibiView extends ViewPart {

  /**
   * The ID of the view as specified by the extension.
   */
  public static final String ID = "org.chibios.tools.eclipse.debug.views.ChibiView";

  private CTabFolder tabFolder;
  private CTabItem tbtmGlobal;
  private CTabItem tbtmThreads;
  private CTabItem tbtmTimers;
  private CTabItem tbtmTraceBuffer;
  private CTabItem tbtmStatistics;

  private Action refreshAction;
  private Table globalTable;
  private Table threadsTable;
  private Table timersTable;
  private Table tbTable;
  private Table statisticsTable;

  private KernelObjects debugger;

  private ITheme theme;

  private FocusAdapter focus = new FocusAdapter() {
    @Override
    public void focusLost(FocusEvent e) {
      setInactive();
    }
    @Override
    public void focusGained(FocusEvent e) {
      setActive();
    }
  };

  /**
   * The constructor.
   */
  public ChibiView() {
    
    theme = PlatformUI.getWorkbench().getThemeManager().getCurrentTheme();

    debugger = new KernelObjects();
  }

  private void setActive() {
    tabFolder.setSelectionBackground(
        new org.eclipse.swt.graphics.Color[] {
            theme.getColorRegistry().get(IWorkbenchThemeConstants.ACTIVE_TAB_BG_START),
            theme.getColorRegistry().get(IWorkbenchThemeConstants.ACTIVE_TAB_BG_END)
        },
        new int[] {100},
        true);
    tabFolder.setSelectionForeground(theme.getColorRegistry().get(IWorkbenchThemeConstants.ACTIVE_TAB_TEXT_COLOR));
  }

  private void setInactive() {
    tabFolder.setSelectionBackground(
        new org.eclipse.swt.graphics.Color[] {
            theme.getColorRegistry().get(IWorkbenchThemeConstants.INACTIVE_TAB_BG_START),
            theme.getColorRegistry().get(IWorkbenchThemeConstants.INACTIVE_TAB_BG_END)
        },
        new int[] {theme.getInt(IWorkbenchThemeConstants.ACTIVE_TAB_PERCENT)},
        true);
    tabFolder.setSelectionForeground(theme.getColorRegistry().get(IWorkbenchThemeConstants.INACTIVE_TAB_TEXT_COLOR));
  }

  /**
   * This is a callback that will allow us to create the viewer and initialize
   * it.
   */
  public void createPartControl(Composite parent) {

    tabFolder = new CTabFolder(parent, SWT.BORDER | SWT.BOTTOM);
    tabFolder.setFont(theme.getFontRegistry().get(IWorkbenchThemeConstants.TAB_TEXT_FONT));
    tabFolder.setBackground(theme.getColorRegistry().get(IWorkbenchThemeConstants.INACTIVE_TAB_BG_END));
    tabFolder.addSelectionListener(new SelectionAdapter() {
      @Override
      public void widgetSelected(SelectionEvent e) {
        setActive();
        tabFolder.getSelection().getControl().setFocus();
      }
    });
    tabFolder.addFocusListener(new FocusAdapter() {
      @Override
      public void focusGained(FocusEvent e) {
        tabFolder.getSelection().getControl().setFocus();
      }
    });
    setInactive();
    tabFolder.setSimple(false);

    tbtmGlobal = new CTabItem(tabFolder, SWT.NONE);
    tbtmGlobal.setText("Global");
    
    globalTable = new Table(tabFolder, SWT.FULL_SELECTION);
    globalTable.addFocusListener(focus);
    globalTable.setFont(SWTResourceManager.getFont("Courier New", 8, SWT.NORMAL));
    tbtmGlobal.setControl(globalTable);
    globalTable.setHeaderVisible(true);

    TableColumn tblclmnGlobalHidden = new TableColumn(globalTable, SWT.RIGHT);
    tblclmnGlobalHidden.setWidth(0);
    tblclmnGlobalHidden.setText("");

    TableColumn tblclmnGlobalVariableName = new TableColumn(globalTable, SWT.LEFT);
    tblclmnGlobalVariableName.setWidth(150);
    tblclmnGlobalVariableName.setText("Variable");

    TableColumn tblclmnGlobalVariableValue = new TableColumn(globalTable, SWT.LEFT);
    tblclmnGlobalVariableValue.setWidth(300);
    tblclmnGlobalVariableValue.setText("Value");

    TableColumn tblclmnGlobalVariableFiller = new TableColumn(globalTable, SWT.FILL);
    tblclmnGlobalVariableFiller.setWidth(1);
    tblclmnGlobalVariableFiller.setText("");

    tbtmThreads = new CTabItem(tabFolder, SWT.NONE);
    tbtmThreads.setText("Threads");

    threadsTable = new Table(tabFolder, SWT.FULL_SELECTION);
    threadsTable.addFocusListener(focus);
    tbtmThreads.setControl(threadsTable);
    threadsTable.setFont(SWTResourceManager.getFont("Courier New", 8, SWT.NORMAL));
    threadsTable.setHeaderVisible(true);

    TableColumn tblclmnThreadAddress = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadAddress.setWidth(72);
    tblclmnThreadAddress.setText("Address");

    TableColumn tblclmnThreadLimit = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadLimit.setWidth(72);
    tblclmnThreadLimit.setText("StkLimit");

    TableColumn tblclmnThreadStack = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadStack.setWidth(72);
    tblclmnThreadStack.setText("Stack");

    TableColumn tblclmnThreadUsed = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadUsed.setWidth(72);
    tblclmnThreadUsed.setText("StkUnused");

    TableColumn tblclmnThreadName = new TableColumn(threadsTable, SWT.LEFT);
    tblclmnThreadName.setWidth(144);
    tblclmnThreadName.setText("Name");

    TableColumn tblclmnThreadState = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadState.setWidth(72);
    tblclmnThreadState.setText("State");

    TableColumn tblclmnThreadFlags = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadFlags.setWidth(40);
    tblclmnThreadFlags.setText("Flgs");

    TableColumn tblclmnThreadPriority = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadPriority.setWidth(40);
    tblclmnThreadPriority.setText("Prio");

    TableColumn tblclmnThreadRefs = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadRefs.setWidth(40);
    tblclmnThreadRefs.setText("Refs");

    TableColumn tblclmnThreadTime = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadTime.setWidth(64);
    tblclmnThreadTime.setText("Time");

    TableColumn tblclmnThreadShared = new TableColumn(threadsTable, SWT.LEFT);
    tblclmnThreadShared.setWidth(72);
    tblclmnThreadShared.setText("Obj/Msg");
    
    TableColumn tblclmnThreadContextSwitches = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadContextSwitches.setWidth(96);
    tblclmnThreadContextSwitches.setText("Switches");
    
    TableColumn tblclmnThreadWorstPath = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadWorstPath.setWidth(96);
    tblclmnThreadWorstPath.setText("Worst Path");
    
    TableColumn tblclmnThreadCumulativeTime = new TableColumn(threadsTable, SWT.RIGHT);
    tblclmnThreadCumulativeTime.setWidth(192);
    tblclmnThreadCumulativeTime.setText("Cumulative Time");

    TableColumn tblclmnThreadFiller = new TableColumn(threadsTable, SWT.FILL);
    tblclmnThreadFiller.setWidth(1);
    tblclmnThreadFiller.setText("");

    tbtmTimers = new CTabItem(tabFolder, SWT.NONE);
    tbtmTimers.setText("Timers");

    timersTable = new Table(tabFolder, SWT.FULL_SELECTION);
    timersTable.addFocusListener(focus);
    tbtmTimers.setControl(timersTable);
    timersTable.setFont(SWTResourceManager.getFont("Courier New", 8, SWT.NORMAL));
    timersTable.setHeaderVisible(true);

    TableColumn tblclmnTimerAddress = new TableColumn(timersTable, SWT.RIGHT);
    tblclmnTimerAddress.setWidth(72);
    tblclmnTimerAddress.setText("Address");

    TableColumn tblclmnTimerTime = new TableColumn(timersTable, SWT.RIGHT);
    tblclmnTimerTime.setWidth(72);
    tblclmnTimerTime.setText("Time");

    TableColumn tblclmnTimerDelta = new TableColumn(timersTable, SWT.RIGHT);
    tblclmnTimerDelta.setWidth(72);
    tblclmnTimerDelta.setText("Delta");

    TableColumn tblclmnTimerCallback = new TableColumn(timersTable, SWT.RIGHT);
    tblclmnTimerCallback.setWidth(72);
    tblclmnTimerCallback.setText("Callback");

    TableColumn tblclmnTimerParameter = new TableColumn(timersTable, SWT.LEFT);
    tblclmnTimerParameter.setWidth(72);
    tblclmnTimerParameter.setText("Param");

    TableColumn tblclmnTimerLastDeadline = new TableColumn(timersTable, SWT.RIGHT);
    tblclmnTimerLastDeadline.setWidth(100);
    tblclmnTimerLastDeadline.setText("Last Deadline");

    TableColumn tblclmnTimerReload = new TableColumn(timersTable, SWT.RIGHT);
    tblclmnTimerReload.setWidth(100);
    tblclmnTimerReload.setText("Reload");

    TableColumn tblclmnTimerFiller = new TableColumn(timersTable, SWT.FILL);
    tblclmnTimerFiller.setWidth(1);
    tblclmnTimerFiller.setText("");

    tbtmTraceBuffer = new CTabItem(tabFolder, SWT.NONE);
    tbtmTraceBuffer.setText("TraceBuffer");
    
    tbTable = new Table(tabFolder, SWT.FULL_SELECTION);
    tbTable.addFocusListener(focus);
    tbTable.setFont(SWTResourceManager.getFont("Courier New", 8, SWT.NORMAL));
    tbtmTraceBuffer.setControl(tbTable);
    tbTable.setHeaderVisible(true);

    TableColumn tblclmnTraceBufferHidden = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferHidden.setWidth(0);
    tblclmnTraceBufferHidden.setText("");

    TableColumn tblclmnTraceBufferIndex = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferIndex.setWidth(48);
    tblclmnTraceBufferIndex.setText("Event");

    TableColumn tblclmnTraceBufferType = new TableColumn(tbTable, SWT.LEFT);
    tblclmnTraceBufferType.setWidth(80);
    tblclmnTraceBufferType.setText("Type");

    TableColumn tblclmnTraceBufferTime = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferTime.setWidth(80);
    tblclmnTraceBufferTime.setText("SysTime");

    TableColumn tblclmnTraceBufferStamp = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferStamp.setWidth(80);
    tblclmnTraceBufferStamp.setText("RT Stamp");

    TableColumn tblclmnTraceBufferPrevAddress = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferPrevAddress.setWidth(72);
    tblclmnTraceBufferPrevAddress.setText("From");

    TableColumn tblclmnTraceBufferPrevName = new TableColumn(tbTable, SWT.LEFT);
    tblclmnTraceBufferPrevName.setWidth(144);
    tblclmnTraceBufferPrevName.setText("Name");

    TableColumn tblclmnTraceBufferState = new TableColumn(tbTable, SWT.LEFT);
    tblclmnTraceBufferState.setWidth(72);
    tblclmnTraceBufferState.setText("State");

    TableColumn tblclmnTraceBufferShared = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferShared.setWidth(72);
    tblclmnTraceBufferShared.setText("Obj/Msg");

    TableColumn tblclmnTraceBufferCurrentAddress = new TableColumn(tbTable, SWT.RIGHT);
    tblclmnTraceBufferCurrentAddress.setWidth(72);
    tblclmnTraceBufferCurrentAddress.setText("To");

    TableColumn tblclmnTraceBufferCurrentName = new TableColumn(tbTable, SWT.LEFT);
    tblclmnTraceBufferCurrentName.setWidth(144);
    tblclmnTraceBufferCurrentName.setText("Name");

    TableColumn tblclmnTraceBufferFiller = new TableColumn(tbTable, SWT.FILL);
    tblclmnTraceBufferFiller.setWidth(1);
    tblclmnTraceBufferFiller.setText("");

    tbtmStatistics = new CTabItem(tabFolder, SWT.NONE);
    tbtmStatistics.setText("Statistics");
    
    statisticsTable = new Table(tabFolder, SWT.FULL_SELECTION);
    statisticsTable.setHeaderVisible(true);
    statisticsTable.setFont(SWTResourceManager.getFont("Courier New", 8, SWT.NORMAL));
    tbtmStatistics.setControl(statisticsTable);
    
    TableColumn tblclmnStatsHidden = new TableColumn(statisticsTable, SWT.RIGHT);
    tblclmnStatsHidden.setWidth(0);
    tblclmnStatsHidden.setText("");
    
    TableColumn tblclmnStatsMeasuredSection = new TableColumn(statisticsTable, SWT.LEFT);
    tblclmnStatsMeasuredSection.setWidth(200);
    tblclmnStatsMeasuredSection.setText("Measured Section");
    
    TableColumn tblclmnStatsBestCase = new TableColumn(statisticsTable, SWT.RIGHT);
    tblclmnStatsBestCase.setWidth(96);
    tblclmnStatsBestCase.setText("Best Case");
    
    TableColumn tblclmnStatsWorstCase = new TableColumn(statisticsTable, SWT.RIGHT);
    tblclmnStatsWorstCase.setWidth(96);
    tblclmnStatsWorstCase.setText("Worst Case");
    
    TableColumn tblclmnStatsIterations = new TableColumn(statisticsTable, SWT.RIGHT);
    tblclmnStatsIterations.setWidth(96);
    tblclmnStatsIterations.setText("Iterations");
    
    TableColumn tblclmnStatsCumulative = new TableColumn(statisticsTable, SWT.RIGHT);
    tblclmnStatsCumulative.setWidth(192);
    tblclmnStatsCumulative.setText("Cumulative Time");

    TableColumn tblclmnStatsFiller = new TableColumn(statisticsTable, SWT.FILL);
    tblclmnStatsFiller.setWidth(1);
    tblclmnStatsFiller.setText("");

    makeActions();
    hookContextMenu();
    contributeToActionBars();

    tabFolder.setSelection(tbtmGlobal);
  }

  private void hookContextMenu() {
    MenuManager menuMgr = new MenuManager("#PopupMenu");
    menuMgr.setRemoveAllWhenShown(true);
    menuMgr.addMenuListener(new IMenuListener() {
      public void menuAboutToShow(IMenuManager manager) {
        ChibiView.this.fillContextMenu(manager);
      }
    });
  }

  private void contributeToActionBars() {
    IActionBars bars = getViewSite().getActionBars();
    fillLocalPullDown(bars.getMenuManager());
    fillLocalToolBar(bars.getToolBarManager());
  }

  private void fillLocalPullDown(IMenuManager manager) {
    manager.add(refreshAction);
/*    manager.add(new Separator());
    manager.add(refreshAction);*/
  }

  private void fillContextMenu(IMenuManager manager) {
    manager.add(refreshAction);
    // Other plug-ins can contribute there actions here
    manager.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));
  }

  private void fillLocalToolBar(IToolBarManager manager) {
    manager.add(refreshAction);
  }

  private void fillGlobalTable() {
    LinkedHashMap<String, String> lhm;

    // If the debugger is not yet present then do nothing.
    if (debugger == null)
      return;

    // Reading the list of global variables, null can be returned if the debugger
    // does not respond.
    try {
      lhm = debugger.readGlobalVariables();
      if (lhm == null)
        return;
    } catch (DebugProxyException e) {
      showMessage("Error: " + e.getMessage() + ".");
      return;
    }

    globalTable.removeAll();
    
    Set<Entry<String, String>> set = lhm.entrySet();
    for (Entry<String, String> entry : set) {
      TableItem tableItem = new TableItem(globalTable, SWT.NONE);
      tableItem.setText(new String[] {
        "",
        entry.getKey(),
        entry.getValue()
      });
    }
  }

  private String makeHex(String s) {
    try {
      s = HexUtils.dword2HexString((int)HexUtils.parseNumber(s));
    } catch (Exception e) {}
    return s;
  }

  private void fillThreadsTable() {
    LinkedHashMap<String, HashMap<String, String>> lhm;

    // If the debugger is not yet present then do nothing.
    if (debugger == null)
      return;

    // Reading the list of threads, null can be returned if the debugger
    // does not respond.
    try {
      lhm = debugger.readThreads();
      if (lhm == null)
        return;
    } catch (DebugProxyException e) {
      showMessage("Error: " + e.getMessage() + ".");
      return;
    }

    threadsTable.removeAll();

    Set<Entry<String, HashMap<String, String>>> set = lhm.entrySet();
    for (Entry<String, HashMap<String, String>> entry : set) {
      HashMap<String, String> map = entry.getValue();
      TableItem tableItem = new TableItem(threadsTable, SWT.NONE);
      tableItem.setText(new String[] {
        makeHex(entry.getKey()),
        makeHex(map.get("stklimit")),
        makeHex(map.get("stack")),
        map.get("stkunused"),
        map.get("name"),
        map.get("state_s"),
        HexUtils.byte2HexString((int)HexUtils.parseNumber(map.get("flags"))),
        map.get("prio"),
        map.get("refs"),
        map.get("time"),
        makeHex(map.get("wtobjp")),
        map.get("stats_n"),
        map.get("stats_worst"),
        map.get("stats_cumulative")
      });
    }
  }

  private void fillTimersTable() {
    LinkedHashMap<String, HashMap<String, String>> lhm;

    // If the debugger is not yet present then do nothing.
    if (debugger == null)
      return;

    // Reading the list of threads, null can be returned if the debugger
    // does not respond.
    try {
      lhm = debugger.readTimers();
      if (lhm == null)
        return;
    } catch (DebugProxyException e) {
      showMessage("Error: " + e.getMessage() + ".");
      return;
    }

    timersTable.removeAll();

    Set<Entry<String, HashMap<String, String>>> set = lhm.entrySet();
    long time = 0;
    for (Entry<String, HashMap<String, String>> entry : set) {
      HashMap<String, String> map = entry.getValue();
      time = time + HexUtils.parseNumber(map.get("delta"));
      TableItem tableItem = new TableItem(timersTable, SWT.NONE);
      tableItem.setText(new String[] {
        makeHex(entry.getKey()),
        Long.toString(time),
        "+" + HexUtils.parseNumber(map.get("delta")),
        makeHex(map.get("func")),
        makeHex(map.get("par")),
        map.get("last"),
        map.get("reload")
      });          
    }
  }

  private void fillTraceBufferTable() {
    LinkedHashMap<String, HashMap<String, String>> lhm, lhmthreads;

    // If the debugger is not yet present then do nothing.
    if (debugger == null)
      return;

    // Read active threads for retrieving names.
    try {
      lhmthreads = debugger.readThreads();
      if (lhmthreads == null)
        return;
    } catch (DebugProxyException e) {
      lhmthreads = new LinkedHashMap<String, HashMap<String, String>>(0);
    }

    // Reading the list of threads, null can be returned if the debugger
    // does not respond.
    try {
      lhm = debugger.readTraceBuffer();
      if (lhm == null)
        return;
    } catch (DebugProxyException e) {
      showMessage("Error: " + e.getMessage() + ".");
      return;
    }

    tbTable.removeAll();

    Set<Entry<String, HashMap<String, String>>> set = lhm.entrySet();
    String prev = "";
    String prevname = "";
    for (Entry<String, HashMap<String, String>> entry : set) {
      HashMap<String, String> map = entry.getValue();
      TableItem tableItem = new TableItem(tbTable, SWT.NONE);

      // Fixed fields.
      int typen = (int)HexUtils.parseNumber(map.get("type"));
      String state = map.get("state_s");
      String rtstamp = map.get("rtstamp");
      String time = map.get("time");

      // Fields depending on type.
      String[] line;
      switch (typen) {
      case 1:
        // Searches the current thread into the threads map.
        {
          String tpaddr = map.get("sw_tp");
          HashMap<String, String> thread = lhmthreads.get(tpaddr);
          String name;
          if (thread != null)
            name = thread.get("name");
          else
            name = "";
          String tp = makeHex(tpaddr);
          String msg = makeHex(map.get("sw_msg"));

          line = new String[] {
            "",
            entry.getKey(),
            "Ready",
            time,
            rtstamp,
            tp,
            name,
            "",
            msg,
            "",
            ""
          };
        }
        break;
      case 2:
        // Searches the current thread into the threads map.
        {
          String currentaddr = map.get("sw_ntp");
          HashMap<String, String> thread = lhmthreads.get(currentaddr);
          String currentname;
          if (thread != null)
            currentname = thread.get("name");
          else
            currentname = "";
          String current = makeHex(currentaddr);
          String wtobjp = makeHex(map.get("sw_wtobjp"));

          line = new String[] {
            "",
            entry.getKey(),
            "Switch",
            time,
            rtstamp,
            prev,
            prevname,
            state,
            wtobjp,
            current,
            currentname
          };
          prev = current;
          prevname = currentname;
        }
        break;
      case 3:
        line = new String[] {
          "",
          entry.getKey(),
          "ISR-enter",
          time,
          rtstamp,
          "",
          map.get("isr_name_s"),
          "",
          "",
          "",
          ""
        };
        break;
      case 4:
        line = new String[] {
          "",
          entry.getKey(),
          "ISR-leave",
          time,
          rtstamp,
          "",
          map.get("isr_name_s"),
          "",
          "",
          "",
          ""
        };
        break;
      case 5:
        line = new String[] {
          "",
          entry.getKey(),
          "Halt",
          time,
          rtstamp,
          "",
          map.get("halt_reason_s"),
          "",
          "",
          "",
          ""
        };
        break;
      case 6:
        line = new String[] {
          "",
          entry.getKey(),
          "User",
          time,
          rtstamp,
          makeHex(map.get("user_up1")),
          "",
          "",
          "",
          makeHex(map.get("user_up2")),
          ""
        };
        break;
      default:
        line = new String[] {
          "",
          entry.getKey(),
          "Unknown",
          time,
          rtstamp,
          "",
          "",
          "",
          "",
          "",
          ""
        };
      }

      tableItem.setText(line);
    }
  }


  private void fillStatisticsTable() {
      LinkedHashMap<String, HashMap<String, String>> lhm;

    // If the debugger is not yet present then do nothing.
    if (debugger == null)
      return;

    // Reading the list of global variables, null can be returned if the debugger
    // does not respond.
    try {
      lhm = debugger.readStatistics();
      if (lhm == null)
        return;
    } catch (DebugProxyException e) {
      showMessage("Error: " + e.getMessage() + ".");
      return;
    }

    statisticsTable.removeAll();
    
    Set<Entry<String, HashMap<String, String>>> set = lhm.entrySet();
    for (Entry<String, HashMap<String, String>> entry : set) {
      TableItem tableItem = new TableItem(statisticsTable, SWT.NONE);
      tableItem.setText(new String[] {
        "",
        entry.getKey(),
        entry.getValue().get("best"),
        entry.getValue().get("worst"),
        entry.getValue().get("n"),
        entry.getValue().get("cumulative")
      });
    }
  }

  private void makeActions() {
    
    // Refresh action.
    refreshAction = new Action() {
      public void run() {
        CTabItem tabitem = tabFolder.getSelection();
        if (tabitem == null)
          return;
        if (tabitem == tbtmGlobal)
          fillGlobalTable();
        else if (tabitem == tbtmThreads)
          fillThreadsTable();
        else if (tabitem == tbtmTimers)
          fillTimersTable();
        else if (tabitem == tbtmTraceBuffer)
          fillTraceBufferTable();
        else if (tabitem == tbtmStatistics)
          fillStatisticsTable();
      }
    };
    refreshAction.setDisabledImageDescriptor(ResourceManager.getPluginImageDescriptor("org.eclipse.cdt.ui", "/icons/dlcl16/refresh_nav.gif"));
    refreshAction.setImageDescriptor(ResourceManager.getPluginImageDescriptor("org.eclipse.cdt.ui", "/icons/elcl16/refresh_nav.gif"));
    refreshAction.setText("Refresh");
    refreshAction.setToolTipText("Refresh current view");
  }

  private void showMessage(String message) {
    MessageDialog.openInformation(tabFolder.getShell(),
        "ChibiOS/RT Views", message);
  }

  /**
   * Passing the focus request to the viewer's control.
   */
  public void setFocus() {
    tabFolder.setFocus();
  }
}
