# S32K358 Hardware-Free Simulation Environment (Renode)

This directory contains the **Renode** virtual hardware setup for running, debugging, and verifying your bare-metal S32K358 firmware without a physical board.

## Prerequisites
* **Renode v1.17.0** (installed at `~/.local/opt/renode`, symlinked to `~/.local/bin/renode`)
* **ARM GDB (`gdb-multiarch`)** for interactive stepping:
  ```bash
  sudo apt update && sudo apt install -y gdb-multiarch
  ```

## Files
* `s32k358.repl` — Virtual S32K358 platform (Cortex-M7 `cpu0`, `nvic0` @ 240 MHz, 8 MB PFLASH @ `0x00400000`, SRAM @ `0x20400000`, `ITCM`/`DTCM`, `MC_ME`, `MC_CGM`, `SIUL2` + virtual RGB LEDs, `LPUART0..15`, `SWT0`, `STM0`, `MSCM`).
* `run.resc` — Loads `build/firmware.elf`, enables LED/LPUART0 output, and starts execution.
* `debug.resc` — Loads `build/firmware.elf` and starts a GDB server on port `3333` (halted at reset).

## Running & Debugging

### 1. Run Firmware Headless (WSL / Terminal-Friendly)
```bash
renode --console --disable-xwt sim/run.resc
```
To override the ELF path:
```bash
renode --console --disable-xwt -e '$elf=@build/your_target.elf' sim/run.resc
```

### 2. Step-Debug with GDB
Terminal 1 (start Renode GDB server):
```bash
renode --console --disable-xwt sim/debug.resc
```
Terminal 2 (attach GDB):
```bash
gdb-multiarch build/firmware.elf -ex "target remote :3333"
```
Useful GDB commands during Milestones 01–06:
* `info reg` — inspect `r0`–`r12`, `sp`, `lr`, `pc`, `xpsr`
* `si` / `ni` — step one assembly instruction (ideal for `Reset_Handler`)
* `x/16wx 0x00400000` — inspect PFLASH / vector table
* `x/16wx 0x20400000` — inspect SRAM (`.data` / `.bss`)
* `monitor sysbus ReadDoubleWord 0x402DC000` — inspect peripherals directly via Renode monitor

## Virtual Board Pinout (for Milestone 03 & 05)
* **LED1 (RGB) on `SIUL2`:** Red = `PTF22` (`MSCR[182]`), Green = `PTF23` (`MSCR[183]`), Blue = `PTF24` (`MSCR[184]`)
* **LED2 (RGB) on `SIUL2`:** Red = `PTF17` (`MSCR[177]`), Green = `PTF18` (`MSCR[178]`), Blue = `PTF19` (`MSCR[179]`)
* **LPUART0:** Base address `0x40328000` (IRQ routed via `MSCM` @ 141)
