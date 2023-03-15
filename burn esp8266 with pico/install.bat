@REM esptool.exe -help

@REM set firmware=%1
set firmware=esp8266-uart-wsserver-v2.0.0.bin

esptool.exe --chip esp8266 --before no_reset_no_sync erase_flash
esptool.exe --chip esp8266 --before no_reset_no_sync write_flash 0 %firmware%

@REM esptool.exe --chip esp8266 --before default_reset erase_flash
@REM esptool.exe --chip esp8266 --before default_reset write_flash 0 "%firmware%"

pause

