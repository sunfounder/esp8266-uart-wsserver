# Change Log


## [2.0.0] - 2023-3-14

### Fixed
- Fix bug of the serial reading data loss 
- Fix the bug that the main loop has a serious delay
- Fix the bug of not executing close() before WiFi reconnection
- Fix the bug that websocket didn't execute close() before restarting

### Added
- Add ping pong for websocket communication
- Add led enable fuctions
- Add wifi.check_status() for check wifi status

### Optimized
- Optimize the communication process with APP (Sunfounder Controller)


## [1.3.1] - 2023-3-6

### Added
- Add command: SMD,NAME,TYPE,CHECK
- Add auto_send_check

## [1.2.0] - 2023-1-16

### Added
- Add the method of simplifying the data received by websocket and then sending it to the lower computer through the serial port

### Modified
- Separate code into multiple files for easier reading


## [1.1.0] - 2023-1-11:
- Add version print


## [1.0.3] - 2023-9-30:

### Added
- Add usage of burning esp8266 with pico


## [1.0.2] - 2022-9-9:

### Added
- Add LED to indicate connection


## [1.0.1] - 2021-12-23:

### Modified
- Tide code
- Add timeout for serial read