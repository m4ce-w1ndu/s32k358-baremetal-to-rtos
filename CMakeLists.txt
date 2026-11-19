# Specify generic CMake requirements
cmake_minimum_required(VERSION 3.20)

# Set language and project name
project(s32k358_baremetal LANGUAGES C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup target compilation options
set(CPU_FLAGS -mcpu=cortex-m7 -mthumb -mfloat-abi=soft)
set(BARE_METAL_FLAGS -ffreestanding -ffunction-sections -fdata-sections)
set(DEBUG_FLAGS -g3 -O0)
set(STRICT_FLAGS -Wall -Wextra -Werror -Wpedantic)
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker/s32k358.ld")

# Target definition
set(FW_ELF "firmware.elf")
add_executable(
	${FW_ELF}
	"src/main.c"
    "src/startup/startup_s32k358.S"
)

# Include directories
target_include_directories(
	${FW_ELF} PRIVATE
	"src/include"
)

# Compile options
target_compile_options(
	${FW_ELF} PRIVATE
	${CPU_FLAGS}
    ${BARE_METAL_FLAGS}
	${DEBUG_FLAGS}
    "$<$<COMPILE_LANGUAGE:C>:${STRICT_FLAGS}>"
)

# Link options
target_link_options(
	${FW_ELF} PRIVATE
    ${CPU_FLAGS}
	-T${LINKER_SCRIPT}
	-nostdlib
	-Wl,--gc-sections
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/firmware.map
)

# Auto-relink
set_target_properties(${FW_ELF} PROPERTIES
	LINK_DEPENDS ${LINKER_SCRIPT})

# Post build commands
add_custom_command(TARGET ${FW_ELF} POST_BUILD 
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${FW_ELF}>
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${FW_ELF}>
    ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin
)
