#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

//always increment by 32, or 0x20
int LOCATION = 0;
uint32_t start_time;

//required functions: init timer, start timer, end timer, get time, write score, read score(s)
//use SDK function time_us_32() to get the current 32 bit time value
void start_game_timer() {
    start_time = time_us_32();
}

uint32_t end_game_timer() {
    uint32_t game_time = time_us_32() - start_time;
    return game_time;
}

void init_i2c() {
    gpio_set_function(20, GPIO_FUNC_I2C);
    //gpio_pull_up(20);
    gpio_set_function(21, GPIO_FUNC_I2C);
    //gpio_pull_up(21);
    //REMEMBER TO PUT PHYSICAL PULL UP RESISTORS IN THE CIRCUIT
    i2c_init(i2c0, 100 * 1000); //100 kbs with fast mode
}

/*
writes to a location in the 24AA32AF EEPROM device

loc: The address in the EEPROM device to write to. Best practice is to align to multiples of 32.
time: The value to be written, obtained from timerawl
*/
void eeprom_write(uint16_t loc, uint32_t time) {
    uint8_t buffer [3] = {(loc & 0xFF00) >> 8, loc & 0xFF, (time & 0xFF000000) >> 24};
    int responses [4] = {0, 0, 0, 0};
    printf("Numbers: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
    responses[0] = i2c_write_blocking(i2c0, 0x50, &buffer, 3, false);
    printf("Error: %d\n", responses[0]);
    //sleep_ms(4);

    buffer[2] = (time & 0xFF0000) >> 16;
    printf("Numbers: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
    responses[1] = i2c_write_blocking(i2c0, 0x50, &buffer, 3, false);
    printf("Error: %d\n", responses[1]);
    //sleep_ms(4);

    buffer[2] = (time & 0xFF00) >> 8;
    printf("Numbers: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
    responses[2] = i2c_write_blocking(i2c0, 0x50, &buffer, 3, false);
    printf("Error: %d\n", responses[2]);
    //sleep_ms(4);

    buffer[2] = (time & 0xFF);
    printf("Numbers: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
    responses[3] = i2c_write_blocking(i2c0, 0x50, &buffer, 3, false);
    printf("Error: %d\n", responses[3]);

    // i2c_write_blocking(i2c0, 0x50, (&loc) + 8, 1, true);
    // i2c_write_blocking(i2c0, 0x50, (&loc), 1, true);
    // i2c_write_blocking(i2c0, 0x50, (&time) + 16, 1, false);

    // i2c_write_blocking(i2c0, 0x50, (&loc) + 8, 1, true);
    // i2c_write_blocking(i2c0, 0x50, (&loc), 1, true);
    // i2c_write_blocking(i2c0, 0x50, (&time) + 8, 1, false);

    // i2c_write_blocking(i2c0, 0x50, (&loc) + 8, 1, true);
    // i2c_write_blocking(i2c0, 0x50, (&loc), 1, true);
    // i2c_write_blocking(i2c0, 0x50, (&time), 1, false);
}

/*
reads from a location in the 24AA32AF EEPROM device

loc: The address in the EEPROM device to read from. Best practice is to align to multiples of 32.
data_read: A pointer to a 32 bit int to store the data once it is read
length: The length of the data to be read in bytes. In this case, it should always be 4.
*/
void eeprom_read(uint16_t loc, uint32_t * data_read, uint8_t length) {
    printf("READ\n");
    uint8_t buffer [2] = {(loc & 0xFF00) >> 8, loc & 0xFF};
    printf("Numbers: %d %d\n", buffer[0], buffer[1]);
    //sleep_ms(4);
    int resp1 = i2c_write_blocking(i2c0, 0x50, &buffer, 2, true);
    printf("Error: %d\n", resp1);
    //sleep_ms(4);

    if(resp1 != PICO_ERROR_GENERIC) {
        int resp2 = i2c_read_blocking(i2c0, 0x50, &data_read, 4, false);
        printf("Error: %d\n", resp2);
    }
}

// int main() { //USED FOR TESTING ONLY
//     stdio_init_all();
//     init_i2c();
//     uint32_t test_nums [2] = {101, 1495};
//     uint8_t test_int [4] = {0, 0, 0, 0};
//     eeprom_write(0, test_nums[0]);
//     eeprom_read(0, &test_int, 4);
//     printf("Number Read from EEPROM: %ld", test_int);
//     return 0;
// }