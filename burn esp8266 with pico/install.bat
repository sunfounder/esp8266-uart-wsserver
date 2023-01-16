@REM esptool.exe -help

esptool.exe --chip esp8266 --before no_reset_no_sync erase_flash

esptool.exe --chip esp8266 --before no_reset_no_sync write_flash 0 "esp8266-uart-wsserver-v1.1.0.bin"

@REM esptool.exe --chip esp8266 --before default_reset erase_flash

@REM esptool.exe --chip esp8266 --before default_reset write_flash 0 "esp8266-uart-wsserver-v1.1.0.bin"

pause

