# Version history

[Leia em português](CHANGELOG.pt-BR.md) · [Project README](README.md)

Each entry compares the archived sketch with the preceding version. Dates are recorded in the source headers.

## v1

Archived file `HAB_OBDH_v1.ino`

- Earliest archived sketch. Reads GPS, OneWire temperatures and the barometer, appends measurements to the SD card, and sends the `DATA` structure over the E32 radio during daytime operation.
- Defines initialization, day, night and safety states. Battery and solar voltages are fields in the packet, with no acquisition routine in this snapshot. The safety state is a placeholder.
- Added the version header during archival reconstruction.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/60e64f053900ab45eb1fdfecb252611bf1f2b27b)

## v1.1

Archived file `HAB_OBDH_v1.1.ino` · 2020-11-05

- Adds battery and solar ADC acquisition on A0 and A1. The low-battery state rechecks the battery and returns to initialization when the reading rises above the threshold.
- Replaces several floating-point telemetry fields with integers, scales temperatures by 10, and stores raw barometric pressure. Adds HDOP, satellite count and ADC readings to the SD log.
- Enables serial diagnostics at 9600 baud. Startup now waits while the satellite count is below six, HDOP exceeds 10, and the battery reading exceeds 9.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/8fdac7df3829db582f90146b6ce9593aaf6231e0) · [Compare with v1](https://github.com/mariopin80/HAB_OBDH/compare/60e64f053900ab45eb1fdfecb252611bf1f2b27b...8fdac7df3829db582f90146b6ce9593aaf6231e0)

## v1.2

Archived file `HAB_OBDH_v1.2.ino` · 2020-11-11

- Adds the `CNT` acquisition counter to the radio packet and the first column of the SD log. Moves the end of the log record after the solar ADC value.
- Changes sea-level reference pressure from 1013.25 to 1016.00 hPa and the OneWire sensor indexes from 0–2 to 1–3.
- Lowers the startup satellite threshold from six to two and removes the time-of-day condition from the day loop. The day loop runs while the battery reading exceeds 9.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/790257b042e94e9e7f8840117133015b7536707f) · [Compare with v1.1](https://github.com/mariopin80/HAB_OBDH/compare/8fdac7df3829db582f90146b6ce9593aaf6231e0...790257b042e94e9e7f8840117133015b7536707f)

## v1.3

Archived file `HAB_OBDH_v1.3.ino` · 2020-11-25

- Restores the daytime window from 11:00 to 22:00 UTC, sets the battery threshold to 587 ADC counts and removes serial diagnostic prints. Serial initialization is still present.
- Changes explicit waits to about 10 seconds in daytime and 30 seconds at night and in safety mode. Restores temperature sensor indexes 0–2.
- The startup condition requires the satellite count to be both below four and above 40, so the GPS wait loop cannot run. This condition is corrected in v1.10.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/e11a1a3628f3ffc0f866635a11f152da5137cc6c) · [Compare with v1.2](https://github.com/mariopin80/HAB_OBDH/compare/790257b042e94e9e7f8840117133015b7536707f...e11a1a3628f3ffc0f866635a11f152da5137cc6c)

## v1.4

Archived file `HAB_OBDH_v1.4.ino` · 2020-12-02

- Gates daytime radio transmission on a change in the GPS minute. Sensor acquisition and SD logging continue each cycle.
- Replaces the daytime 9-second delay and 1-second GPS read window with a 2-second `smartdelay()`.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/edecc396e65573dbd55f1abdc3e9693aa88e3d84) · [Compare with v1.3](https://github.com/mariopin80/HAB_OBDH/compare/e11a1a3628f3ffc0f866635a11f152da5137cc6c...edecc396e65573dbd55f1abdc3e9693aa88e3d84)

## v1.5

Archived file `HAB_OBDH_v1.5.ino`

- Disables serial initialization, sets `min_ant` to the current GPS minute on entry to the day state, and updates it after a transmission. The next minute change triggers the first daytime packet.
- The archived filename identifies v1.5. The inherited header printed v1.4 and 2020-12-02, so the date of this revision is uncertain.
- Corrected the version label to v1.5 during archival reconstruction.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/0e5fe37a6fa64e156694b2c2d402c7199ab3ba32) · [Compare with v1.4](https://github.com/mariopin80/HAB_OBDH/compare/edecc396e65573dbd55f1abdc3e9693aa88e3d84...0e5fe37a6fa64e156694b2c2d402c7199ab3ba32)

## v1.6

Archived file `HAB_OBDH_v1.6.ino` · 2020-12-02

- Adds an E32 high-power path using `OPT_TP30`, restores `OPT_TP21` after the send, and keeps a lower-power send on the other minute changes.
- The header describes a ten-minute high-power interval. The code compares the current minute with `min_ant10` and assigns `10 * Minute` to that variable. v1.7b switches to a modulo-10 test.
- Raises the explicit daytime waits to 8 seconds plus 2 seconds of GPS processing, with additional waits around high-power transmissions.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/f82abd44189e83af0be5a049c3c92234a77a71f7) · [Compare with v1.5](https://github.com/mariopin80/HAB_OBDH/compare/0e5fe37a6fa64e156694b2c2d402c7199ab3ba32...f82abd44189e83af0be5a049c3c92234a77a71f7)

## v1.7b

Archived file `HAB_OBDH_v1.7b.ino` · 2020-12-05

- Puts the E32 in power-down between daytime packets and wakes it for transmission. Selects high power at GPS minutes divisible by ten.
- Adds `LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF)` to the low-battery state. Revises the daytime and nighttime acquisition waits.
- Substitutes a constant of 600 for the battery ADC reading, so the acquired value stays above the low-battery threshold.
- Corrected the header from v1.7 to v1.7b to match the archived filename.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/a19cf190022667bac8dae5c41e848ce7d4fab737) · [Compare with v1.6](https://github.com/mariopin80/HAB_OBDH/compare/f82abd44189e83af0be5a049c3c92234a77a71f7...a19cf190022667bac8dae5c41e848ce7d4fab737)

## v1.8

Archived file `HAB_OBDH_v1.8.ino` · 2020-12-29

- Replaces the eight-second MCU sleep in the safety state with a ten-second delay.
- Splits nighttime waiting into a nine-second delay followed by a one-second GPS read window.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/b208ff5aa7cb71266a5b0b9038a173e6a211b104) · [Compare with v1.7b](https://github.com/mariopin80/HAB_OBDH/compare/a19cf190022667bac8dae5c41e848ce7d4fab737...b208ff5aa7cb71266a5b0b9038a173e6a211b104)

## v1.9

Archived file `HAB_OBDH_v1.9.ino` · 2021-01-13

- Adds a transmission fallback when more than 90 seconds of `millis()` elapse without a daytime send. The header calls this a watchdog for transmission.
- Centralizes the battery threshold at `Vbat_min = 410`. Changes the simulated battery reading from 600 to 500 ADC counts.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/9fd80293c604f9cbd707adde69aedffa64c5ecd2) · [Compare with v1.8](https://github.com/mariopin80/HAB_OBDH/compare/b208ff5aa7cb71266a5b0b9038a173e6a211b104...9fd80293c604f9cbd707adde69aedffa64c5ecd2)

## v1.10

Archived file `HAB_OBDH_v1.10.ino` · 2021-01-14

- Flown on **SAIRUS I**. The sketch header also identifies this as the SAIRUS I flight version.
- Restores the real battery ADC reading and corrects startup to wait while the satellite count is outside 4–40 and the battery is above the minimum.
- Sends `Lo_P` and `Hi_P` configuration bytes over the radio UART and switches E32 modes directly through M0/M1. The day loop sends on a GPS minute change or the 90-second fallback, with high power at minutes divisible by ten.
- Adds a temperature sensor startup delay and samples temperature while waiting for GPS.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/7a742c6ba36d9c11965c09aaa3ca6d9d38d40898) · [Compare with v1.9](https://github.com/mariopin80/HAB_OBDH/compare/9fd80293c604f9cbd707adde69aedffa64c5ecd2...7a742c6ba36d9c11965c09aaa3ca6d9d38d40898)

## v2.0.0

Archived file `HAB_OBDH_v2.0.0.ino` · 2021-10-20

- Starts a rework of the flight software with named pin constants, renamed UARTs and states `INICIALIZACAO`, `NIVEL_1`, `NIVEL_2` and `SEGURANCA`.
- Leaves the state bodies empty. Setup initializes the peripherals and opens the log, with sensor acquisition and transmission helpers outside the main loop.
- Changes the SD write routine from `flush()` to `sync()`. This header identifies SdFat 2.1.0.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/8e7a8a79ae8d5e4d0ccdcec490f83d95966189da) · [Compare with v1.10](https://github.com/mariopin80/HAB_OBDH/compare/7a742c6ba36d9c11965c09aaa3ca6d9d38d40898...8e7a8a79ae8d5e4d0ccdcec490f83d95966189da)

## v2.0.1

Archived file `HAB_OBDH_v2.0.1.ino` · 2022-06-09

- Restores the startup, day (`OPER_1`), night (`NOTURNO`) and low-battery state logic. Startup waits for a satellite count in the 5–40 range while battery voltage is adequate.
- Restores SD logging in day and night, daytime radio sends on a minute change or the 90-second fallback, and a return to startup after battery recovery.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/7a35ba0b62557fcd61e15b8ddf7f6e61abcfe8cd) · [Compare with v2.0.0](https://github.com/mariopin80/HAB_OBDH/compare/8e7a8a79ae8d5e4d0ccdcec490f83d95966189da...7a35ba0b62557fcd61e15b8ddf7f6e61abcfe8cd)

## v2.1

Archived file `HAB_OBDH_v2.1.ino` · 2022-09-28

- Changes the version label and date in the header. The sketch body is byte-identical to v2.0.1.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/8e9e0db21aba89e8ef49968ad22aedd8fe8d4e76) · [Compare with v2.0.1](https://github.com/mariopin80/HAB_OBDH/compare/7a35ba0b62557fcd61e15b8ddf7f6e61abcfe8cd...8e9e0db21aba89e8ef49968ad22aedd8fe8d4e76)

## v2.2

Archived file `HAB_OBDH_v2.2.ino` · 2023-02-07

- Raises the startup minimum from five to six satellites, changes sea-level reference pressure to 1013.00 hPa, and uses a three-second `smartdelay()` in daytime.
- Moves E32 send sequences into `LoRa_HiPWR()` and `LoRa_LoPWR()`. The daytime branches contain local function declarations (`void LoRa_HiPWR();` and `void LoRa_LoPWR();`), so the scheduled branches send no packet in this snapshot.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/1b36b0463ce9392052423eb3288d989efed832a9) · [Compare with v2.1](https://github.com/mariopin80/HAB_OBDH/compare/8e9e0db21aba89e8ef49968ad22aedd8fe8d4e76...1b36b0463ce9392052423eb3288d989efed832a9)

## v2.3

Archived file `HAB_OBDH_v2.3_debug.ino` · 2023-02-07

- Debug sketch. Runs a single acquisition loop with a five-second GPS read window, SD logging, and low-power radio transmission on every iteration. Prints startup diagnostics and telemetry at 115200 baud.
- Selects an external ADC reference and changes `Speed` from `unsigned short` to `float`, affecting the binary telemetry layout.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/bf2ed847e023828646e47c928154ab5944d493b7) · [Compare with v2.2](https://github.com/mariopin80/HAB_OBDH/compare/1b36b0463ce9392052423eb3288d989efed832a9...bf2ed847e023828646e47c928154ab5944d493b7)

## v2.4

Archived file `HAB_OBDH_v2.4_teste.ino` · 2023-02-24

- Test sketch. Removes serial diagnostics and reduces the GPS read window to three seconds. Continues SD logging on every iteration.
- Calls the radio routines on a GPS minute change or after more than 90 seconds. High power is selected at minutes divisible by ten.
- Narrows `CNT` from `unsigned long` to `unsigned short` and increments it on sends, changing both the packet field and the meaning of the counter.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/850597096a8d3f3490ce7a7002c6911a342c54de) · [Compare with v2.3](https://github.com/mariopin80/HAB_OBDH/compare/bf2ed847e023828646e47c928154ab5944d493b7...850597096a8d3f3490ce7a7002c6911a342c54de)

## v2.5

Archived file `HAB_OBDH_v2.5_voo.ino` · 2023-02-28

- Restores startup, daytime, night and safety states with a four-satellite startup minimum. Returns `CNT` to counting acquisition cycles.
- Swaps the analog inputs to battery on A1 and solar panel on A0. Reduces `Vbat_min` from 410 to 390 ADC counts, documented in the sketch as 5.3 V for its hardware.
- Uses 90-second delays at night and in safety. The day loop ends at 21:00 UTC and the night loop starts at 22:00 UTC, leaving the 21:00 hour outside both loop conditions.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/d6807bc32e9cef05e09e653e3cceb8d355e547c7) · [Compare with v2.4](https://github.com/mariopin80/HAB_OBDH/compare/850597096a8d3f3490ce7a7002c6911a342c54de...d6807bc32e9cef05e09e653e3cceb8d355e547c7)

## v2.6

Archived file `HAB_OBDH_v2.6_voo_LP.ino` · 2023-03-07

- Adds MCU power-down through `LowPower`. Startup retries and daytime cycles request four seconds of sleep, and night and safety cycles request eight seconds. The sleep calls turn off the ADC and BOD during sleep.
- Uses a two-second `smartdelay()` after waking for startup, day and night. Documents the external 3.3 V ADC reference.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/b7c9b0057f889c49624b2e580259c1320c9e6953) · [Compare with v2.5](https://github.com/mariopin80/HAB_OBDH/compare/d6807bc32e9cef05e09e653e3cceb8d355e547c7...b7c9b0057f889c49624b2e580259c1320c9e6953)

## v2.7

Archived file `HAB_OBDH_v2.7_voo_LP.ino` · 2023-03-22

- Flown on **SAIRUS II**.
- Shortens night and low-battery sleep from eight to four seconds. Changes `smartdelay()` to check its deadline on every loop iteration and consume one GPS serial byte at a time.
- Updates the day-state comment to 21:00 UTC. The night loop still starts at 22:00 UTC, so the 21:00 hour gap from v2.5 remains in this flight sketch.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/1b00c6f6afe8e7a94091015957b8f7ea6290e0d5) · [Compare with v2.6](https://github.com/mariopin80/HAB_OBDH/compare/b7c9b0057f889c49624b2e580259c1320c9e6953...1b00c6f6afe8e7a94091015957b8f7ea6290e0d5)

## v3.0

Archived file `HAB_OBDH_v3.0.ino` · 2026-04-09

**Experimental post-flight development.**

- Adds the AVR hardware watchdog with an eight-second timeout and re-enables it after power-down. Feeds it only after the required temperature, barometer, GPS, power and SD routines have cleared their completion flags.
- Checks the watchdog during GPS acquisition retries and low-battery operation. Moves the night boundary to 21:00 UTC and returns directly to daytime operation after battery recovery.
- Changes the E32 configuration bytes in both power profiles. The header lists descent, landing, GPS startup recovery and a safety beacon as planned work.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/b89dd4d77374e5eff7de8fc7b022d6f1360bafe5) · [Compare with v2.7](https://github.com/mariopin80/HAB_OBDH/compare/1b00c6f6afe8e7a94091015957b8f7ea6290e0d5...b89dd4d77374e5eff7de8fc7b022d6f1360bafe5)

## v3.1

Archived file `HAB_OBDH_v3.1.ino` · 2026-04-15

**Experimental post-flight development.**

- Adds the state names `STARTUP`, `CLIMB`, `DESCENT`, `LAND`, `NIGHT` and `SAFE`. The descent and landing cases are placeholders in this version.
- Consolidates acquisition and watchdog flags in `ALL_Data_aquisition()`, uses fixed-width telemetry fields, and adds GPS sentinel handling, altitude range and jump filters, and peak-altitude tracking. `Speed` becomes a 16-bit value populated with `gps.speed() / 100`.
- Transmits at low power during startup while waiting for at least four satellites. Adds a separate battery recovery threshold of 401 ADC counts and a low-power beacon every 150 sleep cycles in `SAFE`.
- The header records that this version was untested on hardware.
- Corrected the header from v3.1b to v3.1 to match the archived filename.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/0cfe1428061af5b855eebd36e22013be7cafd06c) · [Compare with v3.0](https://github.com/mariopin80/HAB_OBDH/compare/b89dd4d77374e5eff7de8fc7b022d6f1360bafe5...0cfe1428061af5b855eebd36e22013be7cafd06c)

## v4.0

Archived file `HAB_OBDH_v4.0.ino` · 2026-04-15

**Experimental post-flight development.**

- Implements `CLIMB` → `DESCENT` when the recorded peak exceeds 1,000 m and the current altitude is more than 500 m below it. Descent continues SD logging and sends at high power on a minute change or after 30 seconds of `millis()`.
- Implements `DESCENT` → `LAND` after ten qualifying samples below 3,000 m and within 5 m of the reference altitude. `LAND` sends at high power every 30 acquisition cycles and requests eight seconds of sleep per cycle.
- Changes the return from `NIGHT` to `LAND` regardless of which state entered the night loop. The source marks v4.0 as untested on hardware.

[Source snapshot](https://github.com/mariopin80/HAB_OBDH/tree/fdf6d0913026c8439b11dce2a32f053d733edda6) · [Compare with v3.1](https://github.com/mariopin80/HAB_OBDH/compare/0cfe1428061af5b855eebd36e22013be7cafd06c...fdf6d0913026c8439b11dce2a32f053d733edda6)
