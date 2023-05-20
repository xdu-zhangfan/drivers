#include "ad5689.h"

//configure spi interface 
static SPI_HandleTypeDef *ad5689_hspi;

//LDAC GPIO 

static GPIO_TypeDef *ad5689_ldac_gpio_group;
static uint16_t ad5689_ldac_gpio_pin;

//RESET GPIO
static GPIO_TypeDef *ad5689_rst_gpio_group;
static uint16_t ad5689_rst_gpio_pin;

static uint32_t ad5689_send_recv_data(uint32_t tx_data)
{
    uint32_t rx_data ;
    HAL_SPI_TransmitReceive( ad5689_hspi, (uint8_t *)&tx_data, (uint8_t *)&rx_data, 1,HAL_MAX_DELAY);
    return rx_data ;
    
}

static int ad5689_set_pins(uint16_t pins)
{
    if (HAL_IS_BIT_SET(pins ,AD5689_PIN_LDAC))
{
        HAL_GPIO_WritePin(ad5689_ldac_gpio_group ,ad5689_ldac_gpio_pin, 1);
}
    if (HAL_IS_BIT_SET(pins ,AD5689_PIN_RST))
    {
        HAL_GPIO_WritePin (ad5689_rst_gpio_group ,ad5689_rst_gpio_pin, 1);

    }
    return 0;

}

static int ad5689_clr_pins(uint16_t pins)
{
    if (HAL_IS_BIT_SET(pins, AD5689_PIN_LDAC))
    {
        HAL_GPIO_WritePin(ad5689_ldac_gpio_group , ad5689_ldac_gpio_pin ,0);
    }
    if (HAL_IS_BIT_SET(pins,AD5689_PIN_LDAC))
    {
        HAL_GPIO_WritePin(ad5689_rst_gpio_group, ad5689_rst_gpio_pin,0);
    }
    
    return 0;
}

static uint32_t ad5689_send_cmd(uint8_t cmd , uint8_t addr , uint16_t data)
{
    uint32_t send_data =(((uint32_t)cmd) <<(16+4)) | (((uint32_t)addr)<<16) | data;
    return ad5689_send_recv_data(send_data);

}

int ad5689_init(
    //Configure SPI interface
    SPI_HandleTypeDef *ad5689_hspi_i,
    
    //LDAC GPIO 
    GPIO_TypeDef *ad5689_ldac_gpio_group_i,
    uint16_t ad5689_ldac_gpio_pin_i,

    //RESET GPIO 
    GPIO_TypeDef  *ad5689_rst_gpio_group_i,
    uint16_t ad5689_rst_gpio_pin_i

)
{
    //configure values
    ad5689_hspi = ad5689_hspi_i;


    ad5689_ldac_gpio_group = ad5689_ldac_gpio_group_i;
    ad5689_ldac_gpio_pin = ad5689_ldac_gpio_pin_i;

    ad5689_rst_gpio_group = ad5689_rst_gpio_group_i;
    ad5689_rst_gpio_pin = ad5689_rst_gpio_pin_i;

    //Initialize ad5689
    ad5689_set_pins(AD5689_PIN_RST | AD5689_PIN_LDAC);
    HAL_Delay(1);
    ad5689_clr_pins(AD5689_PIN_RST);
    HAL_Delay(1);
    ad5689_set_pins(AD5689_PIN_RST);

    return 0;
}
uint32_t ad5689_set_dac(uint32_t data , uint8_t dac_ch, uint8_t write_flag,uint8_t update_flag)
{

    uint8_t cmd = 0;
    if (write_flag)
    {
        cmd |= AD5689_CMD_WRITE_INPUT_REG;
    }
    if(update_flag)
    {
        cmd |=AD5689_CMD_UPDATE_DAC_REG;

    }
    uint32_t res  = ad5689_send_cmd (cmd ,dac_ch,data);

    return res;
}

