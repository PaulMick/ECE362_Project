#include "pico/stdlib.h"
#include "hardware/resets.h"
#include "hardware/adc.h"
#include "inputs.h"

#define PADS_ISO      (1u << 8)
#define PADS_IE       (1u << 6)
#define PADS_PUE      (1u << 3)
#define PADS_PDE      (1u << 2)

//input state stored here
static bool btn_prev = false;
static bool btn_edge = false;
static uint16_t pot_val = 0;

int init_inputs(void) {
    uint32_t mask = 1u << (START_BUTTON_PIN & 0x1fu); // select start btn (44 defined in header)
    sio_hw->gpio_oe_clr = mask; // set input mode
    io_bank0_hw->io[START_BUTTON_PIN].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB; // select SIO func
    hw_clear_bits(&pads_bank0_hw->io[START_BUTTON_PIN], PADS_BANK0_GPIO0_ISO_BITS); // remove pad isolation latch
    hw_set_bits(&pads_bank0_hw->io[START_BUTTON_PIN], PADS_BANK0_GPIO0_IE_BITS); // input buffer on
    hw_set_bits(&pads_bank0_hw->io[START_BUTTON_PIN], PADS_BANK0_GPIO0_PUE_BITS); // pull up 1, pull down 0
    hw_clear_bits(&pads_bank0_hw->io[START_BUTTON_PIN], PADS_BANK0_GPIO0_PDE_BITS);

    // pot on gpio45, adc channel 5
    reset_unreset_block_num_wait_blocking(RESET_ADC);
    io_bank0_hw->io[POT_PIN].ctrl = GPIO_FUNC_NULL << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[POT_PIN] &= ~(PADS_ISO | PADS_IE | PADS_PUE | PADS_PDE);
    adc_hw->cs = (1u << 0); //enable adc
    while (!(adc_hw->cs & (1u << 8))) { // wait for ready
        tight_loop_contents();
    }
    adc_hw->cs = (adc_hw->cs & ~(0xFu << 12)) | (POT_ADC_CHAN << 12) | ADC_CS_EN_LSB; // select channel 5

    return 0;
}

static uint16_t read_adc(void) {
    adc_hw->cs |= (1u << 2); // single shot conversion
    while (!(adc_hw->cs & (1u << 8))) { // wait for ready
        tight_loop_contents();
    }
    return (uint16_t)(adc_hw->result & 0x0FFFu);
}

int update_inputs(void) {
    // detect button posedge
    uint32_t input_pins = sio_hw->gpio_in;
    bool btn_curr = (input_pins & (1u << (START_BUTTON_PIN & 0x1fu))) == 0;
    btn_edge = !btn_prev && btn_curr;
    btn_prev = btn_curr;

    // read potentiometer
    pot_val = read_adc();

    return 0;
}

bool input_button_pressed(void) { 
    return btn_edge; 
}

uint16_t input_pot_raw(void) { 
    return pot_val; 
}
