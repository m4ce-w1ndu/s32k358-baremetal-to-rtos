# 03 — Bare-Metal Toolchain, Linker Script & Startup Study Guide

Conceptual checklist for **Milestone 01 (Toolchain & Linker Script)** and **Milestone 02 (Startup & C-Runtime)**.
References: [GNU `ld` Scripts Manual](https://sourceware.org/binutils/docs/ld/Scripts.html), [GCC ARM Options](https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html), and [CMake Toolchains](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html).

---

## 1. Cross-Compilation Pipeline (`arm-none-eabi-*`)

1. **Compile (`.c` / `.S` -> `.o`):** `arm-none-eabi-gcc` translates C11 and assembly into relocatable ARM Thumb-2 object files.
2. **Link (`.o` + `.ld` -> `.elf`):** `arm-none-eabi-gcc` invokes GNU `ld` with your custom linker script to place sections into S32K358 Flash and SRAM addresses and resolve symbols.
3. **Inspect & Convert:**
   * `arm-none-eabi-size` — check `.text`, `.data`, and `.bss` sizes.
   * `arm-none-eabi-objdump -d -S` — inspect disassembly and verify vector table / alignment.
   * `arm-none-eabi-readelf -S -l` — verify ELF section headers, VMA/LMA, and program headers.
   * `arm-none-eabi-objcopy` — generate `.bin` or `.hex` images for flashing.

### Key Compiler & Linker Flags to Understand
* **Architecture:** `-mcpu=cortex-m7`, `-mthumb`
* **Floating Point:** Start with `-mfloat-abi=soft` (simplest before FPU `CPACR` init) or enable FPU early and use `-mfpu=fpv5-d16 -mfloat-abi=hard` (keep flags identical across compile and link!).
* **Bare-Metal Isolation:** `-ffreestanding`, `-nostdlib` (or `--specs=nosys.specs` / `--specs=nano.specs` if selectively linking `libgcc` for 64-bit division helpers).
* **Dead-Code Elimination:** `-ffunction-sections -fdata-sections` (compiler) paired with `-Wl,--gc-sections` (linker). Remember to wrap your IVT and vector table in `KEEP(*(...))` in the linker script so `--gc-sections` does not discard them!
* **Warnings & Standard:** `-std=c11 -Wall -Wextra -Werror -Wpedantic`
* **Map File:** `-Wl,-Map=firmware.map` — indispensable for verifying where every section and symbol landed.

---

## 2. Linker Script Anatomy (`linker/*.ld`)

A bare-metal linker script answers four questions:

1. **Entry Point (`ENTRY(...)`):** Names the reset entry symbol (e.g., `Reset_Handler`) for the debugger and ELF header.
2. **Physical Memory (`MEMORY`):** Declares named regions (`FLASH`, `SRAM`, `ITCM`, `DTCM`), their access attributes (`rx`, `rwx`), `ORIGIN`, and `LENGTH` matching S32K358.
3. **Output Sections (`SECTIONS`):** Maps input sections (`.ivt`, `.isr_vector`, `.text*`, `.rodata*`, `.data*`, `.bss*`) into memory regions:
   * **VMA (Virtual Memory Address):** Where the CPU accesses the section at runtime (e.g., `.data` lives in SRAM).
   * **LMA (Load Memory Address):** Where the section is stored in the flash image (e.g., `.data` initial values are stored in PFLASH using `> SRAM AT > FLASH`).
4. **Boundary Symbols & Alignment:**
   * Export symbols (`_sidata` via `LOADADDR(.data)`, `_sdata`, `_edata`, `_sbss`, `_ebss`, `_estack`, and SRAM start/end for ECC init) so startup code knows exact bounds.
   * Enforce `ALIGN(4)` on section boundaries (for 32-bit word copy loops) and **`ALIGN(8)`** on the top of stack (`_estack`, required by AAPCS) and ECC init boundaries (8-byte / 64-bit aligned!).

---

## 3. Startup & C-Runtime (`Reset_Handler`) Checklist

Before `main()` can legally execute C11 code on an S32K358, `Reset_Handler` must perform these steps in order:

1. **Mask interrupts** (`CPSID I`) while initializing core state.
2. **Initialize ECC SRAM (and TCM if used):** Write 64-bit aligned zeros across the SRAM region(s) using register-only assembly (do **not** push to the stack or read RAM before this completes!).
3. **Set/Verify Stack Pointer (`MSP`)** to `_estack` (8-byte aligned).
4. **Copy `.data` from Flash (LMA `_sidata`) to SRAM (VMA `_sdata`..`_edata`):** Watch out for out-of-bounds off-by-one bugs (`< _edata`, not `<= _edata`).
5. **Zero `.bss` (`_sbss`..`_ebss`):** (May already be zeroed by step 2, but keeping an explicit `.bss` loop ensures correctness across warm resets where full SRAM ECC init might be skipped.)
6. **Set `SCB->VTOR`** to the address of your vector table, followed by `DSB` + `ISB`.
7. *(Optional/Later)* **Enable FPU** in `SCB->CPACR` (`CP10`/`CP11` full access) + `DSB` + `ISB` if compiling with hardware floating point.
8. **Branch to `main()`**, and catch any accidental return from `main()` in an infinite loop.
