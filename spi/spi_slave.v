module spi_slave #(
    parameter CPOL  = 0,  // Clock Polarity, 0 and 1 allowed
    parameter CPHA  = 0,  // Clock Phase, 0 and 1 allowed
    parameter FSB   = 1,  // First bit, 0-LSB, 1-MSB
    parameter WIDTH = 8   // Data bit width
) (
    input clk,
    input rst_n,

    input      spi_sclk,
    input      spi_ss_n,
    input      spi_mosi,
    output reg spi_miso,

    output reg                 spi_valid,
    input                      spi_ready,
    output     [WIDTH - 1 : 0] spi_rx_data,
    input      [WIDTH - 1 : 0] spi_tx_data
);
  // Generate Edge Interrupt
  reg  spi_sclk_0;
  reg  spi_sclk_1;
  wire spi_sclk_edge_int = (spi_sclk_0 != spi_sclk_1) ? 1'b1 : 1'b0;
  always @(posedge clk) begin
    if (!rst_n) begin
      spi_sclk_0 <= 1'b0;
      spi_sclk_1 <= 1'b0;
    end else begin
      spi_sclk_0 <= spi_sclk;
      spi_sclk_1 <= spi_sclk_0;
    end
  end

  // Generate Sample Start Interrupt
  reg  spi_sclk_sample_cnt;
  wire spi_sclk_sample_int = ((spi_sclk_sample_cnt == CPHA) && (spi_sclk_edge_int)) ? 1'b1 : 1'b0;
  always @(posedge clk) begin
    if (!rst_n) begin
      spi_sclk_sample_cnt <= 1'b0;
    end else begin
      if (spi_sclk_edge_int) begin
        spi_sclk_sample_cnt <= spi_sclk_sample_cnt + 1'b1;
      end else begin
        spi_sclk_sample_cnt <= spi_sclk_sample_cnt;
      end
    end
  end

  // Recive and transmit counter
  reg [7 : 0] spi_data_cnt;
  always @(posedge clk) begin
    if (!rst_n) begin
      spi_data_cnt <= 8'h0;
    end else begin
      if (spi_sclk_sample_int && (!spi_ss_n)) begin
        if (spi_data_cnt < WIDTH - 1) begin
          spi_data_cnt <= spi_data_cnt + 1'b1;
        end else begin
          spi_data_cnt <= 8'h0;
        end
      end else begin
        spi_data_cnt <= spi_data_cnt;
      end
    end
  end

  reg [WIDTH - 1 : 0] tx_data_buffer;
  reg [WIDTH - 1 : 0] rx_data_buffer;

  // Recive and transmit data
  always @(posedge clk) begin
    if (!rst_n) begin
      rx_data_buffer <= {WIDTH{1'b0}};
      spi_miso <= 1'b0;
    end else begin
      if (spi_sclk_sample_int) begin
        rx_data_buffer[(FSB==0)?spi_data_cnt : WIDTH-1-spi_data_cnt] <= spi_mosi;
        spi_miso <= tx_data_buffer[(FSB==0)?spi_data_cnt : WIDTH-1-spi_data_cnt];
      end else begin
        rx_data_buffer[spi_data_cnt] <= rx_data_buffer[spi_data_cnt];
        spi_miso <= spi_miso;
      end
    end
  end

  // Generate spi_valid Signal
  always @(posedge clk) begin
    if (!rst_n) begin
      spi_valid <= 1'b0;
    end else begin
      if ((spi_data_cnt == WIDTH - 1) && (spi_sclk_sample_int)) begin
        spi_valid <= 1'b1;
      end else if ((spi_valid == 1) && spi_ready) begin
        spi_valid <= 1'b0;
      end else begin
        spi_valid <= spi_valid;
      end
    end
  end

  // user interface
  assign spi_rx_data = (spi_ready & spi_valid) ? rx_data_buffer : {WIDTH{1'b0}};
  always @(posedge clk) begin
    if (!rst_n) begin
      tx_data_buffer <= {WIDTH{1'b0}};
    end else begin
      if (spi_ready && spi_valid) begin
        tx_data_buffer <= spi_tx_data;
      end else begin
        tx_data_buffer <= tx_data_buffer;
      end
    end
  end

endmodule
