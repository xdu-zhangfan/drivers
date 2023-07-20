make -j64
openocd \
-f interface/cmsis-dap.cfg \
-f target/stm32f4x.cfg \
-c "init" \
-c "transport select swd" \
-c "halt" \
-c "flash write_image erase build/hmc830.hex" \
-c "reset" \
-c "shutdown"