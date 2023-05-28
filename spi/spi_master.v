module spi_master #(
    parameter SYSCLK_FREQ = 50000000,  // System clock frequency, default 50MHz
    parameter SPICLK_FREQ = 1000000,   // SPI clock frequency, default 1MHz

    parameter CPOL  = 0,  // Clock Polarity, 0 and 1 allowed
    parameter CPHA  = 0,  // Clock Phase, 0 and 1 allowed
    parameter FSB   = 1,  // First bit, 0-LSB, 1-MSB
    parameter WIDTH = 8   // Data bit width
) (
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
);
  reg [WIDTH - 1 : 0] rx_data_buffer;
  reg [WIDTH - 1 : 0] tx_data_buffer;

  reg [15 : 0] spi_clk_cnt;
  reg [15 : 0] spi_data_cnt;

  reg [7 : 0] state;
  localparam STATE_RESET = 8'h00;
  localparam STATE_IDEL = 8'h01;
  localparam STATE_TR = 8'h2;
  always @(posedge clk) begin
    if (!rstn) begin
      state <= STATE_RESET;
    end else begin
      case (state)
        STATE_RESET: begin
          state <= STATE_IDEL;
        end

        STATE_IDEL: begin
          if (spi_valid && spi_ready) begin
            state <= STATE_TR;
          end else begin
            state <= STATE_IDEL;
          end
        end

        STATE_TR: begin
          if ((spi_data_cnt == WIDTH - 1) && (spi_clk_cnt == SYSCLK_FREQ / SPICLK_FREQ / 2 - 1) && (spi_sclk != CPOL)) begin
            state <= STATE_IDEL;
          end else begin
            state <= STATE_TR;
          end
        end

        default: begin
          state <= STATE_RESET;
        end

      endcase
    end
  end

  always @(posedge clk) begin
    if (!rstn) begin
      rx_data_buffer <= {WIDTH{1'b0}};
      tx_data_buffer <= {WIDTH{1'b0}};

      spi_clk_cnt    <= 16'h0;
      spi_data_cnt   <= 16'h0;

      spi_sclk       <= {1{CPOL}};
      spi_ss_n       <= 1'b0;

      spi_valid      <= 1'b0;

    end else begin
      case (state)
        STATE_RESET: begin
          rx_data_buffer <= {WIDTH{1'b0}};
          tx_data_buffer <= {WIDTH{1'b0}};

          spi_clk_cnt    <= 16'h0;
          spi_data_cnt   <= 16'h0;

          spi_sclk       <= {1{CPOL}};
          spi_ss_n       <= 1'b1;

          spi_valid      <= 1'b0;
        end

        STATE_IDEL: begin
          rx_data_buffer <= rx_data_buffer;
          spi_valid      <= 1'b1;

          spi_clk_cnt    <= 16'h0;
          spi_data_cnt   <= 16'h0;

          spi_sclk       <= {1{CPOL}};
          spi_ss_n       <= 1'b1;

          if (spi_valid && spi_ready) begin
            tx_data_buffer <= spi_tx_data;
          end else begin
            tx_data_buffer <= {WIDTH{1'b0}};
          end
        end

        STATE_TR: begin
          spi_valid <= 1'b0;
          tx_data_buffer <= tx_data_buffer;

          spi_ss_n <= 1'b0;

          if (spi_clk_cnt < SYSCLK_FREQ / SPICLK_FREQ / 2) begin
            spi_clk_cnt <= spi_clk_cnt + 1;

            spi_sclk <= spi_sclk;
            spi_data_cnt <= spi_data_cnt;

            rx_data_buffer <= rx_data_buffer;
          end else begin
            spi_clk_cnt <= 16'h0;

            spi_sclk <= ~spi_sclk;

            if (CPOL == CPHA) begin
              if (spi_sclk != CPHA) begin
                spi_data_cnt   <= spi_data_cnt + 1;
                rx_data_buffer <= rx_data_buffer;
              end else begin
                spi_data_cnt <= spi_data_cnt;
                if (FSB) begin
                  rx_data_buffer <= {spi_miso, rx_data_buffer[WIDTH-1:1]};
                end else begin
                  rx_data_buffer <= {rx_data_buffer[WIDTH-2:0], spi_miso};
                end
              end
            end else begin
              if (spi_sclk == CPHA) begin
                spi_data_cnt   <= spi_data_cnt + 1;
                rx_data_buffer <= rx_data_buffer;
              end else begin
                spi_data_cnt <= spi_data_cnt;
                if (FSB) begin
                  rx_data_buffer <= {spi_miso, rx_data_buffer[WIDTH-1:1]};
                end else begin
                  rx_data_buffer <= {rx_data_buffer[WIDTH-2:0], spi_miso};
                end
              end
            end
          end
        end

        default: begin
          rx_data_buffer <= {WIDTH{1'b0}};
          tx_data_buffer <= {WIDTH{1'b0}};

          spi_clk_cnt    <= 16'h0;
          spi_data_cnt   <= 16'h0;

          spi_sclk       <= {1{CPOL}};
          spi_ss_n       <= 1'b1;

          spi_valid      <= 1'b0;
        end

      endcase
    end
  end

  assign spi_rx_data = (spi_valid && spi_ready) ? rx_data_buffer : {WIDTH{1'b0}};
  assign spi_mosi = (!spi_ss_n) ? tx_data_buffer[(FSB)?spi_data_cnt : WIDTH-1-spi_data_cnt] : 1'b0;

endmodule
