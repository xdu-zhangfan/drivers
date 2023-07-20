#ifndef HMC830_H
#define HMC830_H

#include "main.h"

#define HMC830_DELAY_FACTOR 20
#define HMC830_REFCLK 50000000

#define HMC830_PIN_SEN ((unsigned int)(1 << 0))
#define HMC830_PIN_SDO ((unsigned int)(1 << 1))
#define HMC830_PIN_SDI ((unsigned int)(1 << 2))
#define HMC830_PIN_SCK ((unsigned int)(1 << 3))

#define HMC830_IS_BIT_SET(reg, bit) HAL_IS_BIT_SET(reg, bit)

int hmc830_init(GPIO_TypeDef *gpio_sen_group_i, uint32_t gpio_sen_pin_i,
                GPIO_TypeDef *gpio_sdo_group_i, uint32_t gpio_sdo_pin_i,
                GPIO_TypeDef *gpio_sdi_group_i, uint32_t gpio_sdi_pin_i,
                GPIO_TypeDef *gpio_sck_group_i, uint32_t gpio_sck_pin_i);
int hmc830_set_dividers(unsigned int integer, float fraction);
int hmc830_chip_read_regs(unsigned int *buffer);

#endif