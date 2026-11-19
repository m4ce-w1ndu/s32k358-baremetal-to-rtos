# 06 — CMake Bare-Metal Cross-Compilation Reference

Quick syntax and command reference for writing `cmake/arm-none-eabi.cmake` and `CMakeLists.txt`.

> **Offline Built-in Manual:** CMake ships with its full documentation locally! Run any of these in your terminal:
> * `cmake --help-manual cmake-toolchains` — Official cross-compilation toolchain guide
> * `cmake --help-command <cmd>` — e.g., `cmake --help-command set`, `cmake --help-command target_compile_options`
> * `cmake --help-variable <var>` — e.g., `cmake --help-variable CMAKE_SYSTEM_NAME`

---

## 1. Basic CMake Syntax Rules

* Every statement in CMake is a **command call**: `command_name(arg1 arg2 ...)`
* Comments start with `#`.
* Variables are assigned with `set(VAR_NAME value)` and dereferenced with `${VAR_NAME}`.
* Lists are just space-separated arguments inside `set(...)` or `target_compile_options(...)`.

---

## 2. Key Variables for a Bare-Metal Toolchain File (`cmake/arm-none-eabi.cmake`)

A toolchain file is passed at configure time via `cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake`.

| Variable | Typical Purpose |
| :--- | :--- |
| `CMAKE_SYSTEM_NAME` | Set to `Generic` for bare-metal (no OS). |
| `CMAKE_SYSTEM_PROCESSOR` | Target CPU architecture (`arm`). |
| `CMAKE_TRY_COMPILE_TARGET_TYPE` | Set to `STATIC_LIBRARY` so CMake's compiler sanity check does not try to link a host executable before your linker script is applied. |
| `CMAKE_C_COMPILER` | Cross C compiler (`arm-none-eabi-gcc`). |
| `CMAKE_ASM_COMPILER` | Cross assembler (`arm-none-eabi-gcc` — using `gcc` allows C preprocessor `#define`s in `.S` files). |
| `CMAKE_OBJCOPY` | Binary/hex converter (`arm-none-eabi-objcopy`). |
| `CMAKE_SIZE` | Section size reporter (`arm-none-eabi-size`). |
| `CMAKE_FIND_ROOT_PATH_MODE_PROGRAM` | `NEVER` (run host tools like `make`, `python`). |
| `CMAKE_FIND_ROOT_PATH_MODE_LIBRARY` | `ONLY` (never pick up host x86_64 `.a`/`.so` libraries). |
| `CMAKE_FIND_ROOT_PATH_MODE_INCLUDE` | `ONLY` (never pick up host `/usr/include` headers). |

---

## 3. Key Commands for `CMakeLists.txt`

| Command | Purpose |
| :--- | :--- |
| `cmake_minimum_required(VERSION 3.20)` | Must be the first line of root `CMakeLists.txt`. |
| `project(name LANGUAGES C ASM)` | Declares the project name and enables C + Assembly toolchains. |
| `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)` | Generates `build/compile_commands.json` so Neovim `clangd` LSP knows your exact ARM flags and includes. |
| `set(CMAKE_C_STANDARD 11)` / `set(CMAKE_C_STANDARD_REQUIRED ON)` | Enforces strict C11. |
| `add_executable(target src1.c src2.S)` | Defines the firmware ELF target (e.g., `firmware.elf`). |
| `target_include_directories(target PRIVATE ...)` | Adds header search paths. |
| `target_compile_options(target PRIVATE ...)` | Adds CPU (`-mcpu=cortex-m7`, `-mthumb`), warning, and section flags. |
| `target_link_options(target PRIVATE ...)` | Adds linker script (`-T...`), `-Wl,--gc-sections`, `-Wl,-Map=...`, `-nostdlib`. |
| `set_target_properties(target PROPERTIES LINK_DEPENDS ...)` | Forces re-linking whenever your `.ld` linker script changes! |
| `add_custom_command(TARGET ... POST_BUILD ...)` | Runs `arm-none-eabi-size` and `arm-none-eabi-objcopy` automatically after linking. |
