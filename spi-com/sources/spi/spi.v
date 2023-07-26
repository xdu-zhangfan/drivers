module spi (
    input clk,
    input rstn,

    input  spi_s_mosi,
    output spi_s_miso,
    input  spi_s_sck,
    input  spi_s_csn,

    output spi_m_mosi,
    input  spi_m_miso,
    output spi_m_sck,
    output spi_m_csn,

    output reg [15 : 0] int_out,

    output [31 : 0] config_ram_do,
    input  [31 : 0] config_ram_di,
    input  [13 : 0] config_ram_addr,
    input           config_ram_we,
    input           config_ram_ce,
    input           config_ram_clk,

    output [31 : 0] trans_ram_do,
    input  [31 : 0] trans_ram_di,
    input  [13 : 0] trans_ram_addr,
    input           trans_ram_we,
    input           trans_ram_ce,
    input           trans_ram_clk
);

  wire [ 7 : 0] spi_config_ram_data_out;
  reg  [ 7 : 0] spi_config_ram_data_in;
  reg  [15 : 0] spi_config_ram_addr;
  reg           spi_config_ram_ce;
  reg           spi_config_ram_we;
  spi_ram spi_config_ram_inst (
      .doa  (spi_config_ram_data_out),
      .dia  (spi_config_ram_data_in),
      .addra(spi_config_ram_addr),
      .cea  (spi_config_ram_ce),
      .clka (clk),
      .wea  (spi_config_ram_we),

      .dob  (config_ram_do),
      .dib  (config_ram_di),
      .addrb(config_ram_addr),
      .ceb  (config_ram_ce),
      .clkb (config_ram_clk),
      .web  (config_ram_we)
  );

  wire [ 7 : 0] spi_trans_ram_data_out;
  reg  [ 7 : 0] spi_trans_ram_data_in;
  reg  [15 : 0] spi_trans_ram_addr;
  reg           spi_trans_ram_ce;
  reg           spi_trans_ram_we;
  spi_ram spi_trans_ram_inst (
      .doa  (spi_trans_ram_data_out),
      .dia  (spi_trans_ram_data_in),
      .addra(spi_trans_ram_addr),
      .cea  (spi_trans_ram_ce),
      .clka (clk),
      .wea  (spi_trans_ram_we),

      .dob  (trans_ram_do),
      .dib  (trans_ram_di),
      .addrb(trans_ram_addr),
      .ceb  (trans_ram_ce),
      .clkb (trans_ram_clk),
      .web  (trans_ram_we)
  );

  reg  [ 1 : 0] spi_slave_core_address;
  reg  [31 : 0] spi_slave_core_data_in;
  wire [31 : 0] spi_slave_core_data_out;
  reg           spi_slave_core_data_read;
  reg           spi_slave_core_data_write;
  wire          spi_slave_core_roe;
  wire          spi_slave_core_rprdy;
  wire          spi_slave_core_rrdy;
  wire          spi_slave_core_toe;
  wire          spi_slave_core_trdy;
  spi_slave_core spi_slave_core_inst (
      .Sys_Clk(clk),
      .Rst_n  (rstn),

      .MOSI(spi_s_mosi),
      .MISO(spi_s_miso),
      .SCLK(spi_s_sck),
      .SS_n(spi_s_csn),

      .Address     (spi_slave_core_address),
      .Data_to_IP  (spi_slave_core_data_in),
      .Data_from_IP(spi_slave_core_data_out),
      .Read        (spi_slave_core_data_read),
      .Write       (spi_slave_core_data_write),

      .ROE  (spi_slave_core_roe),
      .RPRDY(spi_slave_core_rprdy),
      .RRDY (spi_slave_core_rrdy),
      .TOE  (spi_slave_core_toe),
      .TRDY (spi_slave_core_trdy)
  );

  reg  [ 1 : 0] spi_master_core_address;
  reg  [31 : 0] spi_master_core_data_in;
  wire [31 : 0] spi_master_core_data_out;
  reg           spi_master_core_data_read;
  reg           spi_master_core_data_write;
  wire          spi_master_core_roe;
  wire          spi_master_core_rprdy;
  wire          spi_master_core_rrdy;
  wire          spi_master_core_toe;
  wire          spi_master_core_trdy;
  spi_master_core spi_master_core_inst (
      .Sys_Clk(clk),
      .Rst_n  (rstn),

      .MOSI(spi_m_mosi),
      .MISO(spi_m_miso),
      .SCLK(spi_m_sck),
      .SS_n(spi_m_csn),

      .Address     (spi_master_core_address),
      .Data_to_IP  (spi_master_core_data_in),
      .Data_from_IP(spi_master_core_data_out),
      .Read        (spi_master_core_data_read),
      .Write       (spi_master_core_data_write),

      .ROE (spi_master_core_roe),
      .RRDY(spi_master_core_rrdy),
      .TOE (spi_master_core_toe),
      .TRDY(spi_master_core_trdy)
  );

  reg [7 : 0] trans_mode;
  reg [7 : 0] trans_start_addr;
  reg [7 : 0] trans_end_addr;

  reg trans_int;

  reg [7 : 0] option_buf;
  localparam OPTION_RX_SEG = 32'h10;
  localparam OPTION_TX_SEG = 32'h11;
  localparam OPTION_INT = 32'h12;
  localparam OPTION_SET_TRANS = 32'h13;

  reg [ 7 : 0] arg_0_buf;
  reg [ 7 : 0] arg_1_buf;

  reg [31 : 0] config_ram_ci;
  reg [31 : 0] config_ram_si;
  localparam CONFIG_RAM_LENGTH = 32'd65536;

  reg [7 : 0] state;
  localparam STATE_RESET = 8'h00;
  localparam STATE_READ_OPTION = 8'h01;
  localparam STATE_READ_OPTION_READ = 8'h02;
  localparam STATE_READ_ADDR = 8'h03;
  localparam STATE_READ_ADDR_READ = 8'h04;
  localparam STATE_READ_LENGTH = 8'h05;
  localparam STATE_READ_LENGTH_READ = 8'h06;
  localparam STATE_RX_DATA_0 = 8'h07;
  localparam STATE_RX_DATA_1 = 8'h08;
  localparam STATE_TX_DATA_0 = 8'h09;
  localparam STATE_TX_DATA_1 = 8'h0a;
  localparam STATE_TX_DATA_2 = 8'h0b;
  localparam STATE_TX_DATA_3 = 8'h0c;
  localparam STATE_TX_DATA_4 = 8'h0d;
  localparam STATE_INT_READ_LOW_BYTE = 8'h0e;
  localparam STATE_INT_READ_LOW_BYTE_READ = 8'h0f;
  localparam STATE_INT_READ_HIGH_BYTE = 8'h10;
  localparam STATE_INT_READ_HIGH_BYTE_READ = 8'h11;
  localparam STATE_INT = 8'h12;
  localparam STATE_TRANS_READ_MODE = 8'h13;
  localparam STATE_TRANS_READ_MODE_READ = 8'h14;
  localparam STATE_TRANS_READ_S_ADDR = 8'h15;
  localparam STATE_TRANS_READ_S_ADDR_READ = 8'h16;
  localparam STATE_TRANS_READ_E_ADDR = 8'h17;
  localparam STATE_TRANS_READ_E_ADDR_READ = 8'h18;
  localparam STATE_TRANS_INT = 8'h19;

  always @(posedge clk) begin
    if (!rstn) begin
      state <= STATE_RESET;
    end else begin
      case (state)
        STATE_RESET: begin
          if (config_ram_ci < CONFIG_RAM_LENGTH - 1) begin
            state <= STATE_RESET;
          end else begin
            state <= STATE_READ_OPTION;
          end
        end
        STATE_READ_OPTION: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_READ_OPTION_READ;
          end else begin
            state <= STATE_READ_OPTION;
          end
        end
        STATE_READ_OPTION_READ: begin
          if (!spi_slave_core_rrdy) begin
            case (option_buf)
              OPTION_RX_SEG:    state <= STATE_READ_ADDR;
              OPTION_TX_SEG:    state <= STATE_READ_ADDR;
              OPTION_INT:       state <= STATE_INT_READ_LOW_BYTE;
              OPTION_SET_TRANS: state <= STATE_TRANS_READ_MODE;
              default:          state <= STATE_READ_OPTION;
            endcase
          end else begin
            state <= STATE_READ_OPTION_READ;
          end
        end
        STATE_READ_ADDR: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_READ_ADDR_READ;
          end else begin
            state <= STATE_READ_ADDR;
          end
        end
        STATE_READ_ADDR_READ: begin
          if (!spi_slave_core_rrdy) begin
            state <= STATE_READ_LENGTH;
          end else begin
            state <= STATE_READ_ADDR_READ;
          end
        end
        STATE_READ_LENGTH: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_READ_LENGTH_READ;
          end else begin
            state <= STATE_READ_LENGTH;
          end
        end
        STATE_READ_LENGTH_READ: begin
          if (!spi_slave_core_rrdy) begin
            case (option_buf)
              OPTION_RX_SEG: state <= STATE_RX_DATA_0;
              OPTION_TX_SEG: state <= STATE_TX_DATA_0;
              default:       state <= STATE_READ_OPTION;
            endcase
          end else begin
            state <= STATE_READ_LENGTH_READ;
          end
        end
        STATE_RX_DATA_0: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_RX_DATA_1;
          end else begin
            state <= STATE_RX_DATA_0;
          end
        end
        STATE_RX_DATA_1: begin
          if (!spi_slave_core_rrdy) begin
            if (config_ram_si < arg_1_buf) begin
              state <= STATE_RX_DATA_0;
            end else begin
              state <= STATE_READ_OPTION;
            end
          end else begin
            state <= STATE_RX_DATA_1;
          end
        end
        STATE_TX_DATA_0: begin
          if (spi_slave_core_trdy) begin
            state <= STATE_TX_DATA_1;
          end else begin
            state <= STATE_TX_DATA_0;
          end
        end
        STATE_TX_DATA_1: begin
          state <= STATE_TX_DATA_2;
        end
        STATE_TX_DATA_2: begin
          state <= STATE_TX_DATA_3;
        end
        STATE_TX_DATA_3: begin
          state <= STATE_TX_DATA_4;
        end
        STATE_TX_DATA_4: begin
          if (spi_slave_core_rrdy && spi_slave_core_data_read) begin
            if (config_ram_si < arg_1_buf) begin
              state <= STATE_TX_DATA_0;
            end else begin
              state <= STATE_READ_OPTION;
            end
          end else begin
            state <= STATE_TX_DATA_4;
          end
        end
        STATE_INT_READ_LOW_BYTE: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_INT_READ_LOW_BYTE_READ;
          end else begin
            state <= STATE_INT_READ_LOW_BYTE;
          end
        end
        STATE_INT_READ_LOW_BYTE_READ: begin
          if (!spi_slave_core_rrdy) begin
            state <= STATE_INT_READ_HIGH_BYTE;
          end else begin
            state <= STATE_INT_READ_LOW_BYTE_READ;
          end
        end
        STATE_INT_READ_HIGH_BYTE: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_INT_READ_HIGH_BYTE_READ;
          end else begin
            state <= STATE_INT_READ_HIGH_BYTE;
          end
        end
        STATE_INT_READ_HIGH_BYTE_READ: begin
          if (!spi_slave_core_rrdy) begin
            state <= STATE_INT;
          end else begin
            state <= STATE_INT_READ_HIGH_BYTE_READ;
          end
        end
        STATE_INT: begin
          state <= STATE_READ_OPTION;
        end
        STATE_TRANS_READ_MODE: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_TRANS_READ_MODE_READ;
          end else begin
            state <= STATE_TRANS_READ_MODE;
          end
        end
        STATE_TRANS_READ_MODE_READ: begin
          if (!spi_slave_core_rrdy) begin
            state <= STATE_TRANS_READ_S_ADDR;
          end else begin
            state <= STATE_TRANS_READ_MODE_READ;
          end
        end
        STATE_TRANS_READ_S_ADDR: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_TRANS_READ_S_ADDR_READ;
          end else begin
            state <= STATE_TRANS_READ_S_ADDR;
          end
        end
        STATE_TRANS_READ_S_ADDR_READ: begin
          if (!spi_slave_core_rrdy) begin
            state <= STATE_TRANS_READ_E_ADDR;
          end else begin
            state <= STATE_TRANS_READ_S_ADDR_READ;
          end
        end
        STATE_TRANS_READ_E_ADDR: begin
          if (spi_slave_core_rrdy) begin
            state <= STATE_TRANS_READ_E_ADDR_READ;
          end else begin
            state <= STATE_TRANS_READ_E_ADDR;
          end
        end
        STATE_TRANS_READ_E_ADDR_READ: begin
          if (!spi_slave_core_rrdy) begin
            state <= STATE_TRANS_INT;
          end else begin
            state <= STATE_TRANS_READ_E_ADDR_READ;
          end
        end
        STATE_TRANS_INT: begin
          state <= STATE_READ_OPTION;
        end
        default: begin
          state <= STATE_RESET;
        end
      endcase
    end
  end

  always @(posedge clk) begin
    if (!rstn) begin
      config_ram_ci             <= 32'h0000_0000;
      config_ram_si             <= 32'h0000_0000;

      option_buf                <= 8'h00;
      arg_0_buf                 <= 8'h00;
      arg_1_buf                 <= 8'h00;

      trans_mode                <= 8'h00;
      trans_start_addr          <= 8'h00;
      trans_end_addr            <= 8'h00;

      spi_slave_core_address    <= 2'h0;
      spi_slave_core_data_in    <= 32'h0000_0000;
      spi_slave_core_data_read  <= 1'b0;
      spi_slave_core_data_write <= 1'b0;

      spi_config_ram_data_in    <= 8'h00;
      spi_config_ram_addr       <= 16'h0000;
      spi_config_ram_ce         <= 1'b0;
      spi_config_ram_we         <= 1'b0;

      int_out                   <= 16'h0;
      trans_int                 <= 1'b0;
    end else begin
      case (state)
        STATE_RESET: begin
          if (config_ram_ci < CONFIG_RAM_LENGTH - 1) begin
            config_ram_ci             <= config_ram_ci + 1;

            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            config_ram_ci             <= 32'h0000_0000;

            spi_slave_core_address    <= 2'h2;
            spi_slave_core_data_in    <= 32'h0000_0008;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b1;
          end
          config_ram_si          <= 32'h0000_0000;

          option_buf             <= 8'h00;
          arg_0_buf              <= 8'h00;
          arg_1_buf              <= 8'h00;

          trans_mode             <= 8'h00;
          trans_start_addr       <= 8'h00;
          trans_end_addr         <= 8'h00;

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= config_ram_ci;
          spi_config_ram_ce      <= 1'b1;
          spi_config_ram_we      <= 1'b1;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_READ_OPTION: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= 8'h00;
          arg_0_buf        <= 8'h00;
          arg_1_buf        <= 8'h00;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end

        STATE_READ_OPTION_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          if (spi_slave_core_data_read) begin
            option_buf <= spi_slave_core_data_out[7 : 0];
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= arg_1_buf;
          end else begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= arg_1_buf;
          end

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end

        STATE_READ_ADDR: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_READ_ADDR_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          if (spi_slave_core_data_read) begin
            option_buf <= option_buf;
            arg_0_buf  <= spi_slave_core_data_out[7 : 0];
            arg_1_buf  <= arg_1_buf;
          end else begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= arg_1_buf;
          end

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_READ_LENGTH: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_READ_LENGTH_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          if (spi_slave_core_data_read) begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= spi_slave_core_data_out[7 : 0];
          end else begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= arg_1_buf;
          end

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_RX_DATA_0: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= config_ram_si;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_RX_DATA_1: begin
          config_ram_ci             <= 32'h0000_0000;

          option_buf                <= option_buf;
          arg_0_buf                 <= arg_0_buf;
          arg_1_buf                 <= arg_1_buf;

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          if (spi_slave_core_data_read) begin
            config_ram_si          <= config_ram_si + 1;

            spi_config_ram_data_in <= spi_slave_core_data_out[7 : 0];
            spi_config_ram_addr    <= {arg_0_buf, config_ram_si[7 : 0]};
            spi_config_ram_ce      <= 1'b1;
            spi_config_ram_we      <= 1'b1;
          end else begin
            config_ram_si          <= config_ram_si;

            spi_config_ram_data_in <= 8'h0;
            spi_config_ram_addr    <= 16'h0;
            spi_config_ram_ce      <= 1'b0;
            spi_config_ram_we      <= 1'b0;
          end

          int_out   <= 16'h0;
          trans_int <= 1'b0;
        end
        STATE_TX_DATA_0: begin
          config_ram_ci             <= 32'h0000_0000;
          config_ram_si             <= config_ram_si;

          option_buf                <= option_buf;
          arg_0_buf                 <= arg_0_buf;
          arg_1_buf                 <= arg_1_buf;

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= {arg_0_buf, config_ram_si[7 : 0]};
          spi_config_ram_ce         <= 1'b1;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_TX_DATA_1: begin
          config_ram_ci             <= 32'h0000_0000;
          config_ram_si             <= config_ram_si;

          option_buf                <= option_buf;
          arg_0_buf                 <= arg_0_buf;
          arg_1_buf                 <= arg_1_buf;

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_TX_DATA_2: begin
          config_ram_ci             <= 32'h0000_0000;
          config_ram_si             <= config_ram_si + 1;

          option_buf                <= option_buf;
          arg_0_buf                 <= arg_0_buf;
          arg_1_buf                 <= arg_1_buf;

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= {24'b0, spi_config_ram_data_out};
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b1;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_TX_DATA_3: begin
          config_ram_ci             <= 32'h0000_0000;
          config_ram_si             <= config_ram_si;

          option_buf                <= option_buf;
          arg_0_buf                 <= arg_0_buf;
          arg_1_buf                 <= arg_1_buf;

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_TX_DATA_4: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= config_ram_si;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end
          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_INT_READ_LOW_BYTE: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_INT_READ_LOW_BYTE_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          if (spi_slave_core_data_read) begin
            option_buf <= option_buf;
            arg_0_buf  <= spi_slave_core_data_out[7 : 0];
            arg_1_buf  <= arg_1_buf;
          end else begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= arg_1_buf;
          end

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_INT_READ_HIGH_BYTE: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_INT_READ_HIGH_BYTE_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          if (spi_slave_core_data_read) begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= spi_slave_core_data_out[7 : 0];
          end else begin
            option_buf <= option_buf;
            arg_0_buf  <= arg_0_buf;
            arg_1_buf  <= arg_1_buf;
          end

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_INT: begin
          config_ram_ci          <= 32'h0000_0000;
          config_ram_si          <= 32'h0000_0000;

          option_buf             <= option_buf;
          arg_0_buf              <= arg_0_buf;
          arg_1_buf              <= arg_1_buf;

          trans_mode             <= trans_mode;
          trans_start_addr       <= trans_start_addr;
          trans_end_addr         <= trans_end_addr;

          spi_slave_core_address <= 2'h0;
          spi_slave_core_data_in <= 32'h0000_0000;

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= {arg_1_buf, arg_0_buf};
          trans_int              <= 1'b0;
        end
        STATE_TRANS_READ_MODE: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
        end
        STATE_TRANS_READ_MODE_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          option_buf <= option_buf;
          arg_0_buf <= arg_0_buf;
          arg_1_buf <= arg_1_buf;

          if (spi_slave_core_data_read) begin
            trans_mode       <= spi_slave_core_data_out[7 : 0];
            trans_start_addr <= trans_start_addr;
            trans_end_addr   <= trans_end_addr;
          end else begin
            trans_mode       <= trans_mode;
            trans_start_addr <= trans_start_addr;
            trans_end_addr   <= trans_end_addr;
          end

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
        end
        STATE_TRANS_READ_S_ADDR: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
          trans_int              <= 1'b0;
        end
        STATE_TRANS_READ_S_ADDR_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          option_buf <= option_buf;
          arg_0_buf <= arg_0_buf;
          arg_1_buf <= arg_1_buf;

          if (spi_slave_core_data_read) begin
            trans_mode       <= trans_mode;
            trans_start_addr <= spi_slave_core_data_out[7 : 0];
            trans_end_addr   <= trans_end_addr;
          end else begin
            trans_mode       <= trans_mode;
            trans_start_addr <= trans_start_addr;
            trans_end_addr   <= trans_end_addr;
          end

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
          trans_int                 <= 1'b0;
        end
        STATE_TRANS_READ_E_ADDR: begin
          config_ram_ci    <= 32'h0000_0000;
          config_ram_si    <= 32'h0000_0000;

          option_buf       <= option_buf;
          arg_0_buf        <= arg_0_buf;
          arg_1_buf        <= arg_1_buf;

          trans_mode       <= trans_mode;
          trans_start_addr <= trans_start_addr;
          trans_end_addr   <= trans_end_addr;

          if (spi_slave_core_rrdy) begin
            spi_slave_core_address    <= 2'h1;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b1;
            spi_slave_core_data_write <= 1'b0;
          end else begin
            spi_slave_core_address    <= 2'h0;
            spi_slave_core_data_in    <= 32'h0000_0000;
            spi_slave_core_data_read  <= 1'b0;
            spi_slave_core_data_write <= 1'b0;
          end

          spi_config_ram_data_in <= 8'h00;
          spi_config_ram_addr    <= 16'h0000;
          spi_config_ram_ce      <= 1'b0;
          spi_config_ram_we      <= 1'b0;

          int_out                <= 16'h0;
          trans_int              <= 1'b0;
          trans_int              <= 1'b0;
        end
        STATE_TRANS_READ_E_ADDR_READ: begin
          config_ram_ci <= 32'h0000_0000;
          config_ram_si <= 32'h0000_0000;

          option_buf <= option_buf;
          arg_0_buf <= arg_0_buf;
          arg_1_buf <= arg_1_buf;

          if (spi_slave_core_data_read) begin
            trans_mode       <= trans_mode;
            trans_start_addr <= trans_start_addr;
            trans_end_addr   <= spi_slave_core_data_out[7 : 0];
          end else begin
            trans_mode       <= trans_mode;
            trans_start_addr <= trans_start_addr;
            trans_end_addr   <= trans_end_addr;
          end

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
          trans_int                 <= 1'b0;
        end
        STATE_TRANS_INT: begin
          config_ram_ci             <= 32'h0000_0000;
          config_ram_si             <= 32'h0000_0000;

          option_buf                <= 8'h00;
          arg_0_buf                 <= 8'h00;
          arg_1_buf                 <= 8'h00;

          trans_mode                <= trans_mode;
          trans_start_addr          <= trans_start_addr;
          trans_end_addr            <= trans_end_addr;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
          trans_int                 <= 1'b1;
        end
        default: begin
          config_ram_ci             <= 32'h0000_0000;
          config_ram_si             <= 32'h0000_0000;

          option_buf                <= 8'h00;
          arg_0_buf                 <= 8'h00;
          arg_1_buf                 <= 8'h00;

          trans_mode                <= 8'h00;
          trans_start_addr          <= 8'h00;
          trans_end_addr            <= 8'h00;

          spi_slave_core_address    <= 2'h0;
          spi_slave_core_data_in    <= 32'h0000_0000;
          spi_slave_core_data_read  <= 1'b0;
          spi_slave_core_data_write <= 1'b0;

          spi_config_ram_data_in    <= 8'h00;
          spi_config_ram_addr       <= 16'h0000;
          spi_config_ram_ce         <= 1'b0;
          spi_config_ram_we         <= 1'b0;

          int_out                   <= 16'h0;
          trans_int                 <= 1'b0;
          trans_int                 <= 1'b0;
        end
      endcase
    end
  end

  reg [ 7 : 0] trans_mode_buf;
  reg [15 : 0] trans_start_addr_buf;
  reg [15 : 0] trans_end_addr_buf;
  always @(posedge clk) begin
    if (!rstn) begin
      trans_mode_buf       <= 8'h00;
      trans_start_addr_buf <= 8'h00;
      trans_end_addr_buf   <= 8'h00;
    end else begin
      if (trans_int) begin
        trans_mode_buf       <= trans_mode;
        trans_start_addr_buf <= {trans_start_addr, 8'h0};
        trans_end_addr_buf   <= {trans_end_addr, 8'h0};
      end else begin
        trans_mode_buf       <= trans_mode_buf;
        trans_start_addr_buf <= trans_start_addr_buf;
        trans_end_addr_buf   <= trans_end_addr_buf;
      end
    end
  end

  localparam TRANS_MODE_RESET = 8'h00;
  localparam TRANS_MODE_CONTINUES = 8'h01;
  localparam TRANS_MODE_KEEP = 8'h02;

  reg [31 : 0] trans_ram_ci;
  reg [31 : 0] trans_ram_si;
  localparam TRANS_RAM_LENGTH = 32'd65536;

  reg [7 : 0] trans_state;
  localparam TRANS_STATE_RESET = 8'h00;
  localparam TRANS_STATE_READ_MODE = 8'h01;
  localparam TRANS_STATE_WRITE_DATA_0 = 8'h02;
  localparam TRANS_STATE_WRITE_DATA_1 = 8'h03;
  localparam TRANS_STATE_WRITE_DATA_2 = 8'h04;

  always @(posedge clk) begin
    if (!rstn) begin
      trans_state <= TRANS_STATE_RESET;
    end else begin
      case (trans_state)
        TRANS_STATE_RESET: begin
          if (trans_ram_ci < TRANS_RAM_LENGTH - 1) begin
            trans_state <= TRANS_STATE_RESET;
          end else begin
            trans_state <= TRANS_STATE_READ_MODE;
          end
        end
        TRANS_STATE_READ_MODE: begin
          case (trans_mode_buf)
            TRANS_MODE_RESET:     trans_state <= TRANS_STATE_READ_MODE;
            TRANS_MODE_CONTINUES: trans_state <= TRANS_STATE_WRITE_DATA_0;
            TRANS_MODE_KEEP:      trans_state <= TRANS_STATE_WRITE_DATA_0;
            default:              trans_state <= TRANS_STATE_READ_MODE;
          endcase
        end
        TRANS_STATE_WRITE_DATA_0: begin
          trans_state <= TRANS_STATE_WRITE_DATA_1;
        end
        TRANS_STATE_WRITE_DATA_1: begin
          if (spi_master_core_trdy) begin
            trans_state <= TRANS_STATE_WRITE_DATA_2;
          end else begin
            trans_state <= TRANS_STATE_WRITE_DATA_1;
          end
        end
        TRANS_STATE_WRITE_DATA_2: begin
          if (!spi_master_core_trdy) begin
            if (trans_ram_si < trans_end_addr_buf - 1) begin
              trans_state <= TRANS_STATE_WRITE_DATA_0;
            end else begin
              trans_state <= TRANS_STATE_READ_MODE;
            end
          end else begin
            trans_state <= TRANS_STATE_WRITE_DATA_2;
          end
        end
        default: begin
          trans_state <= TRANS_STATE_RESET;
        end
      endcase
    end
  end

  always @(posedge clk) begin
    if (!rstn) begin
      trans_ram_ci               <= 32'h0;
      trans_ram_si               <= 32'h0;

      spi_master_core_address    <= 2'b0;
      spi_master_core_data_in    <= 32'h0;
      spi_master_core_data_read  <= 1'b0;
      spi_master_core_data_write <= 1'b0;

      spi_trans_ram_data_in      <= 8'h00;
      spi_trans_ram_addr         <= 16'h0000;
      spi_trans_ram_ce           <= 1'b0;
      spi_trans_ram_we           <= 1'b0;
    end else begin
      case (trans_state)
        TRANS_STATE_RESET: begin
          if (config_ram_ci < CONFIG_RAM_LENGTH - 1) begin
            trans_ram_ci <= trans_ram_ci + 1;
          end else begin
            trans_ram_ci <= 32'h0;
          end
          trans_ram_si               <= 32'h0;

          spi_master_core_address    <= 2'h0;
          spi_master_core_data_in    <= 32'h0000_0000;
          spi_master_core_data_read  <= 1'b0;
          spi_master_core_data_write <= 1'b0;

          spi_trans_ram_data_in      <= 8'h00;
          spi_trans_ram_addr         <= 16'h0000;
          spi_trans_ram_ce           <= 1'b0;
          spi_trans_ram_we           <= 1'b0;
        end
        TRANS_STATE_READ_MODE: begin
          trans_ram_ci               <= 32'h0;
          trans_ram_si               <= trans_start_addr_buf;

          spi_master_core_address    <= 2'b0;
          spi_master_core_data_in    <= 32'h0;
          spi_master_core_data_read  <= 1'b0;
          spi_master_core_data_write <= 1'b0;

          spi_trans_ram_data_in      <= 8'h00;
          spi_trans_ram_addr         <= 16'h0000;
          spi_trans_ram_ce           <= 1'b0;
          spi_trans_ram_we           <= 1'b0;
        end
        TRANS_STATE_WRITE_DATA_0: begin
          trans_ram_ci               <= 32'h0;
          trans_ram_si               <= trans_ram_si;

          spi_master_core_address    <= 2'b0;
          spi_master_core_data_in    <= 32'h0;
          spi_master_core_data_read  <= 1'b0;
          spi_master_core_data_write <= 1'b0;

          spi_trans_ram_data_in      <= 8'h00;
          spi_trans_ram_addr         <= trans_ram_si;
          spi_trans_ram_ce           <= 1'b1;
          spi_trans_ram_we           <= 1'b0;
        end
        TRANS_STATE_WRITE_DATA_1: begin
          trans_ram_ci <= 32'h0;
          trans_ram_si <= trans_ram_si;

          if (spi_master_core_trdy) begin
            spi_master_core_address    <= 2'b0;
            spi_master_core_data_in    <= spi_trans_ram_data_out;
            spi_master_core_data_read  <= 1'b0;
            spi_master_core_data_write <= 1'b1;
          end else if (spi_master_core_rrdy) begin
            spi_master_core_address    <= 2'b1;
            spi_master_core_data_in    <= 32'h0;
            spi_master_core_data_read  <= 1'b1;
            spi_master_core_data_write <= 1'b0;
          end else begin
            spi_master_core_address    <= 2'b0;
            spi_master_core_data_in    <= 32'h0;
            spi_master_core_data_read  <= 1'b0;
            spi_master_core_data_write <= 1'b0;
          end

          spi_trans_ram_data_in <= 8'h00;
          spi_trans_ram_addr    <= 8'h0;
          spi_trans_ram_ce      <= 1'b0;
          spi_trans_ram_we      <= 1'b0;
        end
        TRANS_STATE_WRITE_DATA_2: begin
          trans_ram_ci <= 32'h0;
          if (!spi_master_core_trdy) begin
            if (trans_ram_si < trans_end_addr_buf - 1) begin
              trans_ram_si <= trans_ram_si + 1;
            end else begin
              trans_ram_si <= 32'h0;
            end
          end else begin
            trans_ram_si <= trans_ram_si;
          end

          spi_master_core_address    <= 2'b0;
          spi_master_core_data_in    <= 32'h0;
          spi_master_core_data_read  <= 1'b0;
          spi_master_core_data_write <= 1'b0;

          spi_trans_ram_data_in      <= 8'h00;
          spi_trans_ram_addr         <= 8'h00;
          spi_trans_ram_ce           <= 1'b0;
          spi_trans_ram_we           <= 1'b0;
        end
        default: begin
          trans_ram_ci               <= 32'h0;
          trans_ram_si               <= 32'h0;

          spi_master_core_address    <= 2'b0;
          spi_master_core_data_in    <= 32'h0;
          spi_master_core_data_read  <= 1'b0;
          spi_master_core_data_write <= 1'b0;

          spi_trans_ram_data_in      <= 8'h00;
          spi_trans_ram_addr         <= 16'h0000;
          spi_trans_ram_ce           <= 1'b0;
          spi_trans_ram_we           <= 1'b0;
        end
      endcase
    end
  end

endmodule
