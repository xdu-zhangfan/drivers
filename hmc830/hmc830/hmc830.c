#include "hmc830.h"

static GPIO_TypeDef *gpio_sen_group;
static uint32_t gpio_sen_pin;
static GPIO_TypeDef *gpio_sdo_group;
static uint32_t gpio_sdo_pin;
static GPIO_TypeDef *gpio_sdi_group;
static uint32_t gpio_sdi_pin;
static GPIO_TypeDef *gpio_sck_group;
static uint32_t gpio_sck_pin;

static int hmc830_set_pin(unsigned int pins)
{
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SCK))
    {
        HAL_GPIO_WritePin(gpio_sck_group, gpio_sck_pin, 1);
    }
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SDO))
    {
        HAL_GPIO_WritePin(gpio_sdo_group, gpio_sdo_pin, 1);
    }
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SDI))
    {
        HAL_GPIO_WritePin(gpio_sdi_group, gpio_sdi_pin, 1);
    }
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SEN))
    {
        HAL_GPIO_WritePin(gpio_sen_group, gpio_sen_pin, 1);
    }

    return 0;
}

static int hmc830_clr_pin(unsigned int pins)
{
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SCK))
    {
        HAL_GPIO_WritePin(gpio_sck_group, gpio_sck_pin, 0);
    }
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SDO))
    {
        HAL_GPIO_WritePin(gpio_sdo_group, gpio_sdo_pin, 0);
    }
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SDI))
    {
        HAL_GPIO_WritePin(gpio_sdi_group, gpio_sdi_pin, 0);
    }
    if (HMC830_IS_BIT_SET(pins, HMC830_PIN_SEN))
    {
        HAL_GPIO_WritePin(gpio_sen_group, gpio_sen_pin, 0);
    }

    return 0;
}

static unsigned int hmc830_read_pin(unsigned int pin)
{
    unsigned int pin_value = 0;

    if (HMC830_IS_BIT_SET(pin, HMC830_PIN_SCK))
    {
        pin_value = HAL_GPIO_ReadPin(gpio_sck_group, gpio_sck_pin);
    }
    else if (HMC830_IS_BIT_SET(pin, HMC830_PIN_SDO))
    {
        pin_value = HAL_GPIO_ReadPin(gpio_sdo_group, gpio_sdo_pin);
    }
    else if (HMC830_IS_BIT_SET(pin, HMC830_PIN_SDI))
    {
        pin_value = HAL_GPIO_ReadPin(gpio_sdi_group, gpio_sdi_pin);
    }
    else if (HMC830_IS_BIT_SET(pin, HMC830_PIN_SEN))
    {
        pin_value = HAL_GPIO_ReadPin(gpio_sen_group, gpio_sen_pin);
    }

    return pin_value;
}

int hmc830_delay(void)
{
    for (unsigned int i = 0; i < HMC830_DELAY_FACTOR; ++i)
        ;

    return 0;
}

static int hmc830_spi_write_reg(unsigned char addr, unsigned int *data)
{
    addr = addr & 0x3f;
    // addr = addr | 0x40;

    hmc830_set_pin(HMC830_PIN_SEN);

    // Write address and W flag
    for (int i = 6; i >= 0; --i)
    {
        unsigned char send_data = 0;
        send_data = (addr >> i) & 0x1;

        hmc830_delay();
        hmc830_clr_pin(HMC830_PIN_SCK);
        if (send_data == 0)
            hmc830_clr_pin(HMC830_PIN_SDI);
        else if (send_data == 1)
            hmc830_set_pin(HMC830_PIN_SDI);

        hmc830_delay();
        hmc830_set_pin(HMC830_PIN_SCK);
    }
    hmc830_clr_pin(HMC830_PIN_SDI);

    // Write data
    for (int i = 23; i >= 0; --i)
    {
        unsigned char send_data = 0;
        send_data = ((*data) >> i) & 0x1;

        hmc830_delay();
        hmc830_clr_pin(HMC830_PIN_SCK);
        if (send_data == 0)
            hmc830_clr_pin(HMC830_PIN_SDI);
        else if (send_data == 1)
            hmc830_set_pin(HMC830_PIN_SDI);

        hmc830_delay();
        hmc830_set_pin(HMC830_PIN_SCK);
    }
    hmc830_delay();
    hmc830_clr_pin(HMC830_PIN_SCK);
    hmc830_clr_pin(HMC830_PIN_SDI);
    hmc830_clr_pin(HMC830_PIN_SEN);

    return 0;
}

static int hmc830_spi_read_reg(unsigned char addr, unsigned int *data)
{
    *data = 0;
    addr = addr & 0x3f;
    addr = addr | 0x40;

    hmc830_set_pin(HMC830_PIN_SEN);

    // Write address and R flag
    for (int i = 6; i >= 0; --i)
    {
        unsigned char send_data = 0;
        send_data = (addr >> i) & 0x1;

        if (send_data == 0)
            hmc830_clr_pin(HMC830_PIN_SDI);
        else if (send_data == 1)
            hmc830_set_pin(HMC830_PIN_SDI);
        hmc830_delay();

        hmc830_set_pin(HMC830_PIN_SCK);
        hmc830_delay();

        hmc830_clr_pin(HMC830_PIN_SCK);
    }
    hmc830_clr_pin(HMC830_PIN_SDI);

    // Read data
    for (int i = 23; i >= 0; --i)
    {

        hmc830_delay();
        hmc830_set_pin(HMC830_PIN_SCK);

        hmc830_delay();
        hmc830_clr_pin(HMC830_PIN_SCK);
        (*data) |= ((unsigned int)(hmc830_read_pin(HMC830_PIN_SDO) << i));
    }

    hmc830_clr_pin(HMC830_PIN_SEN);

    return 0;
}

int hmc830_init(GPIO_TypeDef *gpio_sen_group_i, uint32_t gpio_sen_pin_i,
                GPIO_TypeDef *gpio_sdo_group_i, uint32_t gpio_sdo_pin_i,
                GPIO_TypeDef *gpio_sdi_group_i, uint32_t gpio_sdi_pin_i,
                GPIO_TypeDef *gpio_sck_group_i, uint32_t gpio_sck_pin_i)
{
    gpio_sen_group = gpio_sen_group_i;
    gpio_sen_pin = gpio_sen_pin_i;

    gpio_sdo_group = gpio_sdo_group_i;
    gpio_sdo_pin = gpio_sdo_pin_i;

    gpio_sdi_group = gpio_sdi_group_i;
    gpio_sdi_pin = gpio_sdi_pin_i;

    gpio_sck_group = gpio_sck_group_i;
    gpio_sck_pin = gpio_sck_pin_i;

    hmc830_clr_pin(HMC830_PIN_SCK | HMC830_PIN_SDI | HMC830_PIN_SDO | HMC830_PIN_SEN);

    unsigned int write_data;

    // Set Reg01h to SPI mode
    write_data = 0x000002;
    hmc830_spi_write_reg(0x01, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }

    // to 10MHz input reference clock, this value may be set to 0x000005
    write_data = 0x000005;
    hmc830_spi_write_reg(0x02, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }

    // Automatically forwarded to the VCO subsystem by the VCO SPI state machine controller
    write_data = 0x000005;
    hmc830_spi_write_reg(0x05, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }

    write_data = 0x202f4a;
    hmc830_spi_write_reg(0x06, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }

    write_data = 0x0bcf3c;
    hmc830_spi_write_reg(0x09, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }

    return 0;
}

int hmc830_set_dividers(unsigned int integer, float fraction)
{
    unsigned int write_data = 0;

    write_data = integer;
    hmc830_spi_write_reg(0x03, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }

    write_data = 16777216.0f * fraction + 0.5f;
    hmc830_spi_write_reg(0x04, &write_data);
    for (int i = 0; i < 10; ++i)
    {
        hmc830_delay();
    }
}

int hmc830_chip_read_regs(unsigned int *buffer)
{
    for (unsigned char addr = 0; addr <= 0x13; ++addr)
    {
        hmc830_spi_read_reg(addr, &(buffer[addr]));
        for (int i = 0; i < 10; ++i)
        {
            hmc830_delay();
        }
    }

    return 0;
}