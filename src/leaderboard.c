#include <stdio.h>
#include <stdlib.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "leaderboard.h"

#define MASTER_SDA 6
#define MASTER_SCL 7
#define I2C_BAUDRATE 100000

#define EEPROM_ADDR 0x50
#define LEADERBOARD_MEM_BASE 0x0
#define LEADERBOARD_SPOTS 10

void init_leaderboard() {
    gpio_init(MASTER_SDA);
    gpio_set_function(MASTER_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(MASTER_SDA);
    gpio_init(MASTER_SCL);
    gpio_set_function(MASTER_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(MASTER_SCL);

    i2c_init(i2c1, I2C_BAUDRATE);
}

void eeprom_write(uint16_t write_addr, uint8_t *read_addr, int len_bytes) {
    uint8_t msg[len_bytes + 2];
    msg[0] = (uint8_t)(write_addr >> 8);
    msg[1] = (uint8_t)(write_addr & 0xFF);
    for (int i = 0; i < len_bytes; i++) {
        msg[i + 2] = read_addr[i];
    }
    i2c_write_blocking(i2c1, EEPROM_ADDR, msg, len_bytes + 2, false);
    sleep_ms(10);
}

void eeprom_read(uint16_t read_addr, uint8_t *write_addr, int len_bytes) {
    uint8_t reg_addr[2] = {(uint8_t)(read_addr >> 8), (uint8_t)(read_addr & 0xFF)};
    i2c_write_blocking(i2c1, EEPROM_ADDR, reg_addr, 2, true);
    i2c_read_blocking(i2c1, EEPROM_ADDR, write_addr, len_bytes, false);
}

int reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void scan_i2c() {
    printf("Start scan\n");
    for (int addr = 0; addr < (1 << 7); addr ++) {
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr)) {
            ret = PICO_ERROR_GENERIC;
        } else {
            ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);
        }
        
        if (ret >= 0) {
            printf("0x%x ", addr);
        }
    }
    printf("\nEnd scan\n");
}