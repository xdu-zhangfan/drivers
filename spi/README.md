# spi

## 文件夹结构
```
spi/spi_master.v   SPI主机
spi/spi_slave.v    SPI从机
```

## 模块说明

### 共有参数

```
parameter CPOL  = 0,  // Clock Polarity, 0 and 1 allowed
parameter CPHA  = 0,  // Clock Phase, 0 and 1 allowed
parameter FSB   = 1,  // First bit, 0-LSB, 1-MSB
parameter WIDTH = 8   // Data bit width
```

``CPOL``为时钟极性，即空闲时，时钟应该保持的电平。设置为``0``则为低电平，``1``则为高电平。

``CPHA``为时钟相位，即时钟对数据的采样沿序号。设置为``0``则为第一时钟沿，设置为``1``则为第二时钟沿。

``FSB``为位序，指示先发送低位比特还是先发送高位比特。设置为``0``则为低位先发送，设置为``1``则为高位先发送。

``WIDTH``为位宽，指示单次发送数据有多少比特。因内部逻辑限制，该参数不得小于``1``，不得大于``256``。

### spi_master 私有参数

```
parameter SYSCLK_FREQ = 50000000,  // System clock frequency, default 50MHz
parameter SPICLK_FREQ = 1000000,   // SPI clock frequency, default 1MHz
```

``SYSCLK_FREQ``为系统时钟频率，单位为``Hz``

``SPICLK_FREQ``为期望的SPI时钟频率，单位为``Hz``

## 共有信号

```
input clk,
input rstn,

output reg spi_sclk,
output reg spi_ss_n,
output     spi_mosi,
input      spi_miso,

output reg                 spi_valid,
input                      spi_ready,
output     [WIDTH - 1 : 0] spi_rx_data,
input      [WIDTH - 1 : 0] spi_tx_data
```

``clk``为系统时钟，``rstn``为低电平有效的复位信号。

``spi_sclk``、``spi_ss_n``、``spi_mosi``、``spi_miso``分别为SPI的时钟信号、低有效片选信号、主发从收信号和主收从发信号。

``spi_valid``为模块可接受数据的指示信号，``spi_ready``为写入使能信号，``spi_rx_data``为将要发送的数据，``spi_tx_data``为接收到的数据。这四个信号的时序关系如下：

<div align="center">
<script type="WaveDrom">
{signal: [
  {name: 'clk', wave: 'p.......'},
  {name: 'spi_valid', wave: 'x0.10..x'},
  {name: 'spi_ready', wave: 'x01.0..x'},
  {name: 'spi_rx_data', wave: 'x0.30..x'},
  {name: 'spi_tx_data', wave: 'x0.40..x'}
]}
</script>
</div>

<div align="center">
<script type="WaveDrom">
{signal: [
  {name: 'clk', wave: 'p.......'},
  {name: 'spi_valid', wave: 'x0.10..x'},
  {name: 'spi_ready', wave: 'x01..0.x'},
  {name: 'spi_rx_data', wave: 'x0.30..x'},
  {name: 'spi_tx_data', wave: 'x0.40..x'}
]}
</script>
</div>

<div align="center">
<script type="WaveDrom">
{signal: [
  {name: 'clk', wave: 'p.......'},
  {name: 'spi_valid', wave: 'x01..0.x'},
  {name: 'spi_ready', wave: 'x0..10.x'},
  {name: 'spi_rx_data', wave: 'x0.30..x'},
  {name: 'spi_tx_data', wave: 'x0.40..x'}
]}
</script>
</div>

以上所出现的几种情况都是可以被允许的。注意，``spi_valid``与``spi_ready``仅在一个时钟周期内同时拉高。如果后续``spi_ready``不拉低，对模块的运行没有任何影响。

实际上，如果要连续发送和接受数据，可以把``spi_ready``一直拉高，无需担心会影响模块的运行。
