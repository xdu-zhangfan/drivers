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
wave_generator/wave_generator.c ## 需要添加的内容
```
```
# 更改包含文件目录列表
C_INCLUDES =  \
-ICore/Inc \
......
-IDrivers/CMSIS/DSP/Include \
-Iwave_generator ## 需要添加的内容
```
```
# 添加DSP目录和编译选项

## 原内容
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

## 更改后内容
LIBS = -lc -lm -lnosys -larm_cortexM7lfdp_math ## Cortex-M7小端序、硬浮点单元、双浮点精度
LIBDIR = -LDrivers/CMSIS/DSP/Lib/GCC ## 选择GCC的二进制库
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
```

### 3、在需要的地方包含wave_generator.h
```
#include "wave_generator.h"
```

## 函数使用说明
```
本函数用于生成缓冲区的第i个数据。建议采样率除以缓冲区长度的结果是目标正弦波频率的整数倍（如缓冲区长度为1000，采样率为100kHz，则将正弦波设置为100000/1000=100的整数倍，比如300、500、700、2000），否则将会产生正弦波截断现象。
double wavegen_sinusoid_i(unsigned int buffer_point_num, double dc_offset, double amplitude, double initial_phase,
                          unsigned int sample_rate, unsigned int wave_freq, int i);
unsigned int buffer_point_num：缓冲区长度（点数，如果是int buf[4096]的话，应填入4096，而不是4096*sizeof(int)）
double dc_offset：直流偏置
double amplitude：正弦波幅度
double initial_phase：初始相位
unsigned int sample_rate：DAC对缓冲区的采样率
unsigned int wave_freq：目的正弦波频率
int i：对应缓冲区的第i个数据（缓冲区下标索引）
```
```
本函数通过调用 wavegen_sinusoid_i，在缓冲区填入正弦波数据。建议采样率除以缓冲区长度的结果是目标正弦波频率的整数倍（如缓冲区长度为1000，采样率为100kHz，则将正弦波设置为100000/1000=100的整数倍，比如300、500、700、2000），否则将会产生正弦波截断现象。
int wavegen_sinusoid(unsigned int *dst_buffer, unsigned int buffer_point_num, unsigned int dc_offset, unsigned int amplitude, double initial_phase,
                     unsigned int sample_rate, unsigned int wave_freq);
unsigned int *dst_buffer：缓冲区地址
unsigned int buffer_point_num：缓冲区长度（点数，如果是int buf[4096]的话，应填入4096，而不是4096*sizeof(int)）
double dc_offset：直流偏置
double amplitude：正弦波幅度
double initial_phase：初始相位
unsigned int sample_rate：DAC对缓冲区的采样率
unsigned int wave_freq：目的正弦波频率
```