/**
 * @file mc_me.c
 * @brief Bare-metal Mode Entry Module (MC_ME) clock gating driver for NXP S32K358.
 */

#include <drivers/mc_me.h>

/**
 * @brief Enables a peripheral clock in MC_ME Partition 1.
 *
 * @param cofb_index Partition 1 COFB register index (0 or 1).
 * @param req_bit    Peripheral clock request bit position (0..31).
 */
void mc_me_enable_prtn1_cofb_clock(uint32_t cofb_index, uint32_t req_bit) {
    if ((cofb_index > 1U) || (req_bit >= 32U)) return;
    
    // Select correct index and enable clock
    if (cofb_index == 0) {
        MC_ME_PRTN1_COFB0_CKLEN_KEY |= (1U << req_bit);
    } else {
        MC_ME_PRTN1_COFB1_CKLEN_KEY |= (1U << req_bit);
    }

    // Trigger MC_ME PUPD
    MC_ME_PRTN1_PUPD |= MC_ME_PCUD;

    // Commit the update
    MC_ME_CTL_KEY = 0x5AF0U;
    MC_ME_CTL_KEY = 0xA50FU;

    // Poll until clear
    while ((MC_ME_PRTN1_PUPD & MC_ME_PCUD) != 0U);

    __asm__ volatile ("dsb" ::: "memory");
}
