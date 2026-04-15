#include <stdio.h>
#include <stdlib.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define MASTER_SDA 6
#define MASTER_SCL 7
#define I2C_BAUDRATE 100000

void init_leaderboard() {
    gpio_init(MASTER_SDA);
    gpio_set_function(MASTER_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(MASTER_SDA);
    gpio_init(MASTER_SCL);
    gpio_set_function(MASTER_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(MASTER_SCL);

    i2c_init(i2c1, I2C_BAUDRATE);

    
}

void eeprom_write(uint16_t eeprom_addr, void *read_addr, uint32_t len_bytes) {
    
}