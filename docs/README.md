# Documentation & Public Reference Catalog

This directory holds the reference catalog, hardware architecture notes, memory layout diagrams, and study guides for the **NXP S32K358 (Arm Cortex-M7)** bare-metal-to-FreeRTOS journey.

> **Downloaded Public PDFs:** Directly downloadable public PDFs are stored in [`docs/pdfs/`](pdfs/). Documents gated behind a browser login or interactive download button (such as NXP's `S32K3XXRM` Reference Manual and Arm Developer's PDF portal) are linked below so you can save them into `docs/pdfs/` as well.

---

## Downloaded PDFs (`docs/pdfs/`)

| File | Description |
| :--- | :--- |
| [`S32K3xx_Data_Sheet.pdf`](pdfs/S32K3xx_Data_Sheet.pdf) | Official NXP S32K3xx Family Data Sheet (covers S32K358 memory sizes, clocks, packages, electrical specs) |
| [`UM12541_FRDM-A-S32K358.pdf`](pdfs/UM12541_FRDM-A-S32K358.pdf) | Official NXP FRDM-A-S32K358 Board User Manual (pinouts, FS26 SBC, OpenSDA, LEDs/UART) |
| [`AN14422_S32K3_FS86.pdf`](pdfs/AN14422_S32K3_FS86.pdf) | Official NXP Application Note — FS86 PMIC / Watchdog / FCCU integration for S32K3 |
| [`Mastering-the-FreeRTOS-Real-Time-Kernel.v1.1.0.pdf`](pdfs/Mastering-the-FreeRTOS-Real-Time-Kernel.v1.1.0.pdf) | Official FreeRTOS Kernel Book (v1.1.0) |
| [`aapcs32.pdf`](pdfs/aapcs32.pdf) | Official Arm 32-bit Procedure Call Standard (AAPCS ABI, calling conventions & 8-byte stack alignment) |
| [`gnu-ld-manual.pdf`](pdfs/gnu-ld-manual.pdf) | Official GNU `ld` Linker Script Manual |

---

## Local Study Guides in This Directory

| Document | Focus | Relevant Milestones |
| :--- | :--- | :--- |
| [01-s32k358-architecture-and-memory-map.md](01-s32k358-architecture-and-memory-map.md) | S32K358 core topology, memory map, SBAF/IVT boot, ECC SRAM & peripheral gating | 01, 02, 03, 05 |
| [02-cortex-m7-core-and-barriers.md](02-cortex-m7-core-and-barriers.md) | Cortex-M7 programmer's model, exceptions, NVIC, SysTick, `volatile` & `DMB`/`DSB`/`ISB` | 02, 04, 06 |
| [03-toolchain-linker-and-startup-guide.md](03-toolchain-linker-and-startup-guide.md) | `arm-none-eabi-gcc` flags, ELF sections, VMA/LMA, and C-runtime startup checklist | 01, 02 |
| [04-freertos-cortex-m7-notes.md](04-freertos-cortex-m7-notes.md) | FreeRTOS `ARM_CM7` port requirements, `SVC`/`PendSV`/`SysTick`, and `BASEPRI` priority rules | 06 |
| [05-emulation-and-testing-environment.md](05-emulation-and-testing-environment.md) | Hardware-free emulation options (**Renode** vs. QEMU) and GDB test workflow | All (01–06) |
| [06-cmake-cross-compilation-reference.md](06-cmake-cross-compilation-reference.md) | CMake syntax, bare-metal toolchain variables, and `CMakeLists.txt` commands | 01 |

---

## 1. NXP S32K358 / S32K3xx Silicon & Board Documentation

### Publicly Available (No Login Required)
* **NXP S32K3 Product Page & Documentation Hub**
  * URL: https://www.nxp.com/products/S32K3#documentation
  * Contains the latest Data Sheet, Fact Sheet, Product Brief, and public Application Notes.
* **S32K3xx Data Sheet (`S32K3XXDS`)**
  * Covers S32K358 electrical characteristics, operating frequencies (up to 240 MHz), memory sizes (8 MB PFLASH, 1152 KB SRAM, ITCM/DTCM), package options, and pinouts.
  * Locate via the [NXP S32K3 Documentation Hub](https://www.nxp.com/products/S32K3#documentation).
* **AN13388 — S32K3 Memories Guide**
  * Essential for Milestones 01 & 02: explains Flash, ITCM/DTCM, System SRAM, PRAM controllers, cacheable vs. non-cacheable regions, and **64-bit ECC SRAM initialization** requirements.
* **AN13465 — S32K3xx Secure Boot**
  * Essential for Milestones 01 & 02: explains **SBAF** (Secure Boot Assist Flash), the **IVT** (Image Vector Table) layout, and how HSE-B hands control to Cortex-M7_0 after reset.
* **Evaluation Board Hardware User Manuals & Schematics** *(use the one matching your physical board)*:
  * **S32K3X8EVB-Q289 Hardware User Manual & Quick Start Guide** — jumper settings, LED/button pins, LPUART/OpenSDA routing, and power/PMIC setup for the 289-MAPBGA EVB.
  * **FRDM-A-S32K358 User Manual (`UM12541`)** — board schematics, FS26 SBC, onboard debugger, and peripheral pin assignments for the Freedom board.
  * **AN14422 — FS86 NXP PMIC Solution for S32K3 Processor** — useful if your board uses an NXP FS26/FS86 SBC (which may require watchdog servicing or debug-mode jumpering so the PMIC doesn't reset the MCU).

### Free NXP Account / DocStore Required
* **S32K3xx Reference Manual (`S32K3XXRM`)** *(Primary Silicon Reference)*
  * **Why it is gated:** NXP places the full S32K3xx Reference Manual behind a free NXP.com account / DocStore entitlement.
  * **What to download with it:** Always download the **attached ZIP/Excel spreadsheets** bundled with the RM:
    * `S32K3XX_memory_map.xlsx` — exact base addresses for every peripheral (`MC_ME`, `SIUL2`, `LPUART0..15`, `SWT0`, etc.).
    * `S32K358_IOMUX.xlsx` — exact `MSCR` / `IMCR` indices and `SSS` mux values for every package pin.
    * `S32K3XX_interrupt_map.xlsx` — NVIC IRQ numbers for every peripheral interrupt.

---

## 2. Arm Cortex-M7 & ARMv7-M Architecture Documentation

All core Arm manuals are publicly available on Arm Developer:

* **Arm Cortex-M7 Devices Generic User Guide (`DUI0646`)**
  * URL: https://developer.arm.com/documentation/dui0646/
  * *Best day-to-day core reference:* Programmer's model, core registers, exception entry/return, NVIC, SCB (`VTOR`, `AIRCR`, `CPACR`), SysTick, and MPU.
* **Arm Cortex-M7 Processor Technical Reference Manual (`DDI0489`)**
  * URL: https://developer.arm.com/documentation/ddi0489/
  * *Hardware implementation details:* Pipeline, L1 I-Cache / D-Cache, ITCM / DTCM interfaces, and revision (`r0p1`, `r1p0`, etc.) specifics.
* **ARMv7-M Architecture Reference Manual (`DDI0403`)**
  * URL: https://developer.arm.com/documentation/ddi0403/
  * *Authoritative specification:* Exact architectural semantics of exceptions, memory ordering, bitfields, and Thumb-2 instructions.
* **Arm Application Note 321 — ARM Cortex-M Programming Guide to Memory Barrier Instructions (`DAI0321`)**
  * URL: https://developer.arm.com/documentation/dai0321/
  * *Critical for Cortex-M7:* Explains exactly when `DMB`, `DSB`, and `ISB` are required (NVIC enable/disable, `VTOR` update, clock/cache/MPU changes, `WFI`).
* **Procedure Call Standard for the Arm Architecture (AAPCS)**
  * URL: https://github.com/ARM-software/abi-aa/releases
  * Defines calling conventions (`R0`–`R3` caller-saved, `R4`–`R11` callee-saved) and the **8-byte stack alignment** rule at public interfaces.

---

## 3. GNU Toolchain, Linker & Build System Documentation

* **Arm GNU Toolchain (`arm-none-eabi-gcc`) Downloads & Release Notes**
  * URL: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
* **GCC ARM Options Manual**
  * URL: https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
  * Reference for `-mcpu=cortex-m7`, `-mthumb`, `-mfpu=fpv5-d16`, `-mfloat-abi=hard|softfp|soft`.
* **GCC Extended Inline Assembly (`asm volatile`)**
  * URL: https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
  * Needed for writing clean `DSB`/`ISB`/`CPSID`/`CPSIE` intrinsics without CMSIS.
* **GNU `ld` Linker Script Manual**
  * URL: https://sourceware.org/binutils/docs/ld/Scripts.html
  * Reference for `ENTRY`, `MEMORY`, `SECTIONS`, `AT>`, `KEEP()`, `ALIGN()`, and symbol definitions.
* **GNU Binutils (`objdump`, `objcopy`, `readelf`, `nm`, `size`)**
  * URL: https://sourceware.org/binutils/docs/binutils/
* **CMake Cross-Compiling & Toolchains Documentation**
  * URL: https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html

---

## 4. FreeRTOS Documentation

* **FreeRTOS Official Documentation Hub**
  * URL: https://www.freertos.org/Documentation
* **Mastering the FreeRTOS Real Time Kernel (Official Free Book & PDF)**
  * GitHub & Releases: https://github.com/FreeRTOS/FreeRTOS-Kernel-Book/releases
* **FreeRTOS Kernel Source Repository**
  * URL: https://github.com/FreeRTOS/FreeRTOS-Kernel
  * Relevant port directories: `portable/GCC/ARM_CM7/r0p1/` and `portable/GCC/ARM_CM4F/` (see [04-freertos-cortex-m7-notes.md](04-freertos-cortex-m7-notes.md)).
* **Running FreeRTOS on ARM Cortex-M Cores (Interrupt Priority Guide)**
  * URL: https://www.freertos.org/RTOS-Cortex-M3-M4.html (applies directly to Cortex-M7 NVIC priority configuration).

---

## 5. Debugging & Flashing Tools

* **GDB Manual (`arm-none-eabi-gdb`)**: https://sourceware.org/gdb/current/onlinedocs/gdb/
* **OpenOCD User's Guide**: https://openocd.org/doc/html/index.html
* **SEGGER J-Link Documentation** *(if using J-Link / onboard J-Link)*: https://wiki.segger.com/

---

## Milestone-to-Document Reading Map

| Milestone | Primary References to Open |
| :--- | :--- |
| **01 — Toolchain & Linker Script** | GNU `ld` Scripts manual; CMake Toolchains doc; `S32K3XX_memory_map.xlsx`; `AN13388` (Memories); `AN13465` (IVT) |
| **02 — Startup & C-Runtime** | `DUI0646` Ch. 2 (Programmer's Model & Exception Model); `AN13388` (64-bit ECC SRAM init); AAPCS (8-byte stack alignment) |
| **03 — Clock & GPIO (`MC_ME`, `SIUL2`)** | `S32K3XXRM` (MC_ME, MC_CGM, SIUL2 chapters); `S32K358_IOMUX.xlsx`; Board User Manual (LED pin & active polarity) |
| **04 — SysTick & NVIC** | `DUI0646` Ch. 4 (NVIC, SCB, SysTick); `DAI0321` (Memory Barriers); `S32K3XX_interrupt_map.xlsx` |
| **05 — UART Driver (`LPUART`)** | `S32K3XXRM` (LPUART, MC_ME, MC_CGM, SIUL2 chapters); Board User Manual (UART TX/RX pins & USB-UART bridge) |
| **06 — FreeRTOS Port** | *Mastering the FreeRTOS Real Time Kernel*; `DUI0646` (`CPUID`, `BASEPRI`, `SVC`, `PendSV`, `SysTick`); FreeRTOS Cortex-M interrupt priority guide |
