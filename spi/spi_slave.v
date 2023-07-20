module spi_slave (
    input clk,
    input rstn,

    input      spi_sclk,
    input      spi_ss_n,
    input      spi_mosi,
    output reg spi_miso,

    output reg         spi_valid,
    input              spi_ready,
    output reg [7 : 0] spi_rx_data,
    input      [7 : 0] spi_tx_data
);
  reg sck_i0;
  reg sck_i1;
  reg rt_edge_int;
  always @(posedge clk) begin
    if (!rstn) begin
      sck_i0      <= 1'b0;
      sck_i1      <= 1'b0;
      rt_edge_int <= 1'b0;
    end else begin
      if (!spi_ss_n) begin
        sck_i0 <= spi_sclk;
        sck_i1 <= sck_i0;
        if (sck_i0 != sck_i1) begin
          rt_edge_int <= 1'b1;
        end else begin
          rt_edge_int <= 1'b0;
        end
      end else begin
        sck_i0      <= 1'b0;
        sck_i1      <= 1'b0;
        rt_edge_int <= 1'b0;
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
  localparam STATE_RETDATA = 8'h04;
  always @(posedge clk) begin
    if (!rstn) begin
      state          <= STATE_RESET;
      tx_cnt         <= 8'h0;
      rx_cnt         <= 8'h0;

      spi_valid      <= 1'b0;
      rx_data_buffer <= 8'h0;
      tx_data_buffer <= 8'h0;

      spi_miso       <= 1'b0;
    end else begin
      case (state)
        STATE_RESET: begin
          state          <= STATE_IDEL;
          tx_cnt         <= 8'h0;
          rx_cnt         <= 8'h0;

          spi_valid      <= 1'b0;
          rx_data_buffer <= 8'h0;
          tx_data_buffer <= 8'h0;

          spi_miso       <= 1'b0;
        end

        STATE_IDEL: begin
          tx_cnt <= 8'h0;
          rx_cnt <= 8'h0;

          if (!spi_ss_n) begin
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

          spi_miso       <= 1'b0;
        end

        STATE_EDGE_1: begin
          spi_valid      <= 1'b0;
          tx_data_buffer <= tx_data_buffer;
          spi_rx_data    <= 8'h0;

          if (rt_edge_int) begin
            state          <= STATE_EDGE_2;

            rx_data_buffer <= {rx_data_buffer[6 : 0], spi_mosi};
            rx_cnt         <= rx_cnt + 1;
            tx_cnt         <= tx_cnt;

          end else begin
            state          <= STATE_EDGE_1;

            rx_data_buffer <= rx_data_buffer;
            rx_cnt         <= rx_cnt;
            tx_cnt         <= tx_cnt;
          end

          spi_miso <= tx_data_buffer[7 - tx_cnt];
        end

        STATE_EDGE_2: begin
          spi_valid      <= 1'b0;
          tx_data_buffer <= tx_data_buffer;
          spi_rx_data    <= 8'h0;

          if (rt_edge_int) begin
            if (tx_cnt < 7) begin
              state  <= STATE_EDGE_1;
              rx_cnt <= rx_cnt + 1;
            end else begin
              state  <= STATE_IDEL;
              rx_cnt <= 8'h0;
            end

            rx_data_buffer <= rx_data_buffer;
            rx_cnt         <= rx_cnt;
            tx_cnt         <= tx_cnt + 1;
          end else begin
            state          <= STATE_EDGE_2;

            rx_data_buffer <= rx_data_buffer;
            rx_cnt         <= rx_cnt;
            tx_cnt         <= tx_cnt;
          end

          spi_miso <= tx_data_buffer[7 - tx_cnt];
        end

        default: begin

        end
      endcase
    end
  end

endmodule
