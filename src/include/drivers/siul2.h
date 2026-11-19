/**
 * @file siul2.h
 * @brief Bare-metal System Integration Unit Lite 2 (SIUL2) register definitions and GPIO API for NXP S32K358.
 */

#ifndef S32K358_SIUL2_H
#define S32K358_SIUL2_H

#include <stdint.h>

/** @brief S32K358 SIUL2 peripheral base address. */
#define SIUL2_BASE 0x40290000U

/**
 * @brief Multiplexed Signal Configuration Register (MSCR) for pad @p n (offset 0x0240 + 4*n).
 * @param n Pin/pad index.
 */
#define SIUL2_MSCR(n) (*(volatile uint32_t *)(SIUL2_BASE + 0x0240U + (4U * (n))))

/**
 * @brief GPIO Pad Data Output Register (GPDO) for pad @p n (offset 0x1300 + (n ^ 3)).
 * @note Byte order within each 32-bit word is swapped (n ^ 3) due to big-endian SIUL2 legacy layout.
 * @param n Pin/pad index.
 */
#define SIUL2_GPDO(n) (*(volatile uint8_t *)(SIUL2_BASE + 0x1300U + ((n) ^ 3U)))

/** @brief Source Signal Select (SSS) field mask (bits [3:0]); 0 selects GPIO mode. */
#define SIUL2_MSCR_SSS_MASK 0x0FU

/** @brief Output Buffer Enable (OBE) bit mask in MSCR (bit 21). */
#define SIUL2_MSCR_OBE (1U << 21)

/** @brief Input Buffer Enable (IBE) bit mask in MSCR (bit 19). */
#define SIUL2_MSCR_IBE (1U << 19)

/** @brief Virtual board LED1 Red pin index (PTF22). */
#define LED_RED_PIN 182U

/** @brief Virtual board LED1 Green pin index (PTF23). */
#define LED_GREEN_PIN 183U

/** @brief Virtual board LED1 Blue pin index (PTF24). */
#define LED_BLUE_PIN 184U

/**
 * @brief Configures the specified SIUL2 pad as a GPIO output.
 * @param pin Pad/MSCR index.
 */
void siul2_gpio_init_output(uint32_t pin);

/**
 * @brief Toggles the output state of the specified GPIO pad.
 * @param pin Pad/GPDO index.
 */
void siul2_gpio_toggle(uint32_t pin);

/**
 * @brief Sets the output state of the specified GPIO pad.
 * @param pin   Pad/GPDO index.
 * @param state Desired logic level (0 = low, non-zero = high).
 */
void siul2_gpio_write(uint32_t pin, uint8_t state);

#endif  // S32K358_SIUL2_H
