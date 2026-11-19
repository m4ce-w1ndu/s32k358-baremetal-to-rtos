#include <stdint.h>

// g_data_test - verifies proper data copy from FLASH to SRAM
volatile uint32_t g_data_test = 0xCAFEBABEU;

// g_bss_test - verifies BSS zeroing on startup
volatile uint32_t g_bss_test = 0;

int main(void) {
    volatile uint32_t counter = 0;
    (void)(g_data_test == 0xCAFEBABEU);

    for (;;) {
        counter++;
        g_bss_test++;
    }
}
