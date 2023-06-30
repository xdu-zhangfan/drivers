#include "ad9910.h"

// GPIO and SPI ports
static SPI_HandleTypeDef *__ad9910_hspi;

static GPIO_TypeDef *__ad9910_sync_gpio_group;
static uint16_t __ad9910_sync_gpio_pin;
static GPIO_TypeDef *__ad9910_rso_gpio_group;
static uint16_t __ad9910_rso_gpio_pin;
static GPIO_TypeDef *__ad9910_pf0_gpio_group;
static uint16_t __ad9910_pf0_gpio_pin;
static GPIO_TypeDef *__ad9910_pf1_gpio_group;
static uint16_t __ad9910_pf1_gpio_pin;
static GPIO_TypeDef *__ad9910_pf2_gpio_group;
static uint16_t __ad9910_pf2_gpio_pin;
static GPIO_TypeDef *__ad9910_osk_gpio_group;
static uint16_t __ad9910_osk_gpio_pin;
static GPIO_TypeDef *__ad9910_drover_gpio_group;
static uint16_t __ad9910_drover_gpio_pin;
static GPIO_TypeDef *__ad9910_drctl_gpio_group;
static uint16_t __ad9910_drctl_gpio_pin;
static GPIO_TypeDef *__ad9910_drhold_gpio_group;
static uint16_t __ad9910_drhold_gpio_pin;
static GPIO_TypeDef *__ad9910_ioupdate_gpio_group;
static uint16_t __ad9910_ioupdate_gpio_pin;
static GPIO_TypeDef *__ad9910_reset_gpio_group;
static uint16_t __ad9910_reset_gpio_pin;

// ### Link layer ###

static int ad9910_set_ports(ad9910_PortSettingsTypeDef *port)
{
    __ad9910_hspi = port->hspi;

    __ad9910_sync_gpio_group = port->sync_gpio_group;
    __ad9910_sync_gpio_pin = port->sync_gpio_pin;
    __ad9910_rso_gpio_group = port->rso_gpio_group;
    __ad9910_rso_gpio_pin = port->rso_gpio_pin;
    __ad9910_pf0_gpio_group = port->pf0_gpio_group;
    __ad9910_pf0_gpio_pin = port->pf0_gpio_pin;
    __ad9910_pf1_gpio_group = port->pf1_gpio_group;
    __ad9910_pf1_gpio_pin = port->pf1_gpio_pin;
    __ad9910_pf2_gpio_group = port->pf2_gpio_group;
    __ad9910_pf2_gpio_pin = port->pf2_gpio_pin;
    __ad9910_osk_gpio_group = port->osk_gpio_group;
    __ad9910_osk_gpio_pin = port->osk_gpio_pin;
    __ad9910_drover_gpio_group = port->drover_gpio_group;
    __ad9910_drover_gpio_pin = port->drover_gpio_pin;
    __ad9910_drctl_gpio_group = port->drctl_gpio_group;
    __ad9910_drctl_gpio_pin = port->drctl_gpio_pin;
    __ad9910_drhold_gpio_group = port->drhold_gpio_group;
    __ad9910_drhold_gpio_pin = port->drhold_gpio_pin;
    __ad9910_ioupdate_gpio_group = port->ioupdate_gpio_group;
    __ad9910_ioupdate_gpio_pin = port->ioupdate_gpio_pin;
    __ad9910_reset_gpio_group = port->reset_gpio_group;
    __ad9910_reset_gpio_pin = port->reset_gpio_pin;

    return 0;
}

static int ad9910_read_ports(ad9910_PortSettingsTypeDef *port)
{
    port->hspi = __ad9910_hspi;

    port->sync_gpio_group = __ad9910_sync_gpio_group;
    port->sync_gpio_pin = __ad9910_sync_gpio_pin;
    port->rso_gpio_group = __ad9910_rso_gpio_group;
    port->rso_gpio_pin = __ad9910_rso_gpio_pin;
    port->pf0_gpio_group = __ad9910_pf0_gpio_group;
    port->pf0_gpio_pin = __ad9910_pf0_gpio_pin;
    port->pf1_gpio_group = __ad9910_pf1_gpio_group;
    port->pf1_gpio_pin = __ad9910_pf1_gpio_pin;
    port->pf2_gpio_group = __ad9910_pf2_gpio_group;
    port->pf2_gpio_pin = __ad9910_pf2_gpio_pin;
    port->osk_gpio_group = __ad9910_osk_gpio_group;
    port->osk_gpio_pin = __ad9910_osk_gpio_pin;
    port->drover_gpio_group = __ad9910_drover_gpio_group;
    port->drover_gpio_pin = __ad9910_drover_gpio_pin;
    port->drctl_gpio_group = __ad9910_drctl_gpio_group;
    port->drctl_gpio_pin = __ad9910_drctl_gpio_pin;
    port->drhold_gpio_group = __ad9910_drhold_gpio_group;
    port->drhold_gpio_pin = __ad9910_drhold_gpio_pin;
    port->ioupdate_gpio_group = __ad9910_ioupdate_gpio_group;
    port->ioupdate_gpio_pin = __ad9910_ioupdate_gpio_pin;
    port->reset_gpio_group = __ad9910_reset_gpio_group;
    port->reset_gpio_pin = __ad9910_reset_gpio_pin;

    return 0;
}

static int ad9910_set_pin(unsigned int mask_code)
{
    HAL_GPIO_WritePin(__ad9910_sync_gpio_group, __ad9910_sync_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_SYNC));
    HAL_GPIO_WritePin(__ad9910_rso_gpio_group, __ad9910_rso_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_RSO));
    HAL_GPIO_WritePin(__ad9910_pf0_gpio_group, __ad9910_pf0_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_PF0));
    HAL_GPIO_WritePin(__ad9910_pf1_gpio_group, __ad9910_pf1_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_PF1));
    HAL_GPIO_WritePin(__ad9910_pf2_gpio_group, __ad9910_pf2_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_PF2));
    HAL_GPIO_WritePin(__ad9910_osk_gpio_group, __ad9910_osk_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_OSK));
    HAL_GPIO_WritePin(__ad9910_drover_gpio_group, __ad9910_drover_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_DROVER));
    HAL_GPIO_WritePin(__ad9910_drctl_gpio_group, __ad9910_drctl_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_DRCTL));
    HAL_GPIO_WritePin(__ad9910_drhold_gpio_group, __ad9910_drhold_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_DRHOLD));
    HAL_GPIO_WritePin(__ad9910_ioupdate_gpio_group, __ad9910_ioupdate_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_IOUPDATE));
    HAL_GPIO_WritePin(__ad9910_reset_gpio_group, __ad9910_reset_gpio_pin, AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_RESET));

    return 0;
}

static int ad9910_clr_pin(unsigned int mask_code)
{
    HAL_GPIO_WritePin(__ad9910_sync_gpio_group, __ad9910_sync_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_SYNC));
    HAL_GPIO_WritePin(__ad9910_rso_gpio_group, __ad9910_rso_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_RSO));
    HAL_GPIO_WritePin(__ad9910_pf0_gpio_group, __ad9910_pf0_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_PF0));
    HAL_GPIO_WritePin(__ad9910_pf1_gpio_group, __ad9910_pf1_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_PF1));
    HAL_GPIO_WritePin(__ad9910_pf2_gpio_group, __ad9910_pf2_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_PF2));
    HAL_GPIO_WritePin(__ad9910_osk_gpio_group, __ad9910_osk_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_OSK));
    HAL_GPIO_WritePin(__ad9910_drover_gpio_group, __ad9910_drover_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_DROVER));
    HAL_GPIO_WritePin(__ad9910_drctl_gpio_group, __ad9910_drctl_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_DRCTL));
    HAL_GPIO_WritePin(__ad9910_drhold_gpio_group, __ad9910_drhold_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_DRHOLD));
    HAL_GPIO_WritePin(__ad9910_ioupdate_gpio_group, __ad9910_ioupdate_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_IOUPDATE));
    HAL_GPIO_WritePin(__ad9910_reset_gpio_group, __ad9910_reset_gpio_pin, !AD9910_IF_BIT_SET(mask_code, AD9910_GPIO_RESET));

    return 0;
}

static unsigned char ad9910_rxtx_bytes(unsigned char *data_tx, unsigned char *data_rx, unsigned int data_size)
{
    HAL_SPI_TransmitReceive(__ad9910_hspi, &data_tx, &data_rx, data_size, HAL_MAX_DELAY);

    return 0;
}

// ### Link layer finish ###

// ## Protocol layer ###

static int ad9910_write_reg(unsigned char addr, unsigned int *value)
{
    unsigned char send_buf[5] = {0};
    unsigned char recv_buf[5] = {0};

    memcpy(&(send_buf[0]), &addr, sizeof(unsigned char));
    send_buf[0] &= 0x1f; // Clear bit[7:5], set bit[7] = 0 to write register

    memcpy(&(send_buf[1]), value, sizeof(unsigned int));

    ad9910_rxtx_bytes(send_buf, recv_buf, 5);

    return 0;
}

static int ad9910_read_reg(unsigned char addr, unsigned int *value)
{
    unsigned char send_buf[5] = {0};
    unsigned char recv_buf[5] = {0};

    memcpy(&(send_buf[0]), &addr, sizeof(unsigned char));
    send_buf[0] &= (~(((unsigned char)(1 << 5)) | ((unsigned char)(1 << 6)))); // Clear bit[6:5]
    send_buf[0] |= ((unsigned char)(1 << 7));                                  // Set bit[7] = 1 to read register

    ad9910_rxtx_bytes(send_buf, recv_buf, 5);
    memcpy(value, &(recv_buf[1]), sizeof(unsigned int));

    return 0;
}

// ## Protocol layer finish ###

// ## Application layer ###

int ad9910_set_pll(unsigned char pll_en, unsigned char input_mux_sel, unsigned char refclk_out_current,
                   unsigned char charge_pump_cur_sel, unsigned char main_divider_value, unsigned char vco_sel)
{
    unsigned int cfr3_value = 0;

    ad9910_read_reg(AD9910_REG_CFR3, &cfr3_value);

    cfr3_value &= (~((0x01 << 14) |
                     (0x01 << 15) |
                     (0x01 << 8) |
                     (0x07 << 28) |
                     (0x07 << 19) |
                     (0x7f << 1) |
                     (0x07 << 24))); // Clear bits

    cfr3_value |= (((~input_mux_sel) & 0x1) << 14) |
                  ((input_mux_sel & 0x1) << 15) |
                  ((pll_en & 0x1) << 8) |
                  ((refclk_out_current & 0x7) << 28) |
                  ((charge_pump_cur_sel & 0x7) << 19) |
                  ((main_divider_value & 0x7f) << 1) |
                  ((vco_sel & 0x7) << 24); // Set bits

    ad9910_write_reg(AD9910_REG_CFR3, &cfr3_value);

    return 0;
}

int ad9910_init(ad9910_PortSettingsTypeDef *port)
{
    if (port == NULL)
    {
        return -1;
    }

    // Set Basic parameters
    ad9910_set_ports(port);

    // Reset chip
    ad9910_set_pin(AD9910_GPIO_RESET);
    AD9910_DELAY_MS(1);
    ad9910_clr_pin(AD9910_GPIO_RESET);

    // Reset I/O port
    ad9910_set_pin(AD9910_GPIO_IOUPDATE);
    AD9910_DELAY_MS(1);

    return 0;
}

// ## Application layer finish ###