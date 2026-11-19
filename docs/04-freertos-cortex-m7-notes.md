# 04 — FreeRTOS on Arm Cortex-M7 Study Notes

Reference notes for **Milestone 06 (FreeRTOS Port)**.
Primary sources: [*Mastering the FreeRTOS Real Time Kernel*](https://github.com/FreeRTOS/FreeRTOS-Kernel-Book/releases) and [FreeRTOS Cortex-M Interrupt Priority Guide](https://www.freertos.org/RTOS-Cortex-M3-M4.html).

---

## 1. Choosing the Right Cortex-M7 Port Layer

In `FreeRTOS-Kernel/portable/GCC/`, you will see two relevant ports:
* `ARM_CM7/r0p1/` — includes workarounds for Cortex-M7 `r0p1` silicon errata (e.g., erratum 837070).
* `ARM_CM4F/` — used for Cortex-M7 revisions **newer** than `r0p1` (such as `r1p0` / `r1p1`) that do not have the `r0p1` errata.

**Action when reaching Milestone 06:** Read `SCB->CPUID` (`0xE000_ED00`) on your S32K358 (or check the S32K3xx RM / Errata) to confirm the exact Cortex-M7 `Variant` (`rX`) and `Revision` (`pY`) fields before selecting the port directory.

---

## 2. Core Exception Mapping

FreeRTOS requires three Arm Cortex-M system handlers to be wired directly into your vector table:

| Vector Table Entry | FreeRTOS Handler Symbol | Role |
| :--- | :--- | :--- |
| `SVC_Handler` (Vector 11) | `vPortSVCHandler` | Starts the very first task from `vTaskStartScheduler()`. |
| `PendSV_Handler` (Vector 14) | `xPortPendSVHandler` | Performs context switches at the **lowest** interrupt priority so it never preempts hardware ISRs. |
| `SysTick_Handler` (Vector 15) | `xPortSysTickHandler` | Drives the RTOS tick (`xTaskIncrementTick()`) and pends `PendSV` when a higher-priority task wakes up. |

---

## 3. Interrupt Priorities & `BASEPRI` Rules (Most Common Bug Source!)

Understanding Cortex-M NVIC priorities is essential for a stable FreeRTOS port:

1. **Numerically Lower = Logically Higher Priority:** Priority `0` is the highest configurable urgency; priority `(2^configPRIO_BITS - 1)` is the lowest.
2. **Subpriority Bits Must Be Zero:** Configure `SCB->AIRCR` `PRIGROUP` so **all** implemented priority bits are preemption priority bits (zero subpriority bits).
3. **Why Priority `0` Cannot Call FreeRTOS `FromISR` APIs:**
   * FreeRTOS implements critical sections by writing `configMAX_SYSCALL_INTERRUPT_PRIORITY` into `BASEPRI`.
   * Writing `0` to `BASEPRI` **disables** `BASEPRI` masking altogether! Therefore, `BASEPRI` can never mask a priority `0` interrupt.
   * Any ISR that calls a FreeRTOS `...FromISR()` API **must** have a numerical priority `>= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (and `<= configLIBRARY_LOWEST_INTERRUPT_PRIORITY`).
4. **Hardware Shift (`configPRIO_BITS`):**
   * NVIC `IPR` registers store priority in the **most-significant bits** of each 8-bit byte.
   * Verify `configPRIO_BITS` for S32K358 (typically `4`, meaning priorities `0..15` are shifted left by `4` when written to `IPR`/`SHPR`/`BASEPRI`).
5. **Enable `configASSERT()`:** Define `configASSERT(x)` during development — FreeRTOS's port layer includes built-in assertions that immediately catch misconfigured `PRIGROUP` or illegal ISR priorities!
