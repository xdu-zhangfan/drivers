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