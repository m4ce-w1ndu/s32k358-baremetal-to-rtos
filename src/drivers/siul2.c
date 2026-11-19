/**
 * @file siul2.c
 * @brief Bare-metal System Integration Unit Lite 2 (SIUL2) GPIO driver for NXP S32K358.
 */

#include <drivers/siul2.h>

/**
 * @brief Configures the specified SIUL2 pad as a GPIO output.
 * @param pin Pad/MSCR index.
 */
void siul2_gpio_init_output(uint32_t pin) {
    uint32_t mscr = SIUL2_MSCR(pin);
    mscr &= ~SIUL2_MSCR_SSS_MASK;
    mscr |= SIUL2_MSCR_OBE;
    SIUL2_MSCR(pin) = mscr;
}

/**
 * @brief Sets the output state of the specified GPIO pad.
 * @param pin   Pad/GPDO index.
 * @param state Desired logic level (0 = low, non-zero = high).
 */
void siul2_gpio_write(uint32_t pin, uint8_t state) {
    SIUL2_GPDO(pin) = state != 0U ? 1U : 0U;
}

/**
 * @brief Toggles the output state of the specified GPIO pad.
 * @param pin Pad/GPDO index.
 */
void siul2_gpio_toggle(uint32_t pin) {
    SIUL2_GPDO(pin) ^= 1U;
}
