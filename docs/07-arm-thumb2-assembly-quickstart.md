# 07 — GNU ARM Thumb-2 Assembly Quickstart (Cortex-M7)

A concise, 1-page reference for writing bare-metal Cortex-M7 startup assembly with the GNU Assembler (`arm-none-eabi-gcc` / `as`).

> **Full Official References:**
> * **Arm Cortex-M7 Instruction Set:** `DUI0646` Chapter 3 — https://developer.arm.com/documentation/dui0646/latest
> * **GNU Assembler (`as`) Manual:** https://sourceware.org/binutils/docs/as/

---

## 1. File Header Directives

Place these at the top of any `.S` Cortex-M7 assembly file:

| Directive | Meaning |
| :--- | :--- |
| `.syntax unified` | Uses modern Unified Assembly Language (UAL) syntax for Thumb-2. |
| `.cpu cortex-m7` | Targets Arm Cortex-M7 instructions. |
| `.thumb` | Generates Thumb / Thumb-2 instructions (Cortex-M only executes Thumb!). |

Comments in GNU ARM assembly use `/* ... */` or `// ...` (since `.S` is preprocessed by `gcc`) or `@ ...`.

---

## 2. Sections, Symbols & Labels

| Directive | Meaning |
| :--- | :--- |
| `.section .name, "flags", %type` | Switches output section: `"a"` = allocatable (data/vectors), `"ax"` = allocatable + executable (code). `%progbits` = contains bytes in Flash. |
| `.global SymName` | Exports `SymName` to the linker (like non-`static` in C). |
| `.type SymName, %function` | Marks a symbol as a function (helps GDB and ensures Thumb bit 0 is set!). |
| `.thumb_func` | Tells the assembler the next label is a Thumb function (sets bit 0 in vector table entries!). |
| `.word expr` | Emits a 32-bit value (e.g., an address or `0`). |
| `.weak SymName` | Marks `SymName` as overridable by any strong C definition with the same name. |
| `.thumb_set Alias, Target` | Makes `Alias` point to `Target` (preserving Thumb function type). |
| `label:` | Defines an address label (local labels inside a function often start with `1:` or `.L_loop:`). |

---

## 3. Essential Instructions for Bare-Metal Startup

ARM is a **load/store** architecture: you load addresses/values into registers (`r0`–`r12`), operate on registers, and store back to memory.

### Loading Constants & Addresses
| Instruction | What It Does |
| :--- | :--- |
| `mov r2, #0` | Sets `r2 = 0` (immediate `#` prefix for small constants). |
| `ldr r0, =_sdata` | **Pseudo-instruction:** loads the 32-bit **address** of linker symbol `_sdata` (or any 32-bit constant like `=0xE000ED08`) into `r0`. |

### Memory Load & Store (with Post-Increment!)
| Instruction | What It Does |
| :--- | :--- |
| `ldr r2, [r0], #4` | Reads 32-bit word from `[r0]` into `r2`, **then** adds `4` to `r0` (`r2 = *r0++;`). |
| `str r2, [r1], #4` | Writes 32-bit word `r2` to `[r1]`, **then** adds `4` to `r1` (`*r1++ = r2;`). |
| `str r1, [r0]` | Writes 32-bit word `r1` to memory address held in `r0` (`*r0 = r1;`). |
| `strd r2, r3, [r0], #8` | Writes **64 bits** (`r2` and `r3` together as a single 64-bit store) to `[r0]`, **then** adds `8` to `r0`. Required for **64-bit ECC SRAM initialization**! |

### Comparisons, Branches & Loops
| Instruction | What It Does |
| :--- | :--- |
| `cmp r0, r1` | Computes `r0 - r1` and updates status flags (without modifying `r0` or `r1`). |
| `blo label` | **B**ranch if **Lo**wer (unsigned `r0 < r1`). |
| `bhs label` | **B**ranch if **H**igher or **S**ame (unsigned `r0 >= r1`). |
| `b label` | Unconditional jump to `label` (`b .` = infinite loop at current address). |
| `bl func` | **B**ranch with **L**ink (calls a C/ASM function, saving return address in `lr`). |

#### Conceptual `while (ptr < end)` Loop Pattern
```asm
.L_loop:
    cmp r0, r1        /* compare current ptr (r0) with end (r1) */
    bhs .L_done       /* if r0 >= r1, exit loop immediately */
    /* ... body: str / ldr ... */
    b .L_loop         /* repeat */
.L_done:
```

### Special Cortex-M7 System Instructions
| Instruction | What It Does |
| :--- | :--- |
| `cpsid i` | Disables configurable interrupts (`PRIMASK = 1`). |
| `cpsie i` | Enables configurable interrupts (`PRIMASK = 0`). |
| `msr msp, r0` | **M**ove to **S**pecial **R**egister: sets Main Stack Pointer `MSP = r0`. |
| `dsb` | **Data Synchronization Barrier** (wait for all memory writes to complete). |
| `isb` | **Instruction Synchronization Barrier** (flush CPU pipeline). |
