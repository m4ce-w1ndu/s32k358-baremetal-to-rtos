# 02 — Arm Cortex-M7 Core, Exceptions & Memory Barriers

This reference summarizes the **Arm Cortex-M7** architectural concepts needed for Milestones 02, 04, and 06. Primary sources: **DUI0646** (*Cortex-M7 Devices Generic User Guide*), **DDI0403** (*ARMv7-M Architecture Reference Manual*), and **DAI0321** (*AN321: Memory Barrier Instructions*).

---

## 1. Programmer's Model & Core Registers (`DUI0646` §2.1)

* **General-Purpose Registers:** `R0`–`R12`
  * `R0`–`R3`: Scratch / argument / return registers (caller-saved per AAPCS).
  * `R4`–`R11`: Callee-saved registers.
  * `R12` (`IP`): Intra-procedure-call scratch register.
* **Special Registers:**
  * `R13` (`SP`): Banked as **MSP** (Main Stack Pointer — used at reset and in Handler mode) and **PSP** (Process Stack Pointer — used by FreeRTOS tasks in Thread mode).
  * `R14` (`LR`): Link Register (holds return address, or `EXC_RETURN` magic value inside an ISR).
  * `R15` (`PC`): Program Counter (Thumb state bit 0 is always 1 for branch targets / vector entries).
  * `xPSR`: Combined Program Status Register (`APSR` flags, `IPSR` active exception number, `EPSR` Thumb bit).
* **Interrupt Masking Registers:**
  * `PRIMASK`: 1-bit mask; disables all configurable-priority exceptions (leaves Reset, NMI, HardFault active).
  * `BASEPRI`: Priority threshold mask; blocks all exceptions with priority value **greater than or equal to** `BASEPRI` (remember: on Arm Cortex-M, **lower numerical value = higher urgency**). Used heavily by FreeRTOS.
  * `FAULTMASK`: Disables all exceptions including HardFault (only NMI remains).
  * `CONTROL`: Selects privilege level (`nPRIV`), active stack pointer (`SPSEL`: MSP vs. PSP), and FPU context active (`FPCA`).

---

## 2. Vector Table & Exception Model (`DUI0646` §2.3)

The Cortex-M7 vector table must be naturally aligned (at least 128-byte aligned, or next power-of-two ≥ `4 * total_vectors`; check `SCB->VTOR` alignment rules in `DUI0646` §4.3.4):

| Vector # | Offset | Exception | Notes |
| :---: | :---: | :--- | :--- |
| 0 | `0x00` | Initial `MSP` | Loaded into `SP` on reset (must be 8-byte aligned). |
| 1 | `0x04` | `Reset_Handler` | Entry point (Thumb bit 0 set automatically by assembler/linker for `.thumb_func`). |
| 2 | `0x08` | `NMI_Handler` | Fixed priority -2. |
| 3 | `0x0C` | `HardFault_Handler` | Fixed priority -1. |
| 4–6 | `0x10`–`0x18` | `MemManage`, `BusFault`, `UsageFault` | Configurable fault handlers (useful to implement early for debugging ECC/alignment faults!). |
| 7–10 | `0x1C`–`0x28` | Reserved | Must be `0`. |
| 11 | `0x2C` | `SVC_Handler` | Used by FreeRTOS to launch the first task. |
| 12–13 | `0x30`–`0x34` | `DebugMon`, Reserved | — |
| 14 | `0x38` | `PendSV_Handler` | Used by FreeRTOS for deferred context switching (lowest priority). |
| 15 | `0x3C` | `SysTick_Handler` | Core 24-bit system tick timer ISR. |
| 16+ | `0x40+` | `IRQ0`..`IRQn` | Vendor-specific peripheral interrupts (see `S32K3XX_interrupt_map.xlsx`). |

### Hardware Exception Stacking
On exception entry, the Cortex-M7 hardware automatically pushes an **8-word frame** onto the active stack:
`[R0, R1, R2, R3, R12, LR, ReturnAddress, xPSR]` (plus optional FPU registers if FPU context is active). Because the hardware saves all AAPCS caller-saved registers, normal C functions can be used directly as ISRs!

---

## 3. Core Peripherals in PPB (`0xE000_0000` — `DUI0646` §4)

| Peripheral | Base Address | Key Registers to Know |
| :--- | :--- | :--- |
| **SysTick** | `0xE000_E010` | `SYST_CSR` (Control & Status), `SYST_RVR` (24-bit Reload), `SYST_CVR` (Current Value), `SYST_CALIB` |
| **NVIC** | `0xE000_E100` | `ISER0..7` (Set-Enable), `ICER0..7` (Clear-Enable), `ISPR`/`ICPR` (Pending), `IPR0..59` (Byte-accessible Priority) |
| **SCB** | `0xE000_ED00` | `CPUID`, `ICSR`, `VTOR` (Vector Table Offset), `AIRCR` (`PRIGROUP` + `VECTKEY 0x05FA`), `SHPR1..3` (System Handler Priorities), `SHCSR`, `CFSR`/`HFSR`/`BFAR` (Fault status), `CPACR` (FPU access) |

> **Important Check for S32K358:** Check how many **NVIC priority bits** are implemented in S32K358 (typically the top 4 bits of each 8-bit `IPR` field, i.e., 16 priority levels `0..15`, shifted left by 4). Verify this in `S32K3XXRM`!

---

## 4. `volatile` & Memory Barriers on Cortex-M7 (`DAI0321`)

Cortex-M7 is a **dual-issue superscalar, pipelined** processor with write buffers and optional caches. Both the **compiler** and the **CPU hardware** can reorder memory accesses unless constrained:

1. **Compiler Ordering (`volatile`):**
   * Every memory-mapped hardware register pointer **must** be qualified `volatile` (e.g., `volatile uint32_t *`).
   * `volatile` stops the compiler from caching register values in CPU registers, eliminating "redundant" reads/writes, or reordering accesses to two `volatile` objects.
   * `volatile` does **not** emit hardware memory barrier instructions.

2. **Hardware Ordering (`DMB`, `DSB`, `ISB`):**
   * **`DMB` (Data Memory Barrier):** Ensures all explicit memory accesses before `DMB` are observed before any explicit memory accesses after `DMB`.
   * **`DSB` (Data Synchronization Barrier):** Halts execution until *all* prior memory transactions (including buffered writes and cache/TLB maintenance) have fully completed.
   * **`ISB` (Instruction Synchronization Barrier):** Flushes the processor pipeline so subsequent instructions are fetched under the new architectural state.

### When You Need Barriers in This Project (`DAI0321` Table 1)
* **Relocating `SCB->VTOR`:** `DSB` + `ISB` after writing `VTOR`.
* **Enabling FPU (`SCB->CPACR`):** `DSB` + `ISB` before executing any floating-point instructions.
* **Modifying `NVIC` / `PRIMASK` / `BASEPRI`:** `DSB` + `ISB` after disabling an interrupt if subsequent code depends on the ISR no longer firing, or after updating interrupt configuration.
* **Clearing a Peripheral Interrupt Flag inside an ISR:** Ensure the write completes (`DSB` or read-back of the peripheral register) before returning from the ISR, otherwise the buffered write may not reach the peripheral before `EXC_RETURN` and the ISR will immediately re-enter!
* **Before `WFI` / `WFE`:** Always execute `DSB` prior to entering sleep.
