# wave_generator

## 文件夹结构
```
DAC-test/wave_generator                     
DAC-test/wave_generator/wave_generator.c    函数源文件
DAC-test/wave_generator/wave_generator.h    基本宏定义
```

## 使用方法（Makefile工程）
### 1、复制本文件夹到工程根目录
### 2、更改Makefile：
```
# 更改源文件列表
C_SOURCES =  \
Core/Src/main.c \
......
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c \
ad5689/ad5689.c ## 需要添加的内容
```
```
# 更改包含文件目录列表
C_INCLUDES =  \
-ICore/Inc \
......
-IDrivers/CMSIS/DSP/Include \
-ad5689 ## 需要添加的内容
```

### 3、在需要的地方包含 ad5689.h
```
#include "ad5689.h"
```

## STM32CubeMX更改
需要设置一个LDAC和RST引脚。

## 函数使用说明
```
int ad5689_init(
    //Configure spi interface
    SPI_HandleTypeDef *ad5689_hspi_i,
    //LDAC GPIO
    GPIO_TypeDef *ad5689_ldac_gpio_group_i,
    uint16_t ad5689_ldac_gpio_pin_i,
    //RESET GPIO 
    GPIO_TypeDef *ad5689_rst_gpio_i,
    uint16_t    ad5689_rst_gpio_pin_i
);
```
```
uint32_t ad5689_set_dac( uint32_t data , uint8_t dac_ch , uint8_t write_flag ,uint8_t update_flag);
uint32_t data：将要写入DAC的数据（低16位有效）
uint8_t dac_ch：通道选择，掩码值AD5689_DAC_CH_A_ADDR、AD5689_DAC_CH_B_ADDR
uint8_t write_flag：写入标志，0为不写入，1为写入
uint8_t update_flag：更新标志，0为不更新，1为更新
```
