#include "dac8552.h"

// Initialize DAC8552
int dac8552_init(dac8552_HandleTypeDef *dev_handle, SPI_HandleTypeDef *dev_hspi)
{
    if (dev_handle == NULL)
    {
        return -1;
    }

    memset(dev_handle, 0, sizeof(dac8552_HandleTypeDef));
    dev_handle->dac8552_hspi = dev_hspi;

    return 0;
}

// Abstract HAL library to DAC8552 driver space
static uint32_t dac8552_send_recv_data(dac8552_HandleTypeDef *dev_handle, uint32_t tx_data)
{
    uint32_t rx_data;
    HAL_SPI_TransmitReceive(dev_handle->dac8552_hspi, (uint8_t *)&tx_data, (uint8_t *)&rx_data, 1, HAL_MAX_DELAY);
    return rx_data;
}

// send DAC value to DAC8552
int dac8552_set_value(dac8552_HandleTypeDef *dev_handle)
{
    dac8552_send_recv_data(dev_handle, (unsigned int)(((1 << (dev_handle->ch_sel) << 20) |
                                                       (dev_handle->ch_sel << 18) |
                                                       (dev_handle->out_imp_pdn << 16) |
                                                       (dev_handle->out_value))));

    return 0;
}