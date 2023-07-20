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

  reg           param_wen = 0;
  reg  [31 : 0] spi_period = 0;

  wire          spi_sclk;
  wire          spi_ss_n;
  wire          spi_mosi;
  wire          spi_miso;

  reg           spi_m_ready = 0;
  wire          spi_m_valid;
  wire [ 7 : 0] spi_m_rx_data;
  reg  [ 7 : 0] spi_m_tx_data = 8'h0;
  spi_master spi_master_inst (
      .clk (clk),
      .rstn(rstn),

      .param_wen (param_wen),
      .spi_period(spi_period),

      .spi_sclk(spi_sclk),
      .spi_ss_n(spi_ss_n),
      .spi_mosi(spi_mosi),
      .spi_miso(spi_miso),

      .spi_ready  (spi_m_ready),
      .spi_valid  (spi_m_valid),
      .spi_rx_data(spi_m_rx_data),
      .spi_tx_data(spi_m_tx_data)
  );

  reg          spi_s_ready = 0;
  wire         spi_s_valid;
  wire [7 : 0] spi_s_rx_data;
  reg  [7 : 0] spi_s_tx_data = 8'h0;
  spi_slave spi_slave_inst (
      .clk (clk),
      .rstn(rstn),

      .spi_sclk(spi_sclk),
      .spi_ss_n(spi_ss_n),
      .spi_mosi(spi_mosi),
      .spi_miso(spi_miso),

      .spi_ready  (spi_s_ready),
      .spi_valid  (spi_s_valid),
      .spi_rx_data(spi_s_rx_data),
      .spi_tx_data(spi_s_tx_data)
  );

  initial begin
    #100 rstn = 1;

    #10 param_wen = 1;
    spi_period = 32'h0000_0010;

    #2 param_wen = 0;

    #51 spi_m_ready = 1;
    spi_m_tx_data = 8'ha5;

    #2 spi_m_ready = 0;
    spi_m_tx_data = 8'h0;

    #1000 spi_m_ready = 1;
    spi_m_tx_data = 8'h5a;
    spi_s_ready   = 1;
    spi_s_tx_data = 8'h16;

    #2 spi_m_ready = 0;
    spi_m_tx_data = 8'h0;

    #1000 spi_m_ready = 1;
    spi_m_tx_data = 8'h15;

    #2 spi_m_ready = 0;
    spi_m_tx_data = 8'h0;

    #1000 spi_m_ready = 1;
    spi_m_tx_data = 8'h00;

    #2 spi_m_ready = 0;
    spi_m_tx_data = 8'h0;

    #5000 $finish;
  end

  initial begin
    $dumpfile("wave.vcd");  //生成的vcd文件名称
    $dumpvars(0, bench);  //tb模块名称
  end

endmodule
