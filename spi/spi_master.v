module spi_master (
    input clk,
    input rstn,

    output reg spi_sclk,
    output reg spi_ss_n,
    output reg spi_mosi,
    input      spi_miso,

    input          param_wen,
    input [31 : 0] spi_period,

    output reg         spi_valid,
    input              spi_ready,
    output reg [7 : 0] spi_rx_data,
    input      [7 : 0] spi_tx_data
);
  reg [31 : 0] spi_period_buf;
  always @(posedge clk) begin
    if (!rstn) begin
      spi_period_buf <= 32'h0;
    end else begin
      if (param_wen) begin
        spi_period_buf <= spi_period;
      end else begin
        spi_period_buf <= spi_period_buf;
      end
    end
  end

  reg [31 : 0] rt_cnt;
  reg          rt_cnt_dv;
  reg          rt_cnt_int;
  always @(posedge clk) begin
    if (!rstn) begin
      rt_cnt     <= 32'h0;
      rt_cnt_int <= 1'b0;
    end else begin
      if (rt_cnt_dv) begin
        if (rt_cnt < spi_period_buf) begin
          rt_cnt     <= rt_cnt + 1;
          rt_cnt_int <= 1'b0;
        end else begin
          rt_cnt     <= 32'h0;
          rt_cnt_int <= 1'b1;
        end
      end else begin
        rt_cnt     <= 32'h0;
        rt_cnt_int <= 1'b0;
      end
    end
  end

  reg [7 : 0] rx_data_buffer;
  reg [7 : 0] tx_data_buffer;

  reg [7 : 0] state;
  reg [7 : 0] tx_cnt;
  reg [7 : 0] rx_cnt;
  localparam STATE_RESET = 8'h00;
  localparam STATE_IDEL = 8'h01;
  localparam STATE_EDGE_1 = 8'h02;
  localparam STATE_EDGE_2 = 8'h03;
  localparam STATE_SS_N_DELAY = 8'h04;
  always @(posedge clk) begin
    if (!rstn) begin
      state          <= STATE_RESET;
      tx_cnt         <= 8'h0;
      rx_cnt         <= 8'h0;

      spi_valid      <= 1'b0;
      rx_data_buffer <= 8'h0;
      tx_data_buffer <= 8'h0;

      spi_sclk       <= 1'b0;
      spi_ss_n       <= 1'b1;
      spi_mosi       <= 1'b0;

      rt_cnt_dv      <= 1'b0;
    end else begin
      case (state)
        STATE_RESET: begin
          state          <= STATE_IDEL;
          tx_cnt         <= 8'h0;
          rx_cnt         <= 8'h0;

          spi_valid      <= 1'b0;
          rx_data_buffer <= 8'h0;
          tx_data_buffer <= 8'h0;

          spi_sclk       <= 1'b0;
          spi_ss_n       <= 1'b1;
          spi_mosi       <= 1'b0;

          rt_cnt_dv      <= 1'b0;
        end

        STATE_IDEL: begin
          tx_cnt <= 8'h0;
          rx_cnt <= 8'h0;

          if (spi_ready) begin
            state          <= STATE_EDGE_1;

            tx_data_buffer <= spi_tx_data;
            spi_rx_data    <= rx_data_buffer;
          end else begin
            state          <= STATE_IDEL;

            tx_data_buffer <= 8'h0;
            spi_rx_data    <= 8'h0;
          end

          spi_valid      <= 1'b1;

          rx_data_buffer <= rx_data_buffer;

          spi_sclk       <= 1'b0;
          spi_ss_n       <= 1'b1;
          spi_mosi       <= 1'b0;

          rt_cnt_dv      <= 1'b0;
        end

        STATE_EDGE_1: begin
          spi_valid      <= 1'b0;
          tx_data_buffer <= tx_data_buffer;
          spi_rx_data    <= 8'h0;

          if (rt_cnt_int) begin
            state          <= STATE_EDGE_2;

            spi_sclk       <= ~spi_sclk;

            rx_data_buffer <= {rx_data_buffer[6 : 0], spi_miso};
            rx_cnt         <= rx_cnt + 1;
            tx_cnt         <= tx_cnt;

          end else begin
            state          <= STATE_EDGE_1;

            spi_sclk       <= spi_sclk;

            rx_data_buffer <= rx_data_buffer;
            rx_cnt         <= rx_cnt;
            tx_cnt         <= tx_cnt;
          end

          spi_ss_n  <= 1'b0;
          spi_mosi  <= tx_data_buffer[7 - tx_cnt];

          rt_cnt_dv <= 1'b1;
        end

        STATE_EDGE_2: begin
          spi_valid      <= 1'b0;
          tx_data_buffer <= tx_data_buffer;
          spi_rx_data    <= 8'h0;

          if (rt_cnt_int) begin
            if (tx_cnt < 7) begin
              state  <= STATE_EDGE_1;
              rx_cnt <= rx_cnt + 1;
            end else begin
              state  <= STATE_SS_N_DELAY;
              rx_cnt <= 8'h0;
            end

            rx_data_buffer <= rx_data_buffer;
            rx_cnt         <= rx_cnt;
            tx_cnt         <= tx_cnt + 1;

            spi_sclk       <= ~spi_sclk;
          end else begin
            state          <= STATE_EDGE_2;

            spi_sclk       <= spi_sclk;

            rx_data_buffer <= rx_data_buffer;
            rx_cnt         <= rx_cnt;
            tx_cnt         <= tx_cnt;
          end

          spi_ss_n  <= 1'b0;
          spi_mosi  <= tx_data_buffer[7 - tx_cnt];

          rt_cnt_dv <= 1'b1;
        end

        STATE_SS_N_DELAY: begin
          tx_cnt <= 8'h0;
          rx_cnt <= 8'h0;

          if (rt_cnt_int) begin
            state       <= STATE_IDEL;
          end else begin
            state       <= STATE_SS_N_DELAY;
          end
          tx_data_buffer <= tx_data_buffer;

          spi_valid      <= 1'b0;
          spi_rx_data    <= 8'h0;

          rx_data_buffer <= rx_data_buffer;

          spi_sclk       <= 1'b0;
          spi_ss_n       <= 1'b0;
          spi_mosi       <= 1'b0;

          rt_cnt_dv      <= 1'b1;
        end

        default: begin
          state          <= STATE_IDEL;
          tx_cnt         <= 8'h0;
          rx_cnt         <= 8'h0;

          spi_valid      <= 1'b0;
          rx_data_buffer <= 8'h0;
          tx_data_buffer <= 8'h0;

          spi_sclk       <= 1'b0;
          spi_ss_n       <= 1'b1;
          spi_mosi       <= 1'b0;

          rt_cnt_dv      <= 1'b0;
        end
      endcase
    end
  end

endmodule
