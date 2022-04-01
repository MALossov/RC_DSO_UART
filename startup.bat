openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg -c "init;reset init;flash erase_address 0x08000000 0x20000;"
pause