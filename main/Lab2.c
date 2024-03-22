/* i2c - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "font8x8_basic.h"

#include "ssd1306.h"
static const char *TAG = "i2c-example";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DATA_LENGTH 512                  /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 128               /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK)
    {
        return err;
    }
    ESP_LOGI(TAG, "I2C configured successfully");
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
uint8_t UIT[8][128];
const char logo[64][64] = {
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000011100000000",
    "000000000000000000000000111111100000000000000000000111110000000",
    "000000000000000000000111111110011001100000000000001111111000000",
    "000000000000000000011111111001101110000000011111111111111000000",
    "000000000000000001111111100110110000000111111111111111111100000",
    "000000000000000011111110011011000000111111110000001111111111000",
    "000000000000000111111001101100000011111110010000000111110011100",
    "000000000000001111110110110000011111110000001000000011100001110",
    "000000000000011111011001000001111110000000000100000000000000110",
    "000000000000111110100110000111111000000000000010000000000000011",
    "000000000001111011011000011111100000000000000001000000000000011",
    "000000000011110110110000111110000000000000000001000000000000001",
    "000000000011101001000011111000000000000000000000100000000000001",
    "000000000110010110000111100000000000000000000000100000000000001",
    "000000000100101000011111000000000000000000000000010000000000001",
    "000000000010010000111100000000000000000000000000010000000000001",
    "000000000110100011111000000000000000000000000000001000000000001",
    "000000001001000111110000100000000100000000000000001000000000001",
    "000000000110001111100001111100011110000000000000001000000000000",
    "000000001100011111000001111100011111000000000000001000000000010",
    "000000001000111100000011110011101111100000000000001000000000010",
    "000000000001111100000111100111110111100000000000000000000000100",
    "000000000011111000000111001111111011110000000000000100000000000",
    "000000000111110000001110011111111101110000000000000100000001000",
    "000000001111100000001110111111111110111000000000000000000000000",
    "000000011111000000001100111111111110010000000000001000000000000",
    "000000011110000110000000011111111100000001100000001000000000000",
    "000000111110001111001011000011100001001011111000001000000000000",
    "000001111100001111000011111000001111101011111000001000000000000",
    "000011111000001111000111111000001111100011111000001000000000000",
    "000011111000001111000111100000000111110011110000010000000000000",
    "000111110000000111000111000000000001110011110000010000000000000",
    "000111110000000000000000000000000000000000000000100000000000000",
    "001111100000000001111111000000000001111110000000100000000000000",
    "001111100000000000111111000000000011111100000001000000000000000",
    "011111100000111100001111100000000011111000010111000000000000000",
    "011111100000111110000111110000000111110001111110000000000000000",
    "011111100000011111100011111000001111000011111100000000000000000",
    "011111100000001111110000111000011110000111111000000000000000000",
    "111111100000000111111100011100011000011111110000000000000000000",
    "111111100000000011111111101110111011111111100000000000000000000",
    "111111110000000001111111111111111111111110000000000000000000000",
    "111111110000000000011111111111111111111100000000000000000000000",
    "011111111000000000000111111111111111110000000000000000000000000",
    "011111111100000000000000111111111111000000000000000000000000000",
    "001111111110000000000000000000011100000000000000000000000000000",
    "001111111111110000000000000111110000000000000000000000000000000",
    "000111111111111111111111111111000000000000000000000000000000000",
    "000011111111111111111111111000000000000000000000000000000000000",
    "000000111111111111111111000000000000000000000000000000000000000",
    "000000000111111111110000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000",
    "000000000000000000000000000000000000000000000000000000000000000"};
void ssd1306_init()
{
    esp_err_t espRc;

    i2c_cmd_handle_t cmd;
    cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

    i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true);
    i2c_master_write_byte(cmd, 0x14, true);

    i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP, true); // reverse left-right mapping
    i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true); // reverse up-bottom mapping

    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_NORMAL, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_OFF, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);
    i2c_master_stop(cmd);

    espRc = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    if (espRc == ESP_OK)
    {
        ESP_LOGI(TAG, "OLED configured successfully");
    }
    else
    {
        ESP_LOGE(TAG, "OLED configuration failed. code: 0x%.2X", espRc);
    }
    i2c_cmd_link_delete(cmd);
}

void task_ssd1306_display_text(const void *arg_text)
{
    char *text = (char *)arg_text;
    uint8_t text_len = strlen(text);
    i2c_cmd_handle_t cmd;
    uint8_t cur_page = 0;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, 0x00, true);            // reset column - choose column --> 0
    i2c_master_write_byte(cmd, 0x10, true);            // reset line - choose line --> 0
    i2c_master_write_byte(cmd, 0xB0 | cur_page, true); // reset page

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    for (uint8_t i = 0; i < text_len; i++)
    {
        if (text[i] == '\n')
        {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
            i2c_master_write_byte(cmd, 0x00, true); // reset column
            i2c_master_write_byte(cmd, 0x10, true);
            i2c_master_write_byte(cmd, 0xB0 | ++cur_page, true); // increment page

            i2c_master_stop(cmd);
            i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }
        else
        {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
            i2c_master_write(cmd, font8x8_basic_tr[(uint8_t)text[i]], 8, true);

            i2c_master_stop(cmd);
            i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }
    }

    vTaskDelete(NULL);
}

void task_ssd1306_display_clear(void *ignore)
{
    i2c_cmd_handle_t cmd;

    uint8_t clear[128];
    for (uint8_t i = 0; i < 128; i++)
    {
        clear[i] = 0;
    }
    for (uint8_t i = 0; i < 8; i++)
    {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
        i2c_master_write_byte(cmd, 0xB0 | i, true);

        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
        i2c_master_write(cmd, clear, 128, true);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
    }

    vTaskDelete(NULL);
}
void convert_pixel_to_page()
{
    for (int8_t i = 0; i < 8; ++i)
        for (uint8_t j = 0; j < 63; ++j)
        {
            uint8_t t = 0;
            for (int8_t k = 7; k >= 0; --k)
            {
                t = (t << 1) + logo[i * 8 + k][j] - '0';
            }
            UIT[i][j] = t;
        }
}
void draw_logo_on_oled(void *ignore)
{
    i2c_cmd_handle_t cmd;
    uint8_t cur_page = 0;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, 0x00, true);            // reset column - choose column --> 0
    i2c_master_write_byte(cmd, 0x10, true);            // reset line - choose line --> 0
    i2c_master_write_byte(cmd, 0xB0 | cur_page, true); // reset page
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    // draw
    for (uint8_t i = 0; i < 8; i++)
    {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
        i2c_master_write(cmd, UIT[i], 128, true);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        //  tang hang
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
        i2c_master_write_byte(cmd, 0x00, true); // reset column
        i2c_master_write_byte(cmd, 0x10, true);
        i2c_master_write_byte(cmd, 0xB0 | ++cur_page, true); // increment page

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
    }
    vTaskDelete(NULL);
}
void app_main(void)
{

    ESP_ERROR_CHECK(i2c_master_init());
    ssd1306_init();
    convert_pixel_to_page();
    xTaskCreate(task_ssd1306_display_text, "print_studentid", 1024 * 1, (void *)"21522251\n21521613", 10, NULL); // cau 2
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    xTaskCreate(task_ssd1306_display_clear, "clear", 1024 * 1, (void *)0, 10, NULL);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    xTaskCreate(draw_logo_on_oled, "drawlogo", 1024 * 1, (void *)0, 10, NULL); // cau 3
}
