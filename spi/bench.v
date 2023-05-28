`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/05/27 23:20:16
// Design Name: 
// Module Name: bench
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module bench ();
  reg clk = 0;
  reg rstn = 0;

  always #1 clk = ~clk;

  wire spi_sclk;
  wire spi_ss_n;
  wire spi_mosi;
  wire spi_miso;

  reg spi_ready = 0;
  wire spi_valid;
  wire [7 : 0] spi_rx_data;
  reg [7:0] spi_tx_data = 8'h0;
  spi_master #(
      .SYSCLK_FREQ(1000000000),
      .SPICLK_FREQ(50000000),

      .CPOL (0),
      .CPHA (0),
      .FSB  (0),
      .WIDTH(16)
  ) spi_master_inst (
      .clk (clk),
      .rstn(rstn),

      .spi_sclk(spi_sclk),
      .spi_ss_n(spi_ss_n),
      .spi_mosi(spi_mosi),
      .spi_miso(spi_miso),

      .spi_ready  (spi_ready),
      .spi_valid  (spi_valid),
      .spi_rx_data(spi_rx_data),
      .spi_tx_data(spi_tx_data)
  );

  initial begin
    #100 rstn = 1;

    #51 spi_ready = 1;
    spi_tx_data = 8'ha5;

    #2 spi_ready = 0;
    spi_tx_data = 8'h0;

    #1000 spi_ready = 1;
    spi_tx_data = 8'ha5;

    #2 spi_ready = 0;
    spi_tx_data = 8'h0;

    #1000 spi_ready = 1;
    spi_tx_data = 8'ha5;

    #2 spi_ready = 0;
    spi_tx_data = 8'h0;

    #5000 $finish;
  end

  initial begin
    $dumpfile("wave.vcd");  //生成的vcd文件名称
    $dumpvars(0, bench);  //tb模块名称
  end

  assign spi_miso = spi_mosi;
endmodule
