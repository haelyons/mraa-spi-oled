#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "mraa/gpio.h"
#include "mraa/pwm.h"
#include "mraa/spi.h"

#include "ssd1362_interface.h"

#define BUFFER_WIDTH        256U
#define BUFFER_HEIGHT       64U
#define BUFFER_SIZE         8192U
#define ALIGN_RIGHT         0U
#define ALIGN_LEFT          1U
#define CHAR_SPACING        2U
#define BPP                 4
#define NUM_ROWS            8

#define PWM_PIN       18
#define PWM_FREQ      200
#define SPI_BUS       3
#define SPI_FREQ      400000

#define RES     11 // pin 11
#define DC      13 // pin 13
#define CS      24 // pin 24

#define RES_G   116
#define DC_G    117
#define CS_G    150

#define MOSI    19 // pin 19
#define CLK     23 // pin 23

#define _delay_us(x)	usleep(x)
#define _delay_ms(x)	usleep(x*1000)
#define _delay_s(x)	usleep(x*1000000)


char *fb;

volatile sig_atomic_t flag = 1;
int err;
mraa_spi_context spi;
mraa_gpio_context res, dc, cs;

void sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

void sysfs_pin_init(int gpio)
{
    char buf[100];    
    FILE* fd = fopen("/sys/class/gpio/export", "w");
    fprintf(fd, "%d", gpio);
    fclose(fd);

    sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
    fd = fopen(buf, "w");
    fprintf(fd, "out");
    fclose(fd);

    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
    fd = fopen(buf, "w");

    fd = fopen(buf, "w");
    fprintf(fd, "1");
    fclose(fd);
}

void sysfs_pin_val(int val, int gpio)
{
    char buf[100]; 
    FILE* fd = fopen("/sys/class/gpio/export", "w");
    int length = snprintf( NULL, 0, "%d", val);
    char* str = malloc( length + 1 );

    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
    fd = fopen(buf, "w");

    fd = fopen(buf, "w");
    fprintf(fd, str);
    fclose(fd);
}

int main(int argc, char** argv)
{
    char *fb = (char *) calloc(8192, sizeof(char)); // Initialise framebuffer
    mraa_result_t status = MRAA_SUCCESS;
    
    /* GPIO INITIALISATION */
    signal(SIGINT, sig_handler);
    const char* board_name = mraa_get_platform_name();
    fprintf(stdout, "Version: %s\nRunning on %s\n", mraa_get_version(), board_name);
    mraa_init(); // apparently not always necessary, included in example
    /*
    sysfs_pin_init(RES_G);
    sysfs_pin_init(DC_G);
    sysfs_pin_init(CS_G);
    */

    // INIT PINS
    res = mraa_gpio_init(RES);
    if (res == NULL) 
    {
        fprintf(stderr, "Failed to initialize GPIO %d\n", RES);
        mraa_deinit();
        return EXIT_FAILURE;
    }
    dc = mraa_gpio_init(DC);
    if (dc == NULL) 
    {
        fprintf(stderr, "Failed to initialize GPIO %d\n", DC);
        mraa_deinit();
        return EXIT_FAILURE;
    }
    cs = mraa_gpio_init(CS);
    if (res == NULL) 
    {
        fprintf(stderr, "Failed to initialize GPIO %d\n", CS);
        mraa_deinit();
        return EXIT_FAILURE;
    }

    fprintf(stdout, "TP0\n");
    
    // SET OUTPUT
    status = mraa_gpio_dir(res, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    status = mraa_gpio_dir(dc, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    status = mraa_gpio_dir(cs, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    
    fprintf(stdout, "TP1\n");
    // SET MODE PULLUP (DC, RES)
    status = mraa_gpio_mode(dc, MRAA_GPIO_PULLUP);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    status = mraa_gpio_mode(res, MRAA_GPIO_PULLUP);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    
    fprintf(stdout, "TP2\n");
    /* SPI INITIALISATION */
    spi = mraa_spi_init(SPI_BUS);
    if (spi == NULL) 
    {
        fprintf(stderr, "Failed to initialize SPI\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Successfully initialised SPI bus\n");

    // SPI FREQ
    status = mraa_spi_frequency(spi, SPI_FREQ);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    /*
    // SPI MODE (BIG ENDIAN)
    status = mraa_spi_lsbmode(spi, 0);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    */
    // USE BYTE FUNCTION
    fprintf(stdout, "Opened SPI on Channel 3 [SSD1362]\n");
    fflush(stdout);
    
    /* REMOVE FOR NOW, CAUSING SEG FAULT 
    SSD1362_fb_checker();
    err = SSD1362_fb_write();
    if (err != 0){
        fprintf(stdout, "Error in writing fb to mraa_spi_write_buf()!");
        goto err_exit;
    }
    */

    _delay_ms(1000000); // 10s delay

    // END 
    status = mraa_gpio_close(res);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    status = mraa_gpio_close(dc);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    status = mraa_gpio_close(cs);
    if (status != MRAA_SUCCESS) 
    {
        goto err_exit;
    }
    mraa_spi_stop(spi);
    mraa_deinit();
    fflush(stdout);
    return MRAA_SUCCESS;

err_exit:
    mraa_result_print(status);
    status = mraa_gpio_close(res);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }
    status = mraa_gpio_close(dc);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }
    status = mraa_gpio_close(cs);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }
    mraa_result_print(status);
    mraa_spi_stop(spi);
    mraa_deinit();

    return EXIT_FAILURE;
}

void SSD1362_cmd(char cmd)
{
    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, &cmd);
    mraa_gpio_write(dc, 1);
}

void SSD1362_data(char data)
{
    mraa_spi_write(spi, &data);
}

void SSD1362_init()
{
    mraa_gpio_write(res, 0);
    _delay_ms(150);
    mraa_gpio_write(res, 1); //Reset pin high
    _delay_ms(150);
    _delay_ms(150);

}

int SSD1362_fb_write()
{
    SSD1362_set_columns(SSD1362_COLS_MIN, SSD1362_COLS_MAX);
    SSD1362_set_rows(SSD1362_ROWS_MIN, SSD1362_ROWS_MAX);

    if (fb == NULL) {
        return -1;
    }

    char buffer1[4096];
    char buffer2[4096];
    memcpy(buffer1, fb, 4096);
    memcpy(buffer2, fb + 4096, 4096);
    mraa_spi_write_buf(spi, buffer1, 4096);  // Example function call to transmit data
    mraa_spi_write_buf(spi, buffer2, 4096);  // Transmit the second part

    free(fb);

    return 0;
}

void SSD1362_fb_checker() 
{
    // NSTRIPE
    unsigned int i, j;
    unsigned int n = 0;
    unsigned char b[128], w[128];

    for (n=0; n<128; n++){
        b[n] = 0x00;
        w[n] = 0xFF;
    }

    for (i=0; i<64; i++){
        for (int j = 0; j < 128; j++){
            if (i & 0x01U){
                fb[(i * BUFFER_HEIGHT) + j] = b[j];
            }
            else {
                fb[(i * BUFFER_HEIGHT) + j] = w[j];
            }
        }
    }
}

void SSD1362_stream()
{
    SSD1362_cmd(SSD1362_CMD_SET_WRITE);
    mraa_gpio_write(dc, 1);
}

void SSD1362_set_columns(uint8_t start, uint8_t end) 
{
    SSD1362_cmd(SSD1362_CMD_SET_COLS);
    SSD1362_data(start);
    SSD1362_data(end);
}

void SSD1362_set_rows(uint8_t start, uint8_t end) 
{
    SSD1362_cmd(SSD1362_CMD_SET_ROWS);
    SSD1362_data(start);
    SSD1362_data(end);
}
