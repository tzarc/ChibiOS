; ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.
;
; This file is part of ChibiOS/RT.
;
; ChibiOS/RT is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 3 of the License, or
; (at your option) any later version.

; ChibiOS/RT is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.

; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

        ?PR??_THREAD_START?CHCOREASM segment CODE

        rseg        ?PR??_THREAD_START?CHCOREASM

        ; Performs a context switch between two threads.
        ; In this port swapping the stack pointers is enough, there are
        ; no registers to be preserved across function calls and the
        ; program counter is already in the stack.
        public      ?port_switch
?port_switch:
        LDW         Y,SP
        LDW         (005H,X),Y      ; SP saved in otp->p_ctx.sp
        LDW         X,(003H,SP)     ; ntp
        LDW         X,(005H,X)
        LDW         SP,X            ; SP restored from ntp->p_ctx.sp
        RET

        public      ?_port_thread_start
?_port_thread_start:
        RIM
        POPW        X
        RET

        end
