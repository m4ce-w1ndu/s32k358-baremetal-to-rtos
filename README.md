# s32k358-baremetal-to-rtos

A progressive, from-scratch embedded firmware reference project targeting the **NXP S32K358** microcontroller (**Arm Cortex-M7**). 

This repository documents the journey of building a complete firmware stack without relying on vendor HALs or heavy SDKs (such as NXP RTD), moving step-by-step from raw silicon initialization to a multitasking **FreeRTOS** environment.

## 🎯 Project Goals
* Understand the low-level boot process of an Arm Cortex-M7 core.
* Write custom linker scripts and C-runtime startup code from scratch.
* Develop clean, register-level peripheral drivers using the NXP S32K3xx Reference Manual.
* Port and integrate the FreeRTOS kernel on top of custom hardware drivers.

## 🗺️ Roadmap & Milestones
* [ ] **01 - Toolchain & Linker Script:** Minimal CMake + `arm-none-eabi-gcc` setup and Flash/SRAM memory mapping.
* [ ] **02 - Startup & C-Runtime:** Vector table definition, stack initialization, `.data` copy, and `.bss` zeroing.
* [ ] **03 - Clock & GPIO (SIUL2):** Peripheral clock gating (`MC_ME`) and register-level LED blinky driver.
* [ ] **04 - SysTick & NVIC:** Core system timer configuration and non-blocking millisecond delays.
* [ ] **05 - UART Driver (LPUART):** Serial communication driver for lightweight `printf` debugging.
* [ ] **06 - FreeRTOS Port:** Kernel integration (`GCC/ARM_CM7`), interrupt mapping (`SVC`, `PendSV`, `SysTick`), and concurrent driver tasks.

## 🛠️ Hardware & Tools
* **Target MCU:** NXP S32K358 (Arm Cortex-M7)
* **Compiler:** `arm-none-eabi-gcc`
* **Build System:** CMake / Make
* **AI Workflow:** Developed with [Antigravity CLI](https://antigravity.google) using custom project rules (`AGENTS.md`).

## 📂 Repository Structure
```text
├── cmake/          # Toolchain files for cross-compilation
├── linker/         # Custom .ld linker scripts
├── src/
│   ├── startup/    # Vector table and Reset_Handler
│   ├── drivers/    # Bare-metal peripheral drivers (SIUL2, LPUART, etc.)
│   └── main.c      # Application entry point
├── docs/           # Register notes and memory layout diagrams
├── AGENTS.md       # Guidelines and constraints for Antigravity CLI
└── CMakeLists.txt
```

## 📜 License
MIT License - Feel free to use this repository as a learning reference.
