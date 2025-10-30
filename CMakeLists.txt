cmake_minimum_required(VERSION 3.5...4.2)

project(stm32_template C CXX)

set(TARGET_NAME ${PROJECT_NAME})

# Путь к ARM Toolchain (если arm-none-eabi не в PATH)
set(ARM_TOOLCHAIN_PATH $ENV{ARM_TOOLCHAIN_PATH})
set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-g++)
set(OBJCOPY ${ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy)
set(STFLASH st-flash)

# Папки для сборки
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Флаги компилятора
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=c99 -fdata-sections -ffunction-sections -mlittle-endian -g3 -fno-move-loop-invariants -fsigned-char -ffreestanding")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

# Флаги линкера
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -nostartfiles -Xlinker -Map=${TARGET_NAME}.map -Xlinker -T${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld -Wl,--gc-sections")

# Определения для препроцессора
add_definitions(-DSTM32F103C8 -DSTM32F10X_MD)

# Инклуды
include_directories(cmsis stm32)

# Исходники
set(SOURCE_FILES
    stm32/startup/startup_stm32f10x_md.c
    stm32/system_stm32f10x.c
    syscalls/syscalls.c
    main.c
)

# Копирование .gdbinit
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# Основной ELF таргет
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# BIN и HEX файлы
set(BIN_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.bin")
set(HEX_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.hex")
set(ELF_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

add_custom_command(OUTPUT ${BIN_TARGET}
    COMMAND ${OBJCOPY} -O binary ${ELF_TARGET} ${BIN_TARGET}
    DEPENDS ${ELF_TARGET} ${PROJECT_NAME})

add_custom_target(bin DEPENDS ${BIN_TARGET})

add_custom_command(OUTPUT ${HEX_TARGET}
    COMMAND ${OBJCOPY} -O ihex ${ELF_TARGET} ${HEX_TARGET}
    DEPENDS ${ELF_TARGET} ${PROJECT_NAME})

add_custom_target(hex DEPENDS ${HEX_TARGET})

# Прошивка через st-flash
add_custom_command(OUTPUT flash_target
    COMMAND ${STFLASH} write ${BIN_TARGET} 0x8000000
    DEPENDS ${BIN_TARGET})

add_custom_target(flash DEPENDS flash_target)
