/**
 * @file mc_me.h
 * @brief Bare-metal Mode Entry Module (MC_ME) register definitions and clock gating API for NXP S32K358.
 */

#ifndef S32K358_MC_ME_H
#define S32K358_MC_ME_H

#include <stdint.h>

/** @brief S32K358 MC_ME peripheral base address. */
#define MC_ME_BASE 0x402DC000U

/** @brief MC_ME Control Key Register (offset 0x000). Write 0x5AF0 followed by 0xA50F to commit updates. */
#define MC_ME_CTL_KEY (*(volatile uint32_t *)(MC_ME_BASE + 0x000))

/** @brief MC_ME Partition 1 Process Update Register (offset 0x304). */
#define MC_ME_PRTN1_PUPD (*(volatile uint32_t *)(MC_ME_BASE + 0x304))

/** @brief MC_ME Partition 1 COFB0 Clock Status Register (offset 0x310). */
#define MC_ME_PRTN1_COFB0_STAT_KEY (*(volatile uint32_t *)(MC_ME_BASE + 0x310))
/** @brief MC_ME Partition 1 COFB1 Clock Status Register (offset 0x314). */
#define MC_ME_PRTN1_COFB1_STAT_KEY (*(volatile uint32_t *)(MC_ME_BASE + 0x314))
/** @brief MC_ME Partition 1 COFB0 Clock Enable Register (offset 0x330). */
#define MC_ME_PRTN1_COFB0_CKLEN_KEY (*(volatile uint32_t *)(MC_ME_BASE + 0x330))
/** @brief MC_ME Partition 1 COFB1 Clock Enable Register (offset 0x334). */
#define MC_ME_PRTN1_COFB1_CKLEN_KEY (*(volatile uint32_t *)(MC_ME_BASE + 0x334))

/** @brief Partition Clock Update (PCUD) bit mask in PRTN1_PUPD. */
#define MC_ME_PCUD (1U << 0)

/**
 * @brief Enables a peripheral clock in MC_ME Partition 1.
 *
 * Sets the target clock request bit in PRTN1_COFBx_CLKEN, triggers the
 * partition clock update (PCUD), commits via the CTL_KEY sequence (0x5AF0 /
 * 0xA50F), polls until completion, and executes a DSB memory barrier.
 *
 * @param cofb_index Partition 1 COFB register index (0 for COFB0, 1 for COFB1).
 * @param req_bit    Peripheral clock request bit position (0..31).
 */
void mc_me_enable_prtn1_cofb_clock(uint32_t cofb_index, uint32_t req_bit);

#endif // S32K358_MC_ME_H
