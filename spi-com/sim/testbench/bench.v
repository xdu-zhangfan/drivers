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

  always #5 clk = ~clk;

  wire          spi_m_sck;
  wire          spi_m_csn;
  wire          spi_m_mosi;
  reg           spi_m_miso = 0;

  reg           spi_s_sck = 0;
  reg           spi_s_csn = 1;
  reg           spi_s_mosi = 0;
  wire          spi_s_miso;

  wire [31 : 0] config_ram_do;
  reg  [31 : 0] config_ram_di = 31'h0;
  reg  [13 : 0] config_ram_addr = 14'h0;
  reg           config_ram_we = 0;
  reg           config_ram_ce = 0;

  wire [31 : 0] trans_ram_do;
  reg  [31 : 0] trans_ram_di = 31'h0;
  reg  [13 : 0] trans_ram_addr = 14'h0;
  reg           trans_ram_we = 0;
  reg           trans_ram_ce = 0;

  spi spi_com_inst (
      .clk (clk),
      .rstn(rstn),

      .spi_s_mosi(spi_s_mosi),
      .spi_s_miso(spi_s_miso),
      .spi_s_sck (spi_s_sck),
      .spi_s_csn (spi_s_csn),

      .spi_m_mosi(spi_m_mosi),
      .spi_m_miso(spi_m_miso),
      .spi_m_sck (spi_m_sck),
      .spi_m_csn (spi_m_csn),

      .config_ram_do  (config_ram_do),
      .config_ram_di  (config_ram_di),
      .config_ram_addr(config_ram_addr),
      .config_ram_we  (config_ram_we),
      .config_ram_ce  (config_ram_ce),
      .config_ram_clk (clk),

      .trans_ram_do  (trans_ram_do),
      .trans_ram_di  (trans_ram_di),
      .trans_ram_addr(trans_ram_addr),
      .trans_ram_we  (trans_ram_we),
      .trans_ram_ce  (trans_ram_ce),
      .trans_ram_clk (clk)
  );

  reg [7 : 0] send_data = 8'h0;
  integer i;
  event spi_sim_send;
  always @(spi_sim_send) begin
    #2 spi_s_sck = 0;
    spi_s_csn  = 1;
    spi_s_mosi = 0;

    #200;

    for (i = 0; i < 8; i = i + 1) begin
      #200 spi_s_sck = 0;
      spi_s_mosi = send_data[7-i];
      spi_s_csn  = 0;

      #200 spi_s_sck = 1;
      spi_s_mosi = spi_s_mosi;
      spi_s_csn  = 0;
    end

    #200 spi_s_sck = 0;
    spi_s_csn  = 0;
    spi_s_mosi = 0;

    #200 spi_s_sck = 0;
    spi_s_csn  = 1;
    spi_s_mosi = 0;
  end

  initial begin
    #100 rstn = 1;

    #1000000 send_data = 08'h10;
    #20->spi_sim_send;

    #14000 send_data = 08'h10;
    #20->spi_sim_send;

    #14000 send_data = 08'h01;
    #20->spi_sim_send;

    #14000 send_data = 08'h05;
    #20->spi_sim_send;

    #14000 send_data = 08'h11;
    #20->spi_sim_send;

    #14000 send_data = 08'h22;
    #20->spi_sim_send;

    #14000 send_data = 08'h33;
    #20->spi_sim_send;

    #14000 send_data = 08'h44;
    #20->spi_sim_send;

    #14000 send_data = 08'h55;
    #20->spi_sim_send;

    #14000 send_data = 08'h11;
    #20->spi_sim_send;

    #14000 send_data = 08'h01;
    #20->spi_sim_send;

    #14000 send_data = 08'h08;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h00;
    #20->spi_sim_send;

    #14000 send_data = 08'h12;
    #20->spi_sim_send;

    #14000 send_data = 08'h01;
    #20->spi_sim_send;

    #14000 send_data = 08'h05;
    #20->spi_sim_send;

    #14006 config_ram_addr = 14'h0100 / 4;
    config_ram_we = 0;
    config_ram_ce = 1;
    #20 config_ram_addr = 14'h00;
    config_ram_we = 0;
    config_ram_ce = 0;

    #14000 trans_ram_di = 32'h11223344;
    trans_ram_addr = 14'h140;
    trans_ram_we   = 1;
    trans_ram_ce   = 1;
    #10 trans_ram_di = 32'h55667788;
    trans_ram_addr = 14'h141;
    trans_ram_we   = 1;
    trans_ram_ce   = 1;
    #10 trans_ram_di = 32'h99aabbcc;
    trans_ram_addr = 14'h142;
    trans_ram_we   = 1;
    trans_ram_ce   = 1;
    #10 trans_ram_di = 32'hddeeff00;
    trans_ram_addr = 14'h143;
    trans_ram_we   = 1;
    trans_ram_ce   = 1;
    #10 trans_ram_di = 32'h0;
    trans_ram_addr = 14'h0;
    trans_ram_we   = 0;
    trans_ram_ce   = 0;

    #14000 send_data = 08'h13;
    #20->spi_sim_send;

    #14000 send_data = 08'h01;
    #20->spi_sim_send;

    #14000 send_data = 08'h05;
    #20->spi_sim_send;

    #14000 send_data = 08'h06;
    #20->spi_sim_send;
  end

endmodule
