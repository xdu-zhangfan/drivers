#ifndef AD9910
#define AD9910

#include "main.h"

// Difinitions of registers map
#define AD9910_REG_CFR1 0x00
#define AD9910_REG_CFR2 0x01
#define AD9910_REG_CFR3 0x02
#define AD9910_REG_FSC 0x03
#define AD9910_REG_IO_UPDATE_RATE 0x04
#define AD9910_REG_FTW 0x05
#define AD9910_REG_POW 0x08
#define AD9910_REG_ASF 0x09
#define AD9910_REG_MULTI_CHIP_SYNE 0x0a
#define AD9910_REG_DRG_LIMIT 0x0b
#define AD9910_REG_DRG_STEP 0x0c
#define AD9910_REG_DRG_RATE 0x0d
#define AD9910_REG_SF_RAM_PF0 0x0e
#define AD9910_REG_SF_RAM_PF1 0x0f
#define AD9910_REG_SF_RAM_PF2 0x10
#define AD9910_REG_SF_RAM_PF3 0x11
#define AD9910_REG_SF_RAM_PF4 0x12
#define AD9910_REG_SF_RAM_PF5 0x13
#define AD9910_REG_SF_RAM_PF6 0x14
#define AD9910_REG_SF_RAM_PF7 0x15
#define AD9910_REG_RAM_CTRL 0x16

// Parameters settings
typedef struct
{
    uint8_t ram_en;
    uint8_t ram_playback_destination;
    uint8_t osk_outctrl;
    uint8_t inverse_sin_filter_en;
    uint8_t internal_profile_ctrl;
    uint8_t dds_sine_out_sel;
    uint8_t load_lrr_ioupdate;
    uint8_t autoclear_drg_acc;
    uint8_t autoclear_pha_acc;
    uint8_t clear_drg_acc;
    uint8_t clear_pha_acc;
    uint8_t load_arr_ioupdate;
    uint8_t osk_en;
    uint8_t autoosk_sel;
    uint8_t digital_pdn;
    uint8_t dac_pdn;
    uint8_t auxdac_pdn;
    uint8_t ext_pdn_ctrl;
    uint8_t sdio_input_only;
    uint8_t sdio_lsb_first;
    uint8_t amplitude_scale_from_single_tone_profiles_en;
    uint8_t internal_io_update_active;
    uint8_t sync_clk_en;
    uint8_t drg_destination;
    uint8_t drg_no_dwell_high;
    uint8_t drg_no_dwell_low;
    uint8_t read_effective_ftw;
    uint8_t io_update_rate_ctrl;
    uint8_t pdclk_en;
    uint8_t pdclk_invert;
    uint8_t tx_en_invert;
    uint8_t matched_latency_enable;
    uint8_t data_assembler_hold_last_value;
    uint8_t sync_timing_validation_disable;
    uint8_t parallel_data_port_enable;
    uint8_t fm_gain;
    uint8_t drv0;
    uint8_t vco_sel;
    uint8_t i_cp;
    uint8_t refclk_input_divider_bypass;
    uint8_t refclk_input_divider_reset;
    uint8_t pfd_reset;
    uint8_t pll_en;
    uint8_t n;
} ad9910_CFRsTypeDef;

typedef struct
{
    uint16_t amplitude_ramp_rate;
    uint16_t amplitude_scale_factor;
    uint8_t amplitude_step_size;
} ad9910_ASFTypeDef;

typedef struct
{
    uint8_t sync_validation_delay;
    uint8_t sync_receiver_en;
    uint8_t sync_generator_en;
    uint8_t sync_generator_polarity;
    uint8_t sync_state_preset_value;
    uint8_t output_sync_generator_delay;
    uint8_t input_sync_receiver_delay;
} ad9910_MultichipSyncTypeDef;

typedef struct
{
    uint16_t address_step_rate;
    uint16_t waveform_end_address;
} ad9910_RAMProfileTypeDef;

typedef union
{
    uint32_t single_tone;
    ad9910_RAMProfileTypeDef ram;
} ad9910_ProfileTypeDef;

typedef struct
{
    ad9910_CFRsTypeDef cfr;
    uint8_t fsc;
    uint32_t io_update_rate;
    uint32_t ftw;
    uint32_t pow;
    ad9910_ASFTypeDef asf;
    ad9910_MultichipSyncTypeDef nulti_chip_sync;
    uint32_t drg_limit;
    uint32_t drg_decrement_step_size;
    uint32_t drg_step;
    ad9910_ProfileTypeDef profiles[8];
} ad9910_ParametersTypeDef;

// Ports settings
typedef struct
{
    SPI_HandleTypeDef hspi;
    GPIO_TypeDef *sync_gpio_group;
    uint16_t sync_gpio_pin;
    GPIO_TypeDef *rso_gpio_group;
    uint16_t rso_gpio_pin;
    GPIO_TypeDef *pf0_gpio_group;
    uint16_t pf0_gpio_pin;
    GPIO_TypeDef *pf1_gpio_group;
    uint16_t pf1_gpio_pin;
    GPIO_TypeDef *pf2_gpio_group;
    uint16_t pf2_gpio_pin;
    GPIO_TypeDef *osk_gpio_group;
    uint16_t osk_gpio_pin;
    GPIO_TypeDef *drover_gpio_group;
    uint16_t drover_gpio_pin;
    GPIO_TypeDef *drctl_gpio_group;
    uint16_t drctl_gpio_pin;
    GPIO_TypeDef *drhold_gpio_group;
    uint16_t drhold_gpio_pin;
    GPIO_TypeDef *ioupdate_gpio_group;
    uint16_t ioupdate_gpio_pin;
    GPIO_TypeDef *reset_gpio_group;
    uint16_t reset_gpio_pin;
} ad9910_PortSettingsTypeDef;

#endif