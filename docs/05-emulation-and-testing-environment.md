# 05 — Hardware-Free Emulation & Testing Environment

Because you do not have a physical S32K358 evaluation board yet, you can build, run, debug (with GDB), and test your bare-metal firmware and FreeRTOS port entirely in software simulation.

---

## 1. Emulator Options Comparison

| Emulator | Cortex-M7 Core | S32K3xx Memory Map (`0x0040_0000` / `0x2040_0000`) | S32K3xx Peripherals (`MC_ME`, `SIUL2`, `NXP_LPUART`) | GDB Debugging & Automated Tests | Best For |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Renode** *(Recommended)* | Yes (`CPU.CortexM`) | Yes (exact custom `.repl`) | **Yes** — ships with `nxp-s32k388.repl`, `NXP_LPUART`, `SIUL2`, `MC_ME`, `MSCM`, `STM` | Built-in GDB server (`:3333`) + UART console + Robot Framework | **All Milestones (01–06)** — closest match to real S32K358 |
| **QEMU** (`qemu-system-arm`) | Yes (`-cpu cortex-m7`, e.g., `mps2-an500`) | No (uses Arm MPS2/MPS3 map) | No (uses CMSDK UART/GPIO, not NXP S32K3xx) | Built-in GDB server (`-s -S`) | Generic Cortex-M7 core only (01, 02, 04, 06) |
| **Arm FVP** (Fixed Virtual Platforms) | Yes | Generic | No NXP S32K3xx peripherals | Commercial / proprietary | Core ISA verification only |

---

## 2. Recommended Environment: Renode + `gdb-multiarch`

**Renode** (open-source, by Antmicro) already includes an official NXP S32K3 platform (`platforms/cpus/nxp-s32k388.repl`). Because the **S32K388** and **S32K358** share the same S32K3xx core and peripheral IP (`CM7_0`, `NVIC`, `SysTick`, `MC_ME`, `SIUL2`, `LPUART`, PFLASH at `0x0040_0000`, SRAM at `0x2040_0000`), we can derive a dedicated `sim/s32k358.repl` directly from Renode's S32K3 platform.

### What You Can Test Without Hardware in Renode
* **Milestone 01 & 02 (Linker Script & Startup):** Load your `.elf`, halt at `Reset_Handler` via GDB, single-step your 64-bit SRAM init, `.data` copy, and `.bss` zeroing, and verify memory contents (`x/16wx 0x20400000`) and stack pointer alignment (`info reg sp`).
* **Milestone 03 (Clock & GPIO / `SIUL2`):** Write to `MC_ME` and `SIUL2` registers and observe simulated LED state changes in the Renode log/monitor.
* **Milestone 04 (SysTick & NVIC):** Verify `SysTick_Handler` fires periodically and test interrupt priorities and `BASEPRI`/`PRIMASK` masking.
* **Milestone 05 (`LPUART` Driver):** Open a virtual UART analyzer (`showAnalyzer lpuart0`) and see your `printf` / serial output live in the terminal.
* **Milestone 06 (FreeRTOS Port):** Run multiple concurrent FreeRTOS tasks, verify `SVC`/`PendSV`/`SysTick` context switches, and inspect task stacks in GDB.

### Real-Silicon Differences to Keep in Mind (Important for Your Future Work!)
Even when testing in Renode, we will still write **real-silicon-accurate** firmware:
1. **SBAF / IVT:** Renode loads your `.elf` directly onto `CM7_0` without running HSE-B SBAF. We will still structure your linker script with the real S32K358 Flash layout (and document/place the IVT cleanly) so your mental model matches real silicon.
2. **64-bit SRAM ECC:** Renode does not fault on uninitialized SRAM reads, whereas real S32K358 silicon **will** BusFault. We will still implement and verify 64-bit aligned SRAM initialization in Milestone 02.
3. **PMIC / Watchdog (`SWT0` / FS26):** Real EVBs may reset the MCU if `SWT0` or the external SBC watchdog is not serviced or put in debug mode.

---

## 3. Installing the Environment on Ubuntu 26.04

1. **Install GDB for ARM:**
   ```bash
   sudo apt update && sudo apt install -y gdb-multiarch
   ```
2. **Install Renode (portable Linux package from official releases):**
   Download the latest Linux portable tarball (`renode-*-linux-portable.tar.gz`) from https://github.com/renode/renode/releases (or install the `.deb` package).
3. **Optional (QEMU fallback for generic Cortex-M7 tests):**
   ```bash
   sudo apt install -y qemu-system-arm
   ```
