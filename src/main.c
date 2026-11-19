#include <drivers/mc_me.h>
#include <drivers/siul2.h>

int main(void) {
    uint8_t led_state = 0;

    mc_me_enable_prtn1_cofb_clock(1U, 0U);
    siul2_gpio_init_output(LED_RED_PIN);

    for (;;) {
        siul2_gpio_write(LED_RED_PIN, led_state);
        led_state = !led_state;

        for (volatile uint32_t i = 0; i < 1000000U; i++);
    }
}
