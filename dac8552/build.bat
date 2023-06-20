make -j64
openocd ^
-f interface/cmsis-dap.cfg ^
-f target/stm32h7x.cfg ^
-c "init" ^
-c "transport select swd" ^
-c "halt" ^
-c "flash erase_sector 0 0 last" ^
-c "flash write_image build/dac8552.hex" ^
-c "reset" ^
-c "shutdown"