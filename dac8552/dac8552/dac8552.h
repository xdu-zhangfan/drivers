#ifndef DAC8552
#define DAC8552

#include "main.h"

// for channel selection, independent codes
#define DAC8552_CHSEL_A 0
#define DAC8552_CHSEL_B 1

// for output impedance power down selection, independent codes
#define DAC8552_OUTIMP_PDN_NONE 0
#define DAC8552_OUTIMP_PDN_10K 1
#define DAC8552_OUTIMP_PDN_100K 2
#define DAC8552_OUTIMP_PDN_HZ 3
typedef struct
{
    SPI_HandleTypeDef *dac8552_hspi; // HAL SPI interface of DAC8552

    unsigned char ch_sel;         // channel selection, masked
    unsigned char out_imp_pdn;    // output impedance, independent
    unsigned short int out_value; // output value, effective when [out_imp_pdn == 0]
} dac8552_HandleTypeDef;

int dac8552_init(dac8552_HandleTypeDef *dev_handle, SPI_HandleTypeDef *dev_hspi);
int dac8552_set_value(dac8552_HandleTypeDef *dev_handle);

#endif