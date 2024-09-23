#include <platform.h>
#include <syscall.h>
#include <print.h>

port out p_leds = PORT_TP_20_21_22_23;

int main() {
    if (!_is_simulation()) {
        // If running on hardware, flash the LEDs
        while (1) {
            p_leds <: 0x0;
            printstrln("LED OFF");
            delay_milliseconds(250);
            p_leds <: 0xf;
            printstrln("LED ON");
            delay_milliseconds(250);
        }
    }

    return 0;
}
