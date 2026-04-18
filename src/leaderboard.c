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
#define LEADERBOARD_SLOTS 10
#define LEADERBOARD_SLOT_BYTES 8 // 4 chars for initial and a 32bit int for score

leaderboard_slot_t leaderboard[LEADERBOARD_SLOTS];

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
    while (len_bytes > 32) {
        uint8_t msg[32 + 2];
        msg[0] = (uint8_t) (write_addr >> 8);
        msg[1] = (uint8_t) (write_addr & 0xFF);
        for (int i = 0; i < 32; i++) {
            msg[i + 2] = read_addr[i];
        }
        i2c_write_blocking(i2c1, EEPROM_ADDR, msg, 32 + 2, false);
        len_bytes -= 32;
        write_addr += 32;
        read_addr = &read_addr[32];
        sleep_ms(10);
    }
    uint8_t msg_end[len_bytes + 2];
    msg_end[0] = (uint8_t) (write_addr >> 8);
    msg_end[1] = (uint8_t) (write_addr & 0xFF);
    for (int i = 0; i < len_bytes; i++) {
        msg_end[i + 2] = read_addr[i];
    }
    i2c_write_blocking(i2c1, EEPROM_ADDR, msg_end, len_bytes + 2, false);
    sleep_ms(10);
}

void eeprom_read(uint16_t read_addr, uint8_t *write_addr, int len_bytes) {
    uint8_t reg_addr[2] = {(uint8_t) (read_addr >> 8), (uint8_t) (read_addr & 0xff)};
    i2c_write_blocking(i2c1, EEPROM_ADDR, reg_addr, 2, true);
    i2c_read_blocking(i2c1, EEPROM_ADDR, write_addr, len_bytes, false);
}

void add_score_leaderboard(char *initials, uint32_t score) {
    for (int i = 0; i < LEADERBOARD_SLOTS; i ++) {
        if (score > leaderboard[i].score) {
            leaderboard_slot_t tmp = leaderboard[i];
            leaderboard[i] = (leaderboard_slot_t) {.initials = initials, .score = score};
            initials = tmp.initials;
            score = tmp.score;
        }
    }
}

void save_leaderboard() {
    uint8_t buf[LEADERBOARD_SLOTS * LEADERBOARD_SLOT_BYTES];
    for (int i = 0; i < LEADERBOARD_SLOTS; i ++) {
        buf[i * LEADERBOARD_SLOT_BYTES] = leaderboard[i].initials[0];
        buf[i * LEADERBOARD_SLOT_BYTES + 1] = leaderboard[i].initials[1];
        buf[i * LEADERBOARD_SLOT_BYTES + 2] = leaderboard[i].initials[2];
        buf[i * LEADERBOARD_SLOT_BYTES + 3] = leaderboard[i].initials[3];
        buf[i * LEADERBOARD_SLOT_BYTES + 4] = (uint8_t) leaderboard[i].score & 0xff;
        buf[i * LEADERBOARD_SLOT_BYTES + 5] = (uint8_t) (leaderboard[i].score >> 8) & 0xff;
        buf[i * LEADERBOARD_SLOT_BYTES + 6] = (uint8_t) (leaderboard[i].score >> 16) & 0xff;
        buf[i * LEADERBOARD_SLOT_BYTES + 7] = (uint8_t) (leaderboard[i].score >> 24);
    }
    eeprom_write(LEADERBOARD_MEM_BASE, buf, LEADERBOARD_SLOTS * LEADERBOARD_SLOT_BYTES);
}

leaderboard_slot_t *get_leaderboard() {
    uint8_t buf[LEADERBOARD_SLOTS * LEADERBOARD_SLOT_BYTES];
    eeprom_read(LEADERBOARD_MEM_BASE, buf, LEADERBOARD_SLOTS * LEADERBOARD_SLOT_BYTES);
    for (int i = 0; i < LEADERBOARD_SLOTS; i ++) {
        char *initials = malloc(5 * sizeof(char));
        initials[0] = buf[i * LEADERBOARD_SLOT_BYTES];
        initials[1] = buf[i * LEADERBOARD_SLOT_BYTES + 1];
        initials[2] = buf[i * LEADERBOARD_SLOT_BYTES + 2];
        initials[3] = buf[i * LEADERBOARD_SLOT_BYTES + 3];
        initials[4] = '\0';
        uint32_t score = *((uint32_t*) &buf[i * LEADERBOARD_SLOT_BYTES + 4]);
        leaderboard[i] = (leaderboard_slot_t) {.initials = initials, .score = score};
    }
    return leaderboard;
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