# Si5351 (Copied from https://github.com/MR-DOS/Si5351-lib/tree/master)

## 文件夹结构
```
si5351/si5351
si5351/si5351/si5351.c    函数源文件
si5351/si5351/si5351.h    基本宏定义
```


## 使用方法（Makefile工程）
### 1、复制本文件夹到工程根目录
### 2、更改Makefile：
```
# 更改源文件列表
C_SOURCES =  \
Core/Src/main.c \
......
Core/Src/system_stm32h7xx.c \
si5351/si5351.c ## 需要添加的内容
```
```
# 更改包含文件目录列表
C_INCLUDES =  \
-ICore/Inc \
......
-IDrivers/CMSIS/Include \
-Isi5351 ## 需要添加的内容
```

### 3、在需要的地方包含wave_generator.h
```
#include "si5351.h"
```

## 函数使用说明