#include "ad9910.h"

// GPIO and SPI ports
static SPI_HandleTypeDef __ad9910_hspi;
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

