/////////////////////////////////////////////
// get time from rtc 0x68 
// temp from 0x48
// temp from 0x49
// display on 0x3C
/////////////////////////////////////////////

#include <stdio.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include <time.h>
#include <sys/time.h>

#define PRECOMPILED_TIMESTAMP __TIME__

#define I2C_MASTER_SDA_IO 2
#define I2C_MASTER_SCL_IO 1
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define TMP1075_1_I2C_ADDR 0x48
#define TMP1075_2_I2C_ADDR 0x49

static ssd1306_handle_t ssd1306_dev = NULL;

void i2c_master_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void read_tmp1075(uint8_t i2c_address, uint8_t *temp) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2c_address << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, temp, I2C_MASTER_ACK );
    i2c_master_read_byte(cmd, temp + 1, I2C_MASTER_NACK );
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

double convert_tmp1075_temp(uint8_t *temp) {
    // join the 2 bytes
    uint16_t temp_16bits = (temp[0] << 8) | temp[1];
    // TMP1075 datasheet :
    // "first 12 MSBs are used to indicate temperature while the remaining 4 LSB are set to zero"
    int16_t temp_12bits = temp_16bits >> 4;
    // TMP1075 datasheet :
    // "negative numbers are represented in binary two's-complement format"
    // if most significant bit is 1, its negative, so convert to 2-comp
    // complement of each bit by changing zeroes to ones and ones to zero
    if (temp_12bits & 0x800) {
        // negative and add +1
        // temp2_12bits = -((~temp2_12bits & 0xFFF) + 1); // bitwise NOTs + AND with 1's
        temp_12bits = -((temp_12bits ^ 0xFFF) + 1); // XORs with 1's
    }
    // 0.0625 resolution so divide by 16
    return (double)temp_12bits / 16.0;
}

void app_main(void) {

    i2c_master_init();

    // I2C display
    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);

    // I2C temperatures
    char string_temp_in[20] = "I 00.0dC"; // °
    char string_temp_out[20] = "O 00.0dC"; // °

    // RTC
    struct tm tm = {
        .tm_sec = 0, .tm_min = 0, .tm_hour = 12,
        .tm_mday = 1, .tm_mon = 0, .tm_year = 123, .tm_wday = 0, .tm_yday = 0
    };
    strptime(PRECOMPILED_TIMESTAMP, "%H:%M:%S", &tm);
    struct timeval tv = {
        .tv_sec = mktime(&tm),
        .tv_usec = 0
    };
    settimeofday(&tv, NULL);

    time_t current_time;
    struct tm *timeinfo;
    char string_time[16];


    while (1) {

        // time
        current_time = time(NULL);
        timeinfo = localtime(&current_time);
        snprintf(string_time, sizeof(string_time), "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        ssd1306_draw_string(ssd1306_dev, 32, 13, (const uint8_t *)string_time, 16, 1);
        ssd1306_refresh_gram(ssd1306_dev);

        // temp #1
        uint8_t temp1[2];
        read_tmp1075(TMP1075_1_I2C_ADDR, temp1);
        double temp1_converted = convert_tmp1075_temp(temp1);
        snprintf(string_temp_in, sizeof(string_temp_in), "I%5.1f.C", temp1_converted);
        ssd1306_draw_string(ssd1306_dev, 32, 29, (const uint8_t *)string_temp_in, 16, 1);
        ssd1306_refresh_gram(ssd1306_dev);

        // temp #2
        uint8_t temp2[2];
        read_tmp1075(TMP1075_2_I2C_ADDR, temp2);
        double temp2_converted = convert_tmp1075_temp(temp2);
        snprintf(string_temp_out, sizeof(string_temp_out), "O%5.1f.C", temp2_converted);
        ssd1306_draw_string(ssd1306_dev, 32, 45, (const uint8_t *)string_temp_out, 16, 1);
        ssd1306_refresh_gram(ssd1306_dev);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}