# 01 — NXP S32K358 Architecture & Memory Map Reference

This guide summarizes the key architectural facts and S32K3-specific hardware behaviors you must account for when writing bare-metal linker scripts, startup code, and peripheral drivers.

> **Verification Rule:** Always cross-check exact sizes, offsets, and reserved regions against your downloaded **S32K3xx Reference Manual (`S32K3XXRM`)** and **`S32K3XX_memory_map.xlsx`** before writing code.

---

## 1. S32K358 Core & System Overview

* **CPU Architecture:** Arm Cortex-M7 (ARMv7E-M), up to **240 MHz**
* **Core Topology:** Multi-core Cortex-M7 configuration (dual-core lockstep `CM7_0` for ASIL-D + decoupled additional Cortex-M7 core(s); bare-metal milestones 01–06 start on the primary boot core `CM7_0`)
* **Floating-Point Unit:** Single & double-precision FPU (`FPv5-D16`) — disabled by default at reset (must be enabled via `SCB->CPACR` if using hard-float)
* **Caches:** L1 Instruction Cache and Data Cache (disabled at reset)
* **Security Subsystem:** Hardware Security Engine (**HSE-B**) — executes **SBAF** (Secure Boot Assist Flash) out of reset before releasing `CM7_0`

---

## 2. High-Level Memory Map (S32K358)

The S32K358 separates **core-local Tightly-Coupled Memories (TCM)**, **Program Flash (PFLASH)**, **System SRAM**, **Peripheral Bus Space**, and **Cortex-M7 Private Peripherals**:

| Region | Typical Base Address | Notes / Purpose |
| :--- | :--- | :--- |
| **ITCM** (Instruction TCM) | `0x0000_0000` | Core-local, zero-wait-state instruction RAM (ECC-protected). |
| **PFLASH** (Program Flash) | `0x0040_0000` | Up to 8 MB on S32K358. Holds IVT, vector table, `.text`, `.rodata`, and `.data` load image (LMA). Check RM for reserved/HSE regions at the top of Flash. |
| **DFLASH** (Data Flash) | `0x1000_0000` | Dedicated data flash / EEPROM emulation area. |
| **DTCM** (Data TCM) | `0x2000_0000` | Core-local, zero-wait-state data RAM (ECC-protected). Ideal for fast stack/data once initialized. |
| **System SRAM** | `0x2040_0000` | Up to 1152 KB across SRAM banks (ECC-protected). Holds `.data` (VMA), `.bss`, heap, and stack. |
| **Peripheral Space** | `0x4000_0000` | AIPS0 / AIPS1 / AIPS2 peripheral bridges (`SIUL2`, `MC_ME`, `MC_CGM`, `LPUART`, `SWT`, etc.). |
| **Cortex-M7 PPB** | `0xE000_0000` | Core-internal registers: `SysTick` (`0xE000_E010`), `NVIC` (`0xE000_E100`), `SCB` (`0xE000_ED00`). |

### Key Peripheral Base Addresses to Verify in `S32K3XX_memory_map.xlsx`
* **`MC_ME`** (Mode Entry Module): `0x402DC000`
* **`MC_CGM`** (Clock Generation Module): verify exact base in RM
* **`SIUL2`** (Pin Muxing & GPIO): `0x40290000`
* **`SWT0`** (Software Watchdog Timer 0): verify base and default reset state in RM
* **`LPUART0`..`LPUART15`**: verify exact instance base address matching your board's USB-UART pins

---

## 3. Critical S32K3xx Bare-Metal Gotchas

If you have previously done bare-metal on STM32 or NXP LPC/i.MX RT, **three S32K3xx hardware behaviors** will immediately bite you if overlooked:

### Gotcha A: Boot Flow — SBAF & Image Vector Table (IVT)
* On a generic Cortex-M MCU, the core fetches `[Initial SP, Reset_Handler]` directly from Flash offset `0x0`.
* On **S32K3xx**, the **HSE-B** core runs **SBAF** first. SBAF scans specific PFLASH offsets (starting at `0x0040_0000`) for a valid **IVT** header (magic word `0x5AA55AA5`), reads the `CM7_0` application start address (pointing to your Cortex-M7 vector table), and then releases `CM7_0` from reset.
* **Impact on Milestones 01 & 02:** Your linker script and startup image need a valid **IVT** placed at the start of PFLASH (unless your debugger explicitly loads `PC`/`SP` directly into RAM/Flash during debug sessions). Study **AN13465** and the **Boot** chapter of `S32K3XXRM`.

### Gotcha B: 64-Bit ECC SRAM & TCM Initialization
* All System SRAM and ITCM/DTCM blocks on S32K358 are protected by **64-bit data + 8-bit ECC**.
* After a Power-On Reset (POR), SRAM contents and ECC syndrome bits are **random**.
* If your C code or stack performs a **read** (or a **sub-64-bit write**, which forces the SRAM controller to do a Read-Modify-Write to compute the 64-bit ECC) on uninitialized SRAM, the hardware triggers an **uncorrectable ECC BusFault / HardFault**.
* **Impact on Milestone 02:** In `Reset_Handler` (before touching the stack or running C code that reads RAM!), you must zero-initialize your SRAM/TCM regions using **64-bit aligned stores** (e.g., `STRD` or `STMIA` with 2+ registers in assembly). Study **AN13388**.

### Gotcha C: Peripheral Clock Gating (`MC_ME`) & Watchdog (`SWT0`)
* Most peripherals (`SIUL2`, `LPUART`, etc.) are **clock-gated off** at reset. Reading or writing registers of a clock-gated peripheral can trigger a BusFault.
* Enabling a peripheral clock via `MC_ME` requires:
  1. Setting the target peripheral's bit in the appropriate `PRTNx_COFBn_CLKEN` register.
  2. Triggering the partition update (`PRTNx_PUPD`).
  3. Writing the **control key sequence** (`0x5AF0` followed by `0xA50F`) to `MC_ME->CTL_KEY`.
  4. Polling `PRTNx_COFBn_STAT` until the hardware confirms the clock is active.
* Also check **SWT0** (Software Watchdog Timer) and any onboard **FS26/FS86 SBC watchdog** — if enabled at reset, an unserviced watchdog will repeatedly reset your board while debugging!

---

## 4. Your Notes & Memory Layout Diagrams

*(Add your verified S32K358 Flash/SRAM partition table and board pinout notes here as you complete Milestones 01–05.)*
