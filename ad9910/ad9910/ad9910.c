//-----------------------------------------------------------------
// 程序描述: 	AD9910串行程序
// 作　　者: 凌智电子
// 开始日期: 2020-05-01
// 完成日期: 2020-05-05
// 当前版本: V1.0
// 历史版本:
// 调试工具: 凌智STM32核心开发板、LZE_ST_LINK2
// 说　　明:
//-----------------------------------------------------------------
#include "ad9910.h"

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
static GPIO_TypeDef *__ad9910_sck_gpio_group;
static uint16_t __ad9910_sck_gpio_pin;
static GPIO_TypeDef *__ad9910_sdi_gpio_group;
static uint16_t __ad9910_sdi_gpio_pin;
static GPIO_TypeDef *__ad9910_csn_gpio_group;
static uint16_t __ad9910_csn_gpio_pin;

u8 Profile_All[8]; // Profile的暂存位置

u8 CFR3[] = {0x05, 0x0F, 0x41, 0x50};		// CFR3控制功能寄存器3   时钟倍频40倍  基础时钟25M   最终时钟=1 000 000 000 Hz
u8 Assist_DAC[] = {0x00, 0x00, 0x00, 0x7F}; // 辅助DAC输出  默认0x7f

u8 DRL[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 数字斜坡限值				0x0B
u8 DRS[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 数字斜坡步长				0x0C
u8 DRR[] = {0x00, 0x00, 0x00, 0x00};						 // 数字斜坡速率				0x0D

u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 开启AD9910反Sinc滤波器
u8 CFR2[] = {0x00, 0x40, 0x08, 0x30}; // 并行端口使能 其中当CFR2[3]=0x30时，并行口的频率位设置频率*2^0  , 当 CFR2[3]=0x33时，并行口的频率位设置频率*2^3;

u8 FTW[] = {0x00, 0x00, 0xa7, 0xc6};
u8 ASF[] = {0x00, 0x00, 0x00, 0x00};
//---------------------------------------------------------------------------------------------------------------------------------
// 说明：AD9910 RAM 模式 1024*32位数据数组（这个数组的值自己可以定义，这边我设置了0~23 24~273 274~523 524~773 774~1023 共用到了5个Profile）
//																				（数组的地址1023~0，反过来的，因为是串行写入的）
//																				（起始地址是最后一个，终止地址是第一个）
u32 RAM_AMP[] = // 这是给幅度用的RAM表（熟悉可自己设计, 以下幅度值是大约理论值）
	{
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, // 744mv
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000, 0x7fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, // 372mv
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, // 186mv
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000, 0x3fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, // 93mv
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000, 0x1fff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, // 744mv
		0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000};

//---------------------------------------------------------------------------------------------------------------------------------
// 说明：这是三角波的RAM值
//																				（数组的地址1023~0，反过来的，因为是串行写入的）
//																				（起始地址是最后一个，终止地址是第一个）
u32 RAM_AMP_TRIG_WAVE[] =
	{
		255, 510, 765, 1020, 1275, 1530, 1785, 2040, 2295, 2550, 2805, 3060, 3315, 3570, 3825, 4080, 4335, 4590, 4845, 5100, 5355, 5610, 5865, 6120, 6375,
		6630, 6885, 7140, 7395, 7650, 7905, 8160, 8415, 8670, 8925, 9180, 9435, 9690, 9945, 10200, 10455, 10710, 10965, 11220, 11475, 11730, 11985, 12240,
		12495, 12750, 13005, 13260, 13515, 13770, 14025, 14280, 14535, 14790, 15045, 15300, 15555, 15810, 16065, 16320, 16575, 16830, 17085, 17340,
		17595, 17850, 18105, 18360, 18615, 18870, 19125, 19380, 19635, 19890, 20145, 20400, 20655, 20910, 21165, 21420, 21675, 21930, 22185, 22440,
		22695, 22950, 23205, 23460, 23715, 23970, 24225, 24480, 24735, 24990, 25245, 25500, 25755, 26010, 26265, 26520, 26775, 27030, 27285, 27540,
		27795, 28050, 28305, 28560, 28815, 29070, 29325, 29580, 29835, 30090, 30345, 30600, 30855, 31110, 31365, 31620, 31875, 32130, 32385, 32640,
		32895, 33150, 33405, 33660, 33915, 34170, 34425, 34680, 34935, 35190, 35445, 35700, 35955, 36210, 36465, 36720, 36975, 37230, 37485, 37740,
		37995, 38250, 38505, 38760, 39015, 39270, 39525, 39780, 40035, 40290, 40545, 40800, 41055, 41310, 41565, 41820, 42075, 42330, 42585, 42840,
		43095, 43350, 43605, 43860, 44115, 44370, 44625, 44880, 45135, 45390, 45645, 45900, 46155, 46410, 46665, 46920, 47175, 47430, 47685, 47940,
		48195, 48450, 48705, 48960, 49215, 49470, 49725, 49980, 50235, 50490, 50745, 51000, 51255, 51510, 51765, 52020, 52275, 52530, 52785, 53040,
		53295, 53550, 53805, 54060, 54315, 54570, 54825, 55080, 55335, 55590, 55845, 56100, 56355, 56610, 56865, 57120, 57375, 57630, 57885, 58140,
		58395, 58650, 58905, 59160, 59415, 59670, 59925, 60180, 60435, 60690, 60945, 61200, 61455, 61710, 61965, 62220, 62475, 62730, 62985, 63240,
		63495, 63750, 64005, 64260, 64515, 64770, 65025, 65280, 65532, 65025, 64770, 64515, 64260, 64005, 63750, 63495, 63240, 62985, 62730, 62475,
		62220, 61965, 61710, 61455, 61200, 60945, 60690, 60435, 60180, 59925, 59670, 59415, 59160, 58905, 58650, 58395, 58140, 57885, 57630, 57375,
		57120, 56865, 56610, 56355, 56100, 55845, 55590, 55335, 55080, 54825, 54570, 54315, 54060, 53805, 53550, 53295, 53040, 52785, 52530, 52275,
		52020, 51765, 51510, 51255, 51000, 50745, 50490, 50235, 49980, 49725, 49470, 49215, 48960, 48705, 48450, 48195, 47940, 47685, 47430, 47175,
		46920, 46665, 46410, 46155, 45900, 45645, 45390, 45135, 44880, 44625, 44370, 44115, 43860, 43605, 43350, 43095, 42840, 42585, 42330, 42075,
		41820, 41565, 41310, 41055, 40800, 40545, 40290, 40035, 39780, 39525, 39270, 39015, 38760, 38505, 38250, 37995, 37740, 37485, 37230, 36975,
		36720, 36465, 36210, 35955, 35700, 35445, 35190, 34935, 34680, 34425, 34170, 33915, 33660, 33405, 33150, 32895, 32640, 32385, 32130, 31875,
		31620, 31365, 31110, 30855, 30600, 30345, 30090, 29835, 29580, 29325, 29070, 28815, 28560, 28305, 28050, 27795, 27540, 27285, 27030, 26775,
		26520, 26265, 26010, 25755, 25500, 25245, 24990, 24735, 24480, 24225, 23970, 23715, 23460, 23205, 22950, 22695, 22440, 22185, 21930, 21675,
		21420, 21165, 20910, 20655, 20400, 20145, 19890, 19635, 19380, 19125, 18870, 18615, 18360, 18105, 17850, 17595, 17340, 17085, 16830, 16575,
		16320, 16065, 15810, 15555, 15300, 15045, 14790, 14535, 14280, 14025, 13770, 13515, 13260, 13005, 12750, 12495, 12240, 11985, 11730, 11475,
		11220, 10965, 10710, 10455, 10200, 9945, 9690, 9435, 9180, 8925, 8670, 8415, 8160, 7905, 7650, 7395, 7140, 6885, 6630, 6375, 6120, 5865, 5610,
		5355, 5100, 4845, 4590, 4335, 4080, 3825, 3570, 3315, 3060, 2805, 2550, 2295, 2040, 1785, 1530, 1275, 1020, 765, 510, 255,

		2147418112, 2147418367, 2147418622, 2147418877, 2147419132, 2147419387, 2147419642, 2147419897, 2147420152, 2147420407, 2147420662,
		2147420917, 2147421172, 2147421427, 2147421682, 2147421937, 2147422192, 2147422447, 2147422702, 2147422957, 2147423212, 2147423467,
		2147423722, 2147423977, 2147424232, 2147424487, 2147424742, 2147424997, 2147425252, 2147425507, 2147425762, 2147426017, 2147426272,
		2147426527, 2147426782, 2147427037, 2147427292, 2147427547, 2147427802, 2147428057, 2147428312, 2147428567, 2147428822, 2147429077,
		2147429332, 2147429587, 2147429842, 2147430097, 2147430352, 2147430607, 2147430862, 2147431117, 2147431372, 2147431627, 2147431882,
		2147432137, 2147432392, 2147432647, 2147432902, 2147433157, 2147433412, 2147433667, 2147433922, 2147434177, 2147434432, 2147434687,
		2147434942, 2147435197, 2147435452, 2147435707, 2147435962, 2147436217, 2147436472, 2147436727, 2147436982, 2147437237, 2147437492,
		2147437747, 2147438002, 2147438257, 2147438512, 2147438767, 2147439022, 2147439277, 2147439532, 2147439787, 2147440042, 2147440297,
		2147440552, 2147440807, 2147441062, 2147441317, 2147441572, 2147441827, 2147442082, 2147442337, 2147442592, 2147442847, 2147443102,
		2147443357, 2147443612, 2147443867, 2147444122, 2147444377, 2147444632, 2147444887, 2147445142, 2147445397, 2147445652, 2147445907,
		2147446162, 2147446417, 2147446672, 2147446927, 2147447182, 2147447437, 2147447692, 2147447947, 2147448202, 2147448457, 2147448712,
		2147448967, 2147449222, 2147449477, 2147449732, 2147449987, 2147450242, 2147450497, 2147450752, 2147451007, 2147451262, 2147451517,
		2147451772, 2147452027, 2147452282, 2147452537, 2147452792, 2147453047, 2147453302, 2147453557, 2147453812, 2147454067, 2147454322,
		2147454577, 2147454832, 2147455087, 2147455342, 2147455597, 2147455852, 2147456107, 2147456362, 2147456617, 2147456872, 2147457127,
		2147457382, 2147457637, 2147457892, 2147458147, 2147458402, 2147458657, 2147458912, 2147459167, 2147459422, 2147459677, 2147459932,
		2147460187, 2147460442, 2147460697, 2147460952, 2147461207, 2147461462, 2147461717, 2147461972, 2147462227, 2147462482, 2147462737,
		2147462992, 2147463247, 2147463502, 2147463757, 2147464012, 2147464267, 2147464522, 2147464777, 2147465032, 2147465287, 2147465542,
		2147465797, 2147466052, 2147466307, 2147466562, 2147466817, 2147467072, 2147467327, 2147467582, 2147467837, 2147468092, 2147468347,
		2147468602, 2147468857, 2147469112, 2147469367, 2147469622, 2147469877, 2147470132, 2147470387, 2147470642, 2147470897, 2147471152,
		2147471407, 2147471662, 2147471917, 2147472172, 2147472427, 2147472682, 2147472937, 2147473192, 2147473447, 2147473702, 2147473957,
		2147474212, 2147474467, 2147474722, 2147474977, 2147475232, 2147475487, 2147475742, 2147475997, 2147476252, 2147476507, 2147476762,
		2147477017, 2147477272, 2147477527, 2147477782, 2147478037, 2147478292, 2147478547, 2147478802, 2147479057, 2147479312, 2147479567,
		2147479822, 2147480077, 2147480332, 2147480587, 2147480842, 2147481097, 2147481352, 2147481607, 2147481862, 2147482117, 2147482372,
		2147482627, 2147482882, 2147483137, 2147483392, 2147483137, 2147482882, 2147482627, 2147482372, 2147482117, 2147481862, 2147481607,
		2147481352, 2147481097, 2147480842, 2147480587, 2147480332, 2147480077, 2147479822, 2147479567, 2147479312, 2147479057, 2147478802,
		2147478547, 2147478292, 2147478037, 2147477782, 2147477527, 2147477272, 2147477017, 2147476762, 2147476507, 2147476252, 2147475997,
		2147475742, 2147475487, 2147475232, 2147474977, 2147474722, 2147474467, 2147474212, 2147473957, 2147473702, 2147473447, 2147473192,
		2147472937, 2147472682, 2147472427, 2147472172, 2147471917, 2147471662, 2147471407, 2147471152, 2147470897, 2147470642, 2147470387,
		2147470132, 2147469877, 2147469622, 2147469367, 2147469112, 2147468857, 2147468602, 2147468347, 2147468092, 2147467837, 2147467582,
		2147467327, 2147467072, 2147466817, 2147466562, 2147466307, 2147466052, 2147465797, 2147465542, 2147465287, 2147465032, 2147464777,
		2147464522, 2147464267, 2147464012, 2147463757, 2147463502, 2147463247, 2147462992, 2147462737, 2147462482, 2147462227, 2147461972,
		2147461717, 2147461462, 2147461207, 2147460952, 2147460697, 2147460442, 2147460187, 2147459932, 2147459677, 2147459422, 2147459167,
		2147458912, 2147458657, 2147458402, 2147458147, 2147457892, 2147457637, 2147457382, 2147457127, 2147456872, 2147456617, 2147456362,
		2147456107, 2147455852, 2147455597, 2147455342, 2147455087, 2147454832, 2147454577, 2147454322, 2147454067, 2147453812, 2147453557,
		2147453302, 2147453047, 2147452792, 2147452537, 2147452282, 2147452027, 2147451772, 2147451517, 2147451262, 2147451007, 2147450752,
		2147450497, 2147450242, 2147449987, 2147449732, 2147449477, 2147449222, 2147448967, 2147448712, 2147448457, 2147448202, 2147447947,
		2147447692, 2147447437, 2147447182, 2147446927, 2147446672, 2147446417, 2147446162, 2147445907, 2147445652, 2147445397, 2147445142,
		2147444887, 2147444632, 2147444377, 2147444122, 2147443867, 2147443612, 2147443357, 2147443102, 2147442847, 2147442592, 2147442337,
		2147442082, 2147441827, 2147441572, 2147441317, 2147441062, 2147440807, 2147440552, 2147440297, 2147440042, 2147439787, 2147439532,
		2147439277, 2147439022, 2147438767, 2147438512, 2147438257, 2147438002, 2147437747, 2147437492, 2147437237, 2147436982, 2147436727,
		2147436472, 2147436217, 2147435962, 2147435707, 2147435452, 2147435197, 2147434942, 2147434687, 2147434432, 2147434177, 2147433922,
		2147433667, 2147433412, 2147433157, 2147432902, 2147432647, 2147432392, 2147432137, 2147431882, 2147431627, 2147431372, 2147431117,
		2147430862, 2147430607, 2147430352, 2147430097, 2147429842, 2147429587, 2147429332, 2147429077, 2147428822, 2147428567, 2147428312,
		2147428057, 2147427802, 2147427547, 2147427292, 2147427037, 2147426782, 2147426527, 2147426272, 2147426017, 2147425762, 2147425507,
		2147425252, 2147424997, 2147424742, 2147424487, 2147424232, 2147423977, 2147423722, 2147423467, 2147423212, 2147422957, 2147422702,
		2147422447, 2147422192, 2147421937, 2147421682, 2147421427, 2147421172, 2147420917, 2147420662, 2147420407, 2147420152, 2147419897,
		2147419642, 2147419387, 2147419132, 2147418877, 2147418622, 2147418367

};
//---------------------------------------------------------------------------------------------------------------------------------
// 说明：这是方波的RAM值
//																				（数组的地址1023~0，反过来的，因为是串行写入的）
//																				（起始地址是最后一个，终止地址是第一个）
u32 RAM_AMP_SQUARE_WAVE[] =
	{

		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,
		0x0000fffc,

		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,
		0x7ffffffc,

};
// FTW = 2^32 * Fout / Fsys
u32 RAM_Fre[] = // 这是给频率用的RAM表（熟悉可自己设计）
	{
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, // 10M
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29, 0x28f5c29,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, // 8M
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba, 0x20c49ba,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, // 6M
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c, 0x189374c,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, // 4M
		0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd, 0x10624dd,
		0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, // 2M
		0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e, 0x83126e,
		0x83126e, 0x83126e, 0x83126e, 0x83126e};

u32 RAM_PHA[] =
	{
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000, 0xFF4A0000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
		0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000,
		0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000,
		0x00B60000, 0x00B60000, 0x00B60000, 0x00B60000};

//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
// 初始化程序区
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
// void GPIO_Init_AD9910(void)
// 函数功能: AD9910引脚配置函数
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无
// 注意事项: 无
//---------------------------------------------------------------------------------------------------------------------------------
void GPIO_Init_AD9910(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB,GPIOA,GPIOE
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void GPIO_Parallel_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能GPIOB,GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void Write_8bit(u8 dat)
// 函数功能:AD9910串行口写入数据
// 入口参数:dat      8位数据
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void Write_8bit(u8 dat)
{
	u8 i, com;
	com = 0x80;
	AD9910_SCK_Clr;
	for (i = 0; i < 8; i++)
	{
		if ((dat & com) == 0)
			AD9910_SDI_Clr;
		else
			AD9910_SDI_Set;
		AD9910_SCK_Set;
		com = com >> 1;
		AD9910_SCK_Clr;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void Write_32bit(u32 dat)
// 函数功能:AD9910串行口写入数据
// 入口参数:dat      32位数据
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void Write_32bit(u32 dat)
{
	u8 i;
	u32 com;
	com = 0x80000000;
	AD9910_SCK_Clr;

	for (i = 0; i < 32; i++)
	{
		if ((dat & com) == 0)
			AD9910_SDI_Clr;
		else
			AD9910_SDI_Set;

		AD9910_SCK_Set;
		com = com >> 1;
		AD9910_SCK_Clr;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Init(void)
// 函数功能:AD9910时钟的设置和辅助DAC设置 以及AD9910复位
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Init(ad9910_PortSettingsTypeDef *ports)
{
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

	int j;
	AD9910_MRT_Set; // MRT置1
	AD9910_MRT_Clr; // MRT置0     复位AD9910

	AD9910_CSN_Clr;	  // CSN置0			开片选
	Write_8bit(0x02); // 选择0x02地址            CFR3寄存器写入数据		CFR3是控制时钟倍频之类的寄存器
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR3[j]);
	}
	AD9910_CSN_Set; // CSN置1			关片选
	Delay_ns(10);

	AD9910_CSN_Clr;	  // CSN置0			开片选
	Write_8bit(0x03); // 选择0x03地址            辅助DAC控制写入数据	 控制DAC输出的幅度
	for (j = 0; j < 4; j++)
	{
		Write_8bit(Assist_DAC[j]);
	}
	AD9910_CSN_Set; // CSN置1			关片选

	Delay_ns(10);
	AD9910_IUP_Clr; // 更新AD9910输出
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Singal_Profile_Init(void)
// 函数功能:AD9910单频Profile初始化
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Singal_Profile_Init(void)
{
	int j;

	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 开启AD9910反Sinc滤波器
	u8 CFR2[] = {0x01, 0x40, 0x08, 0x20}; // 使能单频Profiles调制幅度 （SYNC_CLK使能	PDCLK使能		同步时序验证禁用——这些是默认的）

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将单频模式的CFR1[]写入CFR1寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将单频模式的CFR2[]写入CFR2寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910输出
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Parallel_Profile_Init(void)
// 函数功能:AD9910单频Profile初始化
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Parallel_Profile_Init(void)
{
	int j;

	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 开启AD9910反Sinc滤波器
	u8 CFR2[] = {0x01, 0x40, 0x08, 0x30}; // 使能单频Profiles调制幅度 （SYNC_CLK使能	PDCLK使能		同步时序验证禁用——这些是默认的）

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将单频模式的CFR1[]写入CFR1寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将单频模式的CFR2[]写入CFR2寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910输出
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Parallel_Profile_Set(u8 addr,u32 Freq,u16 Amp ,u16 Pha)
// 函数功能:AD9910单频Profile通道、频率、幅度、相位设置
// 入口参数:		addr							Profile通道的选择								(0--7)					总共有8个Profile
//						Freq							单频中的频率的设置					 (1Hz--450MHz)
//						Amp								单频中的幅度设置						 (0x0000--0x3FFF)
//						Pha								单频中的相位设置							(0--360°)
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Parallel_Profile_Set()
{
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Singal_Profile_Set(u8 addr,u32 Freq,u16 Amp ,u16 Pha)
// 函数功能:AD9910单频Profile通道、频率、幅度、相位设置
// 入口参数:		addr							Profile通道的选择								(0--7)					总共有8个Profile
//						Freq							单频中的频率的设置					 (1Hz--450MHz)
//						Amp								单频中的幅度设置						 (0x0000--0x3FFF)
//						Pha								单频中的相位设置							(0--360°)
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Singal_Profile_Set(u8 addr, u32 Freq, u16 Amp, u16 Pha)
{
	u32 Temp_Fre, Temp_Amp, Temp_Pha;
	u8 Temp_addr;
	int k;
	Temp_Fre = (u32)Freq * 4.294967296;	 // 频率对应上频率的FTW		4.294967296=(2^32)/1000000000		设置的范围是（1Hz~450MHz）
	Temp_Pha = (u16)Pha * 182.044444444; // 相位对应上相位的POW		182.044444444=2^16/360					设置的范围是（0~360°）
	Temp_Amp = Amp;						 // 因为幅度无法准确的对应上  	所以没有转换 								设置的范围是（0x0000~0x3FFF）

	switch (addr) // 选择要写入的通道（通道0~7）
	{
	case 0:
		Temp_addr = 0x0e;
		break;
	case 1:
		Temp_addr = 0x0f;
		break;
	case 2:
		Temp_addr = 0x10;
		break;
	case 3:
		Temp_addr = 0x11;
		break;
	case 4:
		Temp_addr = 0x12;
		break;
	case 5:
		Temp_addr = 0x13;
		break;
	case 6:
		Temp_addr = 0x14;
		break;
	case 7:
		Temp_addr = 0x15;
		break;
	default:
		break;
	}

	Profile_All[7] = (u8)Temp_Fre; // 将频率幅度相位的值存入暂存地址——Profile_All
	Profile_All[6] = (u8)(Temp_Fre >> 8);
	Profile_All[5] = (u8)(Temp_Fre >> 16);
	Profile_All[4] = (u8)(Temp_Fre >> 24);
	Profile_All[3] = (u8)Temp_Pha;
	Profile_All[2] = (u8)(Temp_Pha >> 8);
	Profile_All[1] = (u8)Temp_Amp;
	Profile_All[0] = (u8)(Temp_Amp >> 8);

	AD9910_CSN_Clr;
	Write_8bit(Temp_addr); // 将暂存地址的值写入 Profile地址
	for (k = 0; k < 8; k++)
	{
		Write_8bit(Profile_All[k]);
	}
	AD9910_CSN_Set; // 更新AD9910输出

	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void Set_Profile(u8 num)
// 函数功能:AD9910选择Profile
// 入口参数:		num					(0--7)
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void Set_Profile(u8 num)
{
	switch (num)
	{
	case 0:
	{
		AD9910_PF2_Clr; // Profile 0     引脚PF2~PF0——000
		AD9910_PF1_Clr;
		AD9910_PF0_Clr;
	}
	break;
	case 1:
	{
		AD9910_PF2_Clr; // Profile 1     引脚PF2~PF0——001
		AD9910_PF1_Clr;
		AD9910_PF0_Set;
	}
	break;
	case 2:
	{
		AD9910_PF2_Clr; // Profile 2     引脚PF2~PF0——010
		AD9910_PF1_Set;
		AD9910_PF0_Clr;
	}
	break;
	case 3:
	{
		AD9910_PF2_Clr; // Profile 3     引脚PF2~PF0——011
		AD9910_PF1_Set;
		AD9910_PF0_Set;
	}
	break;
	case 4:
	{
		AD9910_PF2_Set; // Profile 4     引脚PF2~PF0——100
		AD9910_PF1_Clr;
		AD9910_PF0_Clr;
	}
	break;
	case 5:
	{
		AD9910_PF2_Set; // Profile 5     引脚PF2~PF0——101
		AD9910_PF1_Clr;
		AD9910_PF0_Set;
	}
	break;
	case 6:
	{
		AD9910_PF2_Set; // Profile 6     引脚PF2~PF0——110
		AD9910_PF1_Set;
		AD9910_PF0_Clr;
	}
	break;
	case 7:
	{
		AD9910_PF2_Set; // Profile 7     引脚PF2~PF0——111
		AD9910_PF1_Set;
		AD9910_PF0_Set;
	}
	break;
	default:
		break;
	}
	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Osk_Init(void)
// 函数功能:AD9910的OSK初始化
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Osk_Init(void)
{
	u8 CFR1[] = {0x00, 0x40, 0x03, 0x00}; // 	反Sinc滤波使能，OSK使能，选择自动OSK
	u8 CFR2[] = {0x00, 0x40, 0x08, 0x20}; // （SYNC_CLK使能	PDCLK使能		同步时序验证禁用）默认值

	int j;

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将OSK模式的CFR1[]写入CFR1寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将OSK模式的CFR2[]写入CFR2寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_Osk_Set(void)
// 函数功能:AD9910的OSK设置
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_Osk_Set(void)
{
	u8 ASF[] = {0x0f, 0xff, 0xff, 0xf0}; // 振幅比例因子的设置

	int j;
	AD9910_CSN_Clr;
	Write_8bit(0x09); // 将OSK模式的振幅比例因子写入振幅比例因子寄存器地址
	for (j = 0; j < 4; j++)
	{
		Write_8bit(ASF[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_DRG_Fre_Init(void)
// 函数功能:AD9910的数字斜坡模式DRG初始化------------------频率
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_DRG_Fre_Init(void)
{
	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 使能AD9910反Sinc滤波
	u8 CFR2[] = {0x00, 0x48, 0x08, 0x20}; // SYNC_CLK使能，数字斜坡使能	  CFR2[21:20]= 00   选择控制参数为频率
	int j;

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将CFR2写入其地址0x01
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_DRG_Freq_set(u32 upper_limit , u32 lower_limit ,u32 dec_step , u32 inc_step , u16 neg_rate ,u16 pos_rate)
// 函数功能:AD9910的DRG设置------------------频率
// 入口参数:		upper_limit										上限频率							1Hz---450M
//						lower_limit										下限频率							1Hz---450M       (上限频率要大于下限频率)
//						dec_step											频率加步进						1Hz---450M
//						inc_step											频率减步进						1Hz---450M
//						neg_rate											频率加的速率(时间)	0x0000----0xffff
//						pos_rate											频率减的速率(时间)	0x0000----0xffff
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_DRG_Freq_set(u32 upper_limit, u32 lower_limit, u32 dec_step, u32 inc_step, u16 neg_rate, u16 pos_rate)
{
	u32 upper_temp, lower_temp, dec_temp, inc_temp;
	u16 neg_rate_temp, pos_rate_temp;
	int i;
	upper_temp = (u32)upper_limit * 4.294967296; // 频率字转换		4.294967296=(2^32)/1000000000
	lower_temp = (u32)lower_limit * 4.294967296;
	dec_temp = (u32)dec_step * 4.294967296;
	inc_temp = (u32)inc_step * 4.294967296;
	neg_rate_temp = neg_rate; // 时间
	pos_rate_temp = pos_rate;

	DRL[7] = (u8)lower_temp; // 将上限下限频率、频率加减步进、频率加减速率写入数组，后写入相应的寄存器
	DRL[6] = (u8)(lower_temp >> 8);
	DRL[5] = (u8)(lower_temp >> 16);
	DRL[4] = (u8)(lower_temp >> 24);
	DRL[3] = (u8)upper_temp;
	DRL[2] = (u8)(upper_temp >> 8);
	DRL[1] = (u8)(upper_temp >> 16);
	DRL[0] = (u8)(upper_temp >> 24);

	DRS[7] = (u8)inc_temp;
	DRS[6] = (u8)(inc_temp >> 8);
	DRS[5] = (u8)(inc_temp >> 16);
	DRS[4] = (u8)(inc_temp >> 24);
	DRS[3] = (u8)dec_temp;
	DRS[2] = (u8)(dec_temp >> 8);
	DRS[1] = (u8)(dec_temp >> 16);
	DRS[0] = (u8)(dec_temp >> 24);

	DRR[3] = (u8)pos_rate_temp;
	DRR[2] = (u8)(pos_rate_temp >> 8);
	DRR[1] = (u8)neg_rate_temp;
	DRR[0] = (u8)(neg_rate_temp >> 8);

	AD9910_CSN_Clr;
	Write_8bit(0x0B); // 将数字斜坡限值存入 0x0B
	for (i = 0; i < 8; i++)
	{
		Write_8bit(DRL[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0C); // 将数字斜坡步长存入 0x0C
	for (i = 0; i < 8; i++)
	{
		Write_8bit(DRS[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0D); // 将数字斜坡速率存入 0x0D
	for (i = 0; i < 4; i++)
	{
		Write_8bit(DRR[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_DRG_AMP_Init(void)
// 函数功能:AD9910的DRG初始化------------------幅度
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_DRG_AMP_Init(void)
{
	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 使能AD9910反Sinc滤波
	u8 CFR2[] = {0x01, 0x69, 0x08, 0x20}; // SYNC_CLK使能，数字斜坡使能	  CFR2[21:20]= 10   选择控制参数为幅度
	int j;

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将CFR2写入其地址0x01
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_DRG_Amp_Set( u32 upper_limit , u32 lower_limit ,u32 dec_step , u32 inc_step , u16 neg_rate ,u16 pos_rate)
// 函数功能:AD9910的DRG设置------------------幅度
// 入口参数:		upper_limit										上限幅度							0x00000000-----0xFFFFFC00				幅度值取高14位
//						lower_limit										下限幅度							0x00000000-----0xFFFFFC00      (上限幅度要大于下限幅度)
//						dec_step											幅度加步进						0x00000000-----0xFFFFFC00
//						inc_step											幅度减步进						0x00000000-----0xFFFFFC00
//						neg_rate											幅度加的速率(时间)		0x0000----0xffff
//						pos_rate											幅度减的速率(时间)		0x0000----0xffff
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_DRG_Amp_Set(u32 upper_limit, u32 lower_limit, u32 dec_step, u32 inc_step, u16 neg_rate, u16 pos_rate)
{
	int i;
	u32 upper_temp, lower_temp, dec_temp, inc_temp;
	u16 neg_rate_temp, pos_rate_temp;

	upper_temp = upper_limit;
	lower_temp = lower_limit;
	dec_temp = dec_step;
	inc_temp = inc_step;
	neg_rate_temp = neg_rate; // 时间
	pos_rate_temp = pos_rate;

	// 将上限下限幅度、幅度加减步进、幅度加减速率写入数组，后写入相应的寄存器
	DRL[7] = 0;
	DRL[6] = 0;
	DRL[5] = (u8)(lower_temp << 2);
	DRL[4] = (u8)(lower_temp >> 6);
	DRL[3] = 0;
	DRL[2] = 0;
	DRL[1] = (upper_temp << 2);
	DRL[0] = (upper_temp >> 6);

	DRS[7] = 0;
	DRS[6] = 0;
	DRS[5] = (u8)(inc_temp << 2);
	DRS[4] = (u8)(inc_temp >> 6);
	DRS[3] = 0;
	DRS[2] = 0;
	DRS[1] = (u8)(dec_temp << 2);
	DRS[0] = (u8)(dec_temp >> 6);

	DRR[3] = (u8)pos_rate_temp;
	DRR[2] = (u8)(pos_rate_temp >> 8);
	DRR[1] = (u8)neg_rate_temp;
	DRR[0] = (u8)(neg_rate_temp >> 8);

	AD9910_CSN_Clr;
	Write_8bit(0x0B); // 将DRL(数字斜坡限值)写入 0x0B
	for (i = 0; i < 8; i++)
	{
		Write_8bit(DRL[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0C); // 将DRS(数字斜坡步长)写入 0x0C
	for (i = 0; i < 8; i++)
	{
		Write_8bit(DRS[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0D); // 将DRR(数字斜坡速率)写入 0x0D
	for (i = 0; i < 4; i++)
	{
		Write_8bit(DRR[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_DRG_Pha_Init(void)
// 函数功能:AD9910的DRG初始化------------------相位
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_DRG_Pha_Init(void)
{
	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 使能AD9910反Sinc滤波
	u8 CFR2[] = {0x01, 0x58, 0x08, 0x20}; // SYNC_CLK使能，数字斜坡使能	  CFR2[21:20]= 01   选择控制参数为相位
	int j;

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将CFR2写入其地址0x01
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_DRG_Pha_Set( u32 upper_limit , u32 lower_limit ,u32 dec_step , u32 inc_step , u16 neg_rate ,u16 pos_rate)
// 函数功能:AD9910的DRG设置------------------相位
// 入口参数:		upper_limit										上限相位							0~360°
//						lower_limit										下限相位							0~360°      (上限频率要大于下限频率)
//						dec_step											相位加步进						0~360°
//						inc_step											相位减步进						0~360°
//						neg_rate											相位加的速率(时间)		0x0000----0xffff
//						pos_rate											相位减的速率(时间)		0x0000----0xffff
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_DRG_Pha_Set(u32 upper_limit, u32 lower_limit, u32 dec_step, u32 inc_step, u16 neg_rate, u16 pos_rate)
{
	int i;
	u32 upper_temp, lower_temp, dec_temp, inc_temp;
	u16 neg_rate_temp, pos_rate_temp;

	upper_temp = (u32)upper_limit * 182.04; // 182.044=2^16/360  根据芯片手册的公式
	lower_temp = (u32)lower_limit * 182.04;
	dec_temp = (u32)dec_step * 182.04;
	inc_temp = (u32)inc_step * 182.04;

	upper_temp = upper_temp << 16;
	lower_temp = lower_temp << 16;
	dec_temp = dec_temp << 16;
	inc_temp = inc_temp << 16;
	neg_rate_temp = neg_rate; // 时间
	pos_rate_temp = pos_rate;

	DRL[7] = (u8)lower_temp;
	DRL[6] = (u8)(lower_temp >> 8); // 将上限下限幅度、幅度加减步进、幅度加减速率写入数组，后写入相应的寄存器

	DRL[3] = (u8)upper_temp;
	DRL[2] = (u8)(upper_temp >> 8);

	DRS[7] = (u8)inc_temp;
	DRS[6] = (u8)(inc_temp >> 8);
	;
	DRS[3] = (u8)dec_temp;
	DRS[2] = (u8)(dec_temp >> 8);

	DRR[3] = (u8)pos_rate_temp;
	DRR[2] = (u8)(pos_rate_temp >> 8);
	DRR[1] = (u8)neg_rate_temp;
	DRR[0] = (u8)(neg_rate_temp >> 8);

	AD9910_CSN_Clr;
	Write_8bit(0x0B); // 将DRL(数字斜坡限值)写入 0x0B
	for (i = 0; i < 8; i++)
	{
		Write_8bit(DRL[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0C); // 将DRS(数字斜坡步长)写入 0x0C
	for (i = 0; i < 8; i++)
	{
		Write_8bit(DRS[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0D); // 将DRR(数字斜坡速率)写入 0x0D
	for (i = 0; i < 4; i++)
	{
		Write_8bit(DRR[i]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_Init(void)
// 函数功能:将CFR1和CFR2恢复默认值
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_Init(void)
{
	int j;

	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00};
	u8 CFR2[] = {0x01, 0x40, 0x08, 0x20};
	u8 PRO[] = {0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将CFR2写入其寄存器0x01
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM Profile0写入其寄存器0x0e
	for (j = 0; j < 8; j++)
	{
		Write_8bit(PRO[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_ZB_Fre_Init(void)
// 函数功能:设置RAM模式中的载波的频率的初始化
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_ZB_Fre_Init(void)
{
	int j;

	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 开启AD9910反Sinc滤波功能
	u8 CFR2[] = {0x00, 0x41, 0x08, 0x20}; // 读取有效的FTW寄存器开启，FTW是写载波频率时用的

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将CFR2写入其寄存器0x01
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_ZB_Fre_Set(u32 Freq)
// 函数功能:设置RAM模式中的载波的频率
// 入口参数:Freq--------------------载波频率
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_ZB_Fre_Set(u32 Freq)
{
	u8 FTW[] = {0x00, 0x00, 0xa7, 0xc6};
	int j;
	u32 Temp;

	Temp = (u32)Freq * 4.294967296; // 将输入频率转化为相应的FTW值  4.294967296=(2^32)/1000000000

	FTW[3] = (u8)Temp;
	FTW[2] = (u8)(Temp >> 8);
	FTW[1] = (u8)(Temp >> 16);
	FTW[0] = (u8)(Temp >> 24);

	AD9910_CSN_Clr;	  // CSN置0			开片选
	Write_8bit(0x07); // 选择0x07地址           	将频率写入寄存器
	for (j = 0; j < 4; j++)
	{
		Write_8bit(FTW[j]);
	}
	AD9910_CSN_Set; // CSN置1			关片选
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910输出
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_Fre_W(void)
// 函数功能:将数据写入1024*32的RAM中---------------------------频率
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_Fre_W(void)
{
	int j;
	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00}; // 开启AD9910反Sinc滤波
	u8 CFR2[] = {0x00, 0x40, 0x08, 0x20};
	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0x00, 0x00, 0x00}; // 设置RAM的起始和终止地址、地址步进率

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x01); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr; // 将RAM的起始和终止地址、地址步进率写入相应的寄存器  0x0e
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新一下AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x16);
	for (j = 0; j < 1024; j++)
	{
		Write_32bit(RAM_Fre[j]); // 将频率RAM的内容写入0x16寄存器
	}
	AD9910_CSN_Set;

	AD9910_IUP_Clr; // 在更新一下AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_AMP_W(void)
// 函数功能:将数据写入1024*32的RAM中-------------------------幅度
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_AMP_W(void)
{
	int j;

	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00};							  // 开启AD9910反Sinc滤波
	u8 RAM_PRO0[] = {0x00, 0x00, 0xff, 0xff, 0xC0, 0x00, 0x00, 0x00}; // 设置RAM的起始和终止地址、地址步进率

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]); // 将RAM的起始和终止地址、地址步进率写入相应的寄存器  0x0e
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x16);
	for (j = 0; j < 1024; j++)
	{
		Write_32bit(RAM_AMP[j]); // 将幅度RAM的内容写入寄存器0x16
	}
	AD9910_CSN_Set;

	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_WAVE_RAM_AMP_W(int mode)
// 函数功能:将数据写入1024*32的RAM中-------------------------幅度
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_WAVE_RAM_AMP_W(int mode)
{
	int j;

	u8 CFR1[] = {0x60, 0x40, 0x00, 0x00};							  // 开启AD9910反Sinc滤波
	u8 RAM_PRO0[] = {0x00, 0x00, 0x01, 0xff, 0xC0, 0x00, 0x00, 0x00}; // 设置RAM的起始和终止地址、地址步进率

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]); // 将RAM的起始和终止地址、地址步进率写入相应的寄存器  0x0e
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x16);
	for (j = 0; j < 1024; j++)
	{
		if (mode == 1)
		{
			Write_32bit(RAM_AMP[j]); // 将幅度RAM的内容写入寄存器0x16
		}
		else if (mode == 2)
		{
			Write_32bit(RAM_AMP_TRIG_WAVE[j]); // 将三角波的数据写入ram
		}
		else if (mode == 3)
		{
			Write_32bit(RAM_AMP_SQUARE_WAVE[j]); // 将方波的数据写入ram
		}
	}
	AD9910_CSN_Set;

	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_Pha_W(void)
// 函数功能:将数据写入1024*32的RAM中-------------------------相位
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_Pha_W(void)
{
	int j;
	u8 CFR1[] = {0x00, 0x40, 0x00, 0x00};							  // 开启AD9910反Sinc滤波
	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0x00, 0x00, 0x00}; // 设置RAM的起始和终止地址、地址步进率

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr; // 将RAM的起始和终止地址、地址步进率写入相应的寄存器  0x0e
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新一下AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x16);
	for (j = 0; j < 1024; j++)
	{
		Write_32bit(RAM_PHA[j]); // 将频率RAM的内容写入0x16寄存器
	}
	AD9910_CSN_Set;

	AD9910_IUP_Clr; // 在更新一下AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_DIR_Fre_R(void)
// 函数功能:直接转换模式的回放-------------------------频率
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_DIR_Fre_R(void)
{
	int j;
	u8 CFR1[] = {0x80, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“00” 回放频率 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x00}; //				24~273   								直接转换模式RAM_PROx[7]=0x00
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x00}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x00}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x00}; //				774~1023								熟悉了可以自己设置
																	  // u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};
	// u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};
	// u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其相应的寄存器
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]); // 将PRO0~PRO4写入相应的寄存器
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0F);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910输出
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_DIR_AMP_R(void)
// 函数功能:直接转换模式的回放-------------------------幅度
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_DIR_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xC0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x00}; //				24~273   									直接转换模式RAM_PROx[7]=0x00
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x00}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x00}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x00}; //				774~1023								熟悉了可以自己设置
																	  // u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};
	// u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};
	// u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};

	AD9910_CSN_Clr;
	Write_8bit(0x00);
	for (j = 0; j < 4; j++) // 将CFR1写入其相应的寄存器
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	} // 将PRO0~PRO4写入相应的寄存器
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0F);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910输出
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_DIR_PHA_R(void)
// 函数功能:直接转换模式的回放-------------------------相位
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_DIR_PHA_R(void)
{
	int j;
	u8 CFR1[] = {0xA0, 0x40, 0x00, 0x00};							  // 使能RAM模式  将回放目的位设为“01” 回放相位 （注：使能后就变成了RAM寄存器了）
	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x00}; //				24~273   								直接转换模式RAM_PROx[7]=0x00
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x00}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x00}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x00}; //				774~1023								熟悉了可以自己设置
																	  // u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};
	// u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};
	// u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x00};

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其相应的寄存器
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]); // 将PRO0~PRO4写入相应的寄存器
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0F);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910输出
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_RAMP_UP_ONE_Fre_R(void)
// 函数功能:上斜坡模式的回放-------------------------频率
//														ONE 代表由外部引脚PRO2~PRO0控制
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_RAMP_UP_ONE_Fre_R(void)
{
	int j;
	u8 CFR1[] = {0x80, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“00” 回放频率 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x01}; //	从地址0回放到1023			（熟悉了这些回放可以自己设置）
																	  // 上斜坡模式RAM_PROx[7]=0x01

	//	u8 RAM_PRO0[]={0x00,0xff,0xff,0x05,0x00,0x00,0x00,0x01};
	//	u8 RAM_PRO1[]={0x00,0xff,0xff,0x44,0x40,0x06,0x00,0x01};
	//	u8 RAM_PRO2[]={0x00,0xff,0xff,0x82,0xC0,0x44,0x80,0x01};
	//	u8 RAM_PRO3[]={0x00,0xff,0xff,0xC1,0x40,0x83,0x00,0x01};
	//	u8 RAM_PRO4[]={0x00,0xff,0xff,0xff,0xC0,0xC1,0x80,0x01};
	//	u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	//	u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	//	u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_PRO0写入其寄存器0x0e
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	//	AD9910_CSN_Clr;
	//	Write_8bit(0x0f);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO1[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);
	//
	//	AD9910_CSN_Clr;
	//	Write_8bit(0x10);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO2[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);

	//	AD9910_CSN_Clr;
	//	Write_8bit(0x11);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO3[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);
	//
	//	AD9910_CSN_Clr;
	//	Write_8bit(0x12);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO4[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);

	AD9910_IUP_Clr; // 使能AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_RAMP_UP_ONE_AMP_R(void)
// 函数功能:上斜坡模式的回放-------------------------幅度
//														ONE 代表由外部引脚PRO2~PRO0控制
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_RAMP_UP_ONE_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xC0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x01}; //	从地址0回放到1023			（熟悉了这些回放可以自己设置）
																	  // 上斜坡模式RAM_PROx[7]=0x01
	//	u8 RAM_PRO1[]={0x00,0xff,0xff,0x44,0x40,0x06,0x00,0x01};
	//	u8 RAM_PRO2[]={0x00,0xff,0xff,0x82,0xC0,0x44,0x80,0x01};
	//	u8 RAM_PRO3[]={0x00,0xff,0xff,0xC1,0x40,0x83,0x00,0x01};
	//	u8 RAM_PRO4[]={0x00,0xff,0xff,0xff,0xC0,0xC1,0x80,0x01};
	//	u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	//	u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	//	u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_PRO0写入其寄存器0x0e
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	//	AD9910_CSN_Clr;
	//	Write_8bit(0x0f);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO1[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);
	//
	//	AD9910_CSN_Clr;
	//	Write_8bit(0x10);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO2[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);

	//	AD9910_CSN_Clr;
	//	Write_8bit(0x11);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO3[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);
	//
	//	AD9910_CSN_Clr;
	//	Write_8bit(0x12);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO4[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_RAMP_UP_ONE_PHA_R(void)
// 函数功能:上斜坡模式的回放-------------------------相位
//														ONE 代表由外部引脚PRO2~PRO0控制
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_RAMP_UP_ONE_PHA_R(void)
{
	int j;
	u8 CFR1[] = {0xA0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“01” 回放相位 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x01}; //	从地址0回放到1023			（熟悉了这些回放可以自己设置）
																	  // 上斜坡模式RAM_PROx[7]=0x01

	//	u8 RAM_PRO0[]={0x00,0xff,0xff,0x05,0x00,0x00,0x00,0x01};
	//	u8 RAM_PRO1[]={0x00,0xff,0xff,0x44,0x40,0x06,0x00,0x01};
	//	u8 RAM_PRO2[]={0x00,0xff,0xff,0x82,0xC0,0x44,0x80,0x01};
	//	u8 RAM_PRO3[]={0x00,0xff,0xff,0xC1,0x40,0x83,0x00,0x01};
	//	u8 RAM_PRO4[]={0x00,0xff,0xff,0xff,0xC0,0xC1,0x80,0x01};
	//	u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	//	u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	//	u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其寄存器0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_PRO0写入其寄存器0x0e
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	//	AD9910_CSN_Clr;
	//	Write_8bit(0x0f);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO1[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);
	//
	//	AD9910_CSN_Clr;
	//	Write_8bit(0x10);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO2[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);

	//	AD9910_CSN_Clr;
	//	Write_8bit(0x11);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO3[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);
	//
	//	AD9910_CSN_Clr;
	//	Write_8bit(0x12);
	//	for(j=0;j<8;j++)
	//	{
	//		Write_8bit(RAM_PRO4[j]);
	//	}
	//	AD9910_CSN_Set;
	//	Delay_ns (10);

	AD9910_IUP_Clr; // 使能AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_RAMP_UP_TWO_Fre_R(void)
// 函数功能:上斜坡模式的回放-------------------------频率
//														TWO 代表由内部控制							控制位为（CFR1[20:17]） 参考数据手册
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_RAMP_UP_TWO_Fre_R(void)
{
	int j;
	u8 CFR1[] = {0x80, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“00” 回放频率 （注：使能后就变成了RAM寄存器了）
										  // 选择相应的控制位，我选择的是0100   突发 执行Profile 0至Profile 4，然后中止
										  // 控制位为（CFR1[20:17]） 参考数据手册

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x01}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x01}; //				24~273   												//上斜坡模式RAM_PROx[7]=0x01
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x01}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x01}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x01}; //				774~1023								熟悉了可以自己设置
																	  // u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	// u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	// u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};

	AD9910_CSN_Clr;
	Write_8bit(0x00);
	for (j = 0; j < 4; j++) // 将CFR1写入其相应的寄存器
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	} // 将PRO0~PRO4写入相应的寄存器
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0f);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_RAMP_UP_TWO_AMP_R(void)
// 函数功能:上斜坡模式的回放-------------------------幅度
//														TWO 代表由内部控制							控制位为（CFR1[20:17]） 参考数据手册
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_RAMP_UP_TWO_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xC0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）
										  // 选择相应的控制位，我选择的是0100   突发 执行Profile 0至Profile 4，然后中止
										  // 控制位为（CFR1[20:17]） 参考数据手册

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x01}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x01}; //				24~273   												//上斜坡模式RAM_PROx[7]=0x01
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x01}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x01}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x01}; //				774~1023								熟悉了可以自己设置
																	  // u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	// u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	// u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};

	AD9910_CSN_Clr;
	Write_8bit(0x00);
	for (j = 0; j < 4; j++)
	{ // 将CFR1写入其相应的寄存器
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set; // 将PRO0~PRO4写入相应的寄存器
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0f);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_RAMP_UP_TWO_PHA_R(void)
// 函数功能:上斜坡模式的回放-------------------------相位
//														TWO 代表由内部控制							控制位为（CFR1[20:17]） 参考数据手册
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_RAMP_UP_TWO_PHA_R(void)
{
	int j;
	u8 CFR1[] = {0xA0, 0x48, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“01” 回放相位 （注：使能后就变成了RAM寄存器了）
										  // 选择相应的控制位，我选择的是0100   突发 执行Profile 0至Profile 4，然后中止
										  // 控制位为（CFR1[20:17]） 参考数据手册

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x01}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x01}; //				24~273   												//上斜坡模式RAM_PROx[7]=0x01
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x01}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x01}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x01}; //				774~1023								熟悉了可以自己设置
																	  // u8 RAM_PRO5[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	// u8 RAM_PRO6[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};
	// u8 RAM_PRO7[]={0x00,0xff,0xff,0xff,0xC0,0x00,0x00,0x01};

	AD9910_CSN_Clr;
	Write_8bit(0x00);
	for (j = 0; j < 4; j++) // 将CFR1写入其相应的寄存器
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	} // 将PRO0~PRO4写入相应的寄存器
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0f);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12);
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_BID_RAMP_Fre_R(void)
// 函数功能:双向斜坡模式的回放-------------------------频率
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_BID_RAMP_Fre_R(void)
{
	int j;
	u8 CFR1[] = {0x80, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“00” 回放频率 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0x05, 0x00, 0x00, 0x00, 0x01}; //	从地址0到地址23
	u8 RAM_PRO1[] = {0x00, 0xff, 0xff, 0x44, 0x40, 0x06, 0x00, 0x01}; //				24~273   												//上斜坡模式RAM_PROx[7]=0x01
	u8 RAM_PRO2[] = {0x00, 0xff, 0xff, 0x82, 0xC0, 0x44, 0x80, 0x01}; //      	274~523
	u8 RAM_PRO3[] = {0x00, 0xff, 0xff, 0xC1, 0x40, 0x83, 0x00, 0x01}; //				524~773
	u8 RAM_PRO4[] = {0x00, 0xff, 0xff, 0xff, 0xC0, 0xC1, 0x80, 0x01}; //				774~1023								熟悉了可以自己设置

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPO0写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0f); // 将RAM_RPO1写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x10); // 将RAM_RPO2写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x11); // 将RAM_RPO3写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO3[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x12); // 将RAM_RPO4写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO4[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_BID_RAMP_AMP_R(void)
// 函数功能:双向斜坡模式的回放-------------------------幅度
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_BID_RAMP_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xC0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x02}; //	从地址0回放到1023					// 双向斜坡模式RAM_PROx[7]=0x02

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_BID_RAMP_PHA_R(void)
// 函数功能:双向斜坡模式的回放-------------------------相位
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_BID_RAMP_PHA_R(void)
{
	int j;
	u8 CFR1[] = {0xA0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“01” 回放相位 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x02}; //	从地址0回放到1023					// 双向斜坡模式RAM_PROx[7]=0x02

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr; // 更新AD9910
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_BID_RAMP_Fre_R(void)
// 函数功能:连续双向斜坡模式的回放-------------------------频率
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_CON_BID_RAMP_Fre_R(void)
{
	int j;
	u8 CFR1[] = {0x80, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“00” 回放频率 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x03

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_BID_RAMP_AMP_R(void)
// 函数功能:连续双向斜坡模式的回放-------------------------幅度
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_CON_BID_RAMP_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xC0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x03

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_BID_RAMP_PHA_R(void)
// 函数功能:连续双向斜坡模式的回放-------------------------相位
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_CON_BID_RAMP_PHA_R(void)
{
	int j;
	u8 CFR1[] = {0xA0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“01” 回放相位 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x03

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_RECIR_Fre_R(void)
// 函数功能:连续循环模式的回放-------------------------频率
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_CON_RECIR_Fre_R(void)
{
	int j;
	u8 CFR1[] = {0x80, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“00” 回放频率 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x04}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x04

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
	Delay_ns(10);
}

//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_RECIR_AMP_R(void)
// 函数功能:连续循环模式的回放-------------------------幅度
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_CON_RECIR_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xE0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x04}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x04

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_WAVE_AMP_R(void)
// 函数功能:连续循环模式的回放-------------------------幅度
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_WAVE_AMP_R(void)
{
	int j;
	u8 CFR1[] = {0xC0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“10” 回放幅度 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x04}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x04

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//---------------------------------------------------------------------------------------------------------------------------------
// 函数名称:void AD9910_RAM_CON_RECIR_PHA_R(void)
// 函数功能:连续循环模式的回放-------------------------相位
//
// 入口参数:无
// 出口参数:无
//---------------------------------------------------------------------------------------------------------------------------------
void AD9910_RAM_CON_RECIR_PHA_R(void)
{
	int j;
	u8 CFR1[] = {0xA0, 0x40, 0x00, 0x00}; // 使能RAM模式  将回放目的位设为“01” 回放相位 （注：使能后就变成了RAM寄存器了）

	u8 RAM_PRO0[] = {0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x04}; //	从地址0回放到1023					// 连续双向斜坡模式RAM_PROx[7]=0x04

	AD9910_CSN_Clr;
	Write_8bit(0x00); // 将CFR1写入其地址0x00
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_CSN_Clr;
	Write_8bit(0x0e); // 将RAM_RPOx写入其地址
	for (j = 0; j < 8; j++)
	{
		Write_8bit(RAM_PRO0[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10); // 更新AD9910
	AD9910_IUP_Clr;
	Delay_ns(10);
}
//------------------------------------------------------------------------------------
// 函数名称:void Freq_convert(u32 Freq)
// 函数功能:AD9910写初始频率
// 入口参数:Freq      写频率   频率小于450M
// 出口参数:无
//------------------------------------------------------------------------------------
void Freq_convert(u32 Freq)
{
	u32 Temp;
	Temp = (u32)Freq * 4.294967296; // 将输入频率因子分为四个字节  4.294967296=(2^32)/1000000000

	FTW[3] = (u8)Temp;
	FTW[2] = (u8)(Temp >> 8);
	FTW[1] = (u8)(Temp >> 16);
	FTW[0] = (u8)(Temp >> 24);
}
//------------------------------------------------------------------------------------
// 函数名称:void Amp_convert(u32 Amp)
// 函数功能:AD9910写初始幅度
// 入口参数:Amp      写幅度
// 出口参数:无
//------------------------------------------------------------------------------------
void Amp_convert(u32 Amp)
{

	ASF[3] = (u8)Amp;
	ASF[2] = (u8)(Amp >> 8);
}
//------------------------------------------------------------------------------------
// 函数名称:void AD9910_Init_Sin(void)
// 函数功能:AD9910时钟的设置和辅助DAC设置 以及AD9910并行正弦初始化
// 入口参数:无
// 出口参数:无
//------------------------------------------------------------------------------------
void AD9910_Init_Sin(int gain)
{
	int j;
	AD9910_PF0_Clr; // 选择 Profile0
	AD9910_PF1_Clr;
	AD9910_PF2_Clr;
	AD9910_DRH_Clr;
	AD9910_DRC_Clr;
	AD9910_MRT_Set; // 复位AD9910
	AD9910_MRT_Clr;

	AD9910_CSN_Clr;	  // CSN置0			开片选
	Write_8bit(0x00); // 选择0x00地址            CFR1寄存器写入数据
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR1[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);
	AD9910_CSN_Clr; // CSN置1			关片选

	AD9910_CSN_Clr; // CSN置0			开片选

	Write_8bit(0x01); // 选择0x01地址            CFR2寄存器写入数据
	CFR2[3] += gain;
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR2[j]);
	}
	AD9910_CSN_Set;
	Delay_ns(10);
	CFR2[3] = 0x30;
	AD9910_CSN_Clr; // CSN置0			开片选

	Write_8bit(0x02); // 选择0x02地址            CFR3寄存器写入数据
	for (j = 0; j < 4; j++)
	{
		Write_8bit(CFR3[j]);
	}
	AD9910_CSN_Set; // CSN置1			关片选
	Delay_ns(10);

	AD9910_CSN_Clr;	  // CSN置0			开片选
	Write_8bit(0x03); // 选择0x03地址            辅助DAC寄存器写入数据
	for (j = 0; j < 4; j++)
	{
		Write_8bit(Assist_DAC[j]);
	}
	AD9910_CSN_Set; // CSN置1			关片选
	Delay_ns(10);

	AD9910_CSN_Clr;	  // CSN置0			开片选
	Write_8bit(0x07); // 选择0x07地址           	将初始频率写入寄存器
	for (j = 0; j < 4; j++)
	{
		Write_8bit(FTW[j]);
	}
	AD9910_CSN_Set; // CSN置1			关片选
	Delay_ns(10);

	AD9910_IUP_Clr;
	Delay_ns(10);
	AD9910_IUP_Set;
	Delay_ns(10);
	AD9910_IUP_Clr;
}
//------------------------------------------------------------------------------------
// 函数名称:void Par_mod(u8 des ,u16 FF)
// 函数功能:AD9910并行口写入数据
// 入口参数:des      位数据
//				 FF
// 出口参数:无
//------------------------------------------------------------------------------------
void Par_mod(u8 des, u16 FF) //  FF最大的值位15240
{
	u16 Temp[20], Data[3];
	u16 data;
	data = FF * 4.294967296; // 芯片手册上的公式 4.294967296=(2^32)/1000000000
	AD9910_TE_Clr;
	AD9910_TE_Set;

	Temp[1] = (u16)(des & 0x01) << 5; // A5                      F0
	Temp[2] = (u16)(des & 0x02) << 3; // A4											 F1

	Data[0] = Temp[1] + Temp[2];
	Temp[0] = GPIO_ReadOutputData(GPIOA);
	GPIOA->ODR = (Data[0] & 0x0030) | (Temp[0] & 0xFFCF);

	Temp[1] = (u16)(data & 0x0001) << 3;  // A3               D0
	Temp[2] = (u16)(data & 0x0002) << 1;  // A2								D1
	Temp[3] = (u16)(data & 0x0004) >> 1;  // A1								D2
	Temp[4] = (u16)(data & 0x0008) >> 3;  // A0								D3
	Temp[5] = (u16)(data & 0x0010) << 1;  // C5								D4
	Temp[6] = (u16)(data & 0x0020) >> 1;  // C4								D5
	Temp[7] = (u16)(data & 0x0040) >> 3;  // C3 							D6
	Temp[8] = (u16)(data & 0x0080) >> 5;  // C2								D7
	Temp[9] = (u16)(data & 0x0100) >> 7;  // C1								D8
	Temp[10] = (u16)(data & 0x0200) >> 9; // C0								D9
	Temp[11] = (u16)(data & 0x0400) >> 2; // C8								D10
	Temp[12] = (u16)(data & 0x0800) >> 2; // C9								D11
	Temp[13] = (u16)(data & 0x1000) >> 2; // C10							D12
	Temp[14] = (u16)(data & 0x2000) >> 2; // C11							D13
	Temp[15] = (u16)(data & 0x4000) >> 2; // C12							D14
	Temp[16] = (u16)(data & 0x8000) >> 2; // C13							D15

	Data[0] = Temp[1] + Temp[2] + Temp[3] + Temp[4];
	Data[1] = Temp[5] + Temp[6] + Temp[7] + Temp[8] + Temp[9] + Temp[10] + Temp[11] + Temp[12] + Temp[13] + Temp[14] + Temp[15] + Temp[16];

	Temp[0] = GPIO_ReadOutputData(GPIOA);
	GPIOA->ODR = (Data[0] & 0x000F) | (Temp[0] & 0xFFF0);

	Temp[0] = GPIO_ReadOutputData(GPIOC);
	GPIOC->ODR = (Data[1] & 0x3F3F) | (Temp[0] & 0xC0C0);
	//	AD9910_TE_Clr;
}
