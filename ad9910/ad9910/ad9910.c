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

// ### debug (STM32 HAL Platform) ###
#ifdef USE_HAL_DRIVER
#ifdef DEBUG
__STATIC_INLINE uint32_t GXT_SYSTICK_IsActiveCounterFlag(void)
{
    return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}
static uint32_t getCurrentMicros(void)
{
    /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
    GXT_SYSTICK_IsActiveCounterFlag();
    uint32_t m = HAL_GetTick();
    const uint32_t tms = SysTick->LOAD + 1;
    __IO uint32_t u = tms - SysTick->VAL;
    if (GXT_SYSTICK_IsActiveCounterFlag())
    {
        m = HAL_GetTick();
        u = tms - SysTick->VAL;
    }
    return (m * 1000 + (u * 1000) / tms);
}

#endif
#endif
// ### debug finish ###

// ### Link layer ###

static int ad9910_set_ports(ad9910_PortSettingsTypeDef *port)
{

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_ports entry\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

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

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_ports exit\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

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

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pin entry\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

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

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pin exit\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    return 0;
}

static int ad9910_clr_pin(unsigned int mask_code)
{

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_clr_pin entry\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

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

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_clr_pin exit\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    return 0;
}

static unsigned char ad9910_rxtx_bytes(unsigned char *data_tx, unsigned char *data_rx, unsigned int data_size)
{
    HAL_SPI_TransmitReceive(__ad9910_hspi, data_tx, data_rx, data_size, HAL_MAX_DELAY);

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

    send_buf[1] = (*value >> 24);
    send_buf[2] = (*value >> 16);
    send_buf[3] = (*value >> 8);
    send_buf[4] = (*value >> 0);

#ifdef DEBUG
    printf("[%.3f] ad9910: ad9910_write_reg send_buf:%02x %02x %02x %02x %02x | %08x\r\n", 1.0 * getCurrentMicros() / 1000.0,
           send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4], *value);
#endif

    ad9910_clr_pin(AD9910_GPIO_IOUPDATE);
    HAL_Delay(1);
    ad9910_rxtx_bytes(send_buf, recv_buf, 5);
    HAL_Delay(1);
    ad9910_set_pin(AD9910_GPIO_IOUPDATE);

#ifdef DEBUG
    printf("[%.3f] ad9910: ad9910_write_reg recv_buf:%02x %02x %02x %02x %02x\r\n", 1.0 * getCurrentMicros() / 1000.0,
           recv_buf[0], recv_buf[1], recv_buf[2], recv_buf[3], recv_buf[4]);
#endif

    return 0;
}

static int ad9910_read_reg(unsigned char addr, unsigned int *value)
{
    unsigned char send_buf[5] = {0};
    unsigned char recv_buf[5] = {0};

    memcpy(&(send_buf[0]), &addr, sizeof(unsigned char));
    send_buf[0] &= (~(((unsigned char)(1 << 5)) | ((unsigned char)(1 << 6)))); // Clear bit[6:5]
    send_buf[0] |= ((unsigned char)(1 << 7));                                  // Set bit[7] = 1 to read register

#ifdef DEBUG
    printf("[%.3f] ad9910: ad9910_read_reg send_buf:%02x %02x %02x %02x %02x\r\n", 1.0 * getCurrentMicros() / 1000.0,
           send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4]);
#endif

    ad9910_clr_pin(AD9910_GPIO_IOUPDATE);
    HAL_Delay(1);
    ad9910_rxtx_bytes(send_buf, recv_buf, 5);
    HAL_Delay(1);
    ad9910_set_pin(AD9910_GPIO_IOUPDATE);
#ifdef DEBUG
    printf("[%.3f] ad9910: ad9910_read_reg recv_buf:%02x %02x %02x %02x %02x\r\n", 1.0 * getCurrentMicros() / 1000.0,
           recv_buf[0], recv_buf[1], recv_buf[2], recv_buf[3], recv_buf[4]);
#endif

    memcpy(value, &(recv_buf[1]), sizeof(unsigned int));

    return 0;
}

// ## Protocol layer finish ###

// ## Application layer ###
/**
 * @brief Function to set PLL.
 *
 * @param pll_en 1 to enable PLL, 0 to disable PLL.
 * @param input_mux_sel 1 to select bypassed mode, 0 to select divided mode.
 * @param refclk_out_current use definition AD9910_REFCLK_OUTCURRENT_x to set output current.
 * @param charge_pump_cur_sel use definition AD9910_PLL_CHARGE_PUMP_CUR_x to select PLL charge pump current.
 * @param main_divider_value value range from 0 to 127 (low 7 bit valid) to set the parameter of PLL divider.
 * @param vco_sel use definition AD9910_VCO_x to select VCO.
 *
 * @retval Error code.
 */
int ad9910_set_pll(unsigned char pll_en, unsigned char input_mux_sel, unsigned char refclk_out_current,
                   unsigned char charge_pump_cur_sel, unsigned char main_divider_value, unsigned char vco_sel)
{

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pll entry\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    unsigned int cfr3_value = 0;

    ad9910_read_reg(AD9910_REG_CFR3, &cfr3_value);

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pll cfr3_value: %08x\r\n", 1.0f * getCurrentMicros() / 1000.0f,
           cfr3_value);
#endif

    cfr3_value &= (~((0x01 << 14) |
                     (0x01 << 15) |
                     (0x01 << 8) |
                     (0x07 << 28) |
                     (0x07 << 19) |
                     (0x7f << 1) |
                     (0x07 << 24))); // Clear bits

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pll cfr3_value: %08x\r\n", 1.0f * getCurrentMicros() / 1000.0f,
           cfr3_value);
#endif

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pll: %02x %02x %02x %02x %02x %02x \r\n", 1.0f * getCurrentMicros() / 1000.0f,
           input_mux_sel, pll_en, refclk_out_current, charge_pump_cur_sel, main_divider_value, vco_sel);
#endif

    cfr3_value |= (((~input_mux_sel) & 0x1) << 14) |
                  ((input_mux_sel & 0x1) << 15) |
                  ((pll_en & 0x1) << 8) |
                  ((refclk_out_current & 0x3) << 28) |
                  ((charge_pump_cur_sel & 0x7) << 19) |
                  ((main_divider_value & 0x7f) << 1) |
                  ((vco_sel & 0x7) << 24); // Set bits

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_pll cfr3_value: %08x\r\n", 1.0f * getCurrentMicros() / 1000.0f,
           cfr3_value);
#endif

    ad9910_write_reg(AD9910_REG_CFR3, &cfr3_value);

    return 0;
}

/**
 * @brief Initialize AD9910.
 *
 * @param port a ad9910_PortSettingsTypeDef to set GPIO and SPI.
 *
 * @retval Error code.
 */
int ad9910_init(ad9910_PortSettingsTypeDef *port)
{

#ifdef DEBUG
    printf("[%.3f] ad9910: ad9910_init entry\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    if (port == NULL)
    {
        return -1;
    }

    // Set Basic parameters
    ad9910_set_ports(port);
    ad9910_clr_pin(~0);

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_ports Set Basic parameters\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    // Reset chip
    ad9910_set_pin(AD9910_GPIO_RESET);
    AD9910_DELAY_MS(1);
    ad9910_clr_pin(AD9910_GPIO_RESET);

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_ports Reset chip\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    // Reset I/O port
    ad9910_set_pin(AD9910_GPIO_IOUPDATE);
    AD9910_DELAY_MS(1);

#ifdef DEBUG
    printf("[%.3f] AD9910: ad9910_set_ports Reset I/O port\r\n", 1.0f * getCurrentMicros() / 1000.0f);
#endif

    return 0;
}

int ad9910_set_singal_tone(unsigned int amptitude, unsigned int frequency, unsigned int init_phase)
{
}

// ## Application layer finish ###