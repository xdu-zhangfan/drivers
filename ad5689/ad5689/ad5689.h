#ifndef _AD5689_H_
#define _AD5689_H_

#include "main.h"

#define AD5689_PIN_LDAC 0X1
#define AD5689_PIN_RST 0X2

#define AD5689_CMD_WRITE_INPUT_REG 0X1
#define AD5689_CMD_UPDATE_DAC_REG 0X2
#define AD5689_CMD_DAC_POWER_CTRL 0X4
#define AD5689_CMD_HM_LDAC 0X5
#define AD5689_CMD_SW_RST 0X6
#define AD5689_CMD_SET_DCEN 0X9
#define AD5689_CMD_SET_READ_BACK 0Xa

// DAC register values (Mask codes) in argument[uint8_t dac_ch] function
#define AD5689_DAC_CH_A_ADDR 0X1
#define AD5689_DAC_CH_B_ADDR 0X8

int ad5689_init(
    // Configure spi interface
    SPI_HandleTypeDef *ad5689_hspi_i,
    // LDAC GPIO
    GPIO_TypeDef *ad5689_ldac_gpio_group_i,
    uint16_t ad5689_ldac_gpio_pin_i,

    // RESET GPIO
    GPIO_TypeDef *ad5689_rst_gpio_i,
    uint16_t ad5689_rst_gpio_pin_i);

uint32_t ad5689_set_dac(uint32_t data, uint8_t dac_ch, uint8_t write_flag, uint8_t update_flag);

#endif // _AD5689_H_