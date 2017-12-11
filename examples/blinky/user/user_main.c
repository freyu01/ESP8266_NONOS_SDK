#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

// note: #define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text"))), so these code will be put into irom0 section.
/*
Now all the code in .text section will be copied by bootloader to IRAM from where it will be executed. There is 32kb of IRAM 
so it's imposible to fix all the code there. Rest of the code will be executed from flash by ICACHE (istruction cache mechanizm).
This is possible because whole flash is memory mapped (at 0x40200000).
*/

void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}

// Timer used to determine when the LED is to be turned on/off.
LOCAL os_timer_t blink_timer;

// The current state of the LED's output.
LOCAL uint8_t led_state = 0;

/*
 * Call-back for when the blink timer expires. This simply toggles the GPIO 4 state.
 */
LOCAL void ICACHE_FLASH_ATTR blink_cb(void *arg) {
    led_state = !led_state;
    GPIO_OUTPUT_SET(4, led_state);
}

/*
 * Entry point for the program. Sets up the microcontroller for use.
 */
void user_init(void) {
    // Initialise all GPIOs.
    gpio_init();

    // GPIO 4 is an output, start with it low.
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    gpio_output_set(0, BIT4, BIT4, 0);

    // Start a timer for the flashing of the LED on GPIO 4, every 1 second, continuous.
    os_timer_disarm(&blink_timer);
    os_timer_setfn(&blink_timer, (os_timer_func_t *)blink_cb, (void *)0);
    os_timer_arm(&blink_timer, 1000, 1);
}
