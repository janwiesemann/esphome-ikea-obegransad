# ESPHome Component for IKEA OBEGRÄNSAD

This project adds a display component for the IKEA OBEGRÄNSAD to ESPHome.

This reimplementation aims to provide a simpler and faster alternative to <https://github.com/sascha-hemi/esphome-ikea-led-matrix> without any external dependencies. It's output mechanism is based on the [ikea-led-obegraensad Project by ph1p](https://github.com/ph1p/ikea-led-obegraensad), which utilizes [SPI](https://github.com/ph1p/ikea-led-obegraensad/blob/2ea603c390050e167fd90387da6b4bd4afbe7687/src/screen.cpp#L252) over a bit banging approach.

## Wiring

For a more detailed guide check out [ph1p's project page](https://github.com/ph1p/ikea-led-obegraensad/tree/main#hardware-setup).

> [!Important]
> If you're not using any of the mentioned boards, ensure, that you're using Pins, which support hardware based SPI and ideally a Hardware-PWM-Pin for the Brightness-Pin.

| OBEGRÄNSAD Pin | Description | WeMos D1 mini Pin      |
| -------------- | ----------- | ---------------------- |
| `CLA`          | Latch-Pin   | `D2` (`GPIO4`)         |
| `CLK`          | Clock-Pin   | `D5` (`GPIO14`/`SCLK`) |
| `IN`           | Data-Pin    | `D7` (`GPIO13`/`MOSI`) |
| `EN`           | Enabled-Pin | `D6` (`GPIO12`/`MOSI`) |

## Example configuration

> [!Note]
> The example is based on the WeMos D1 mini.

```yaml
substitutions:
  PIN_PANEL_BRIGHTNESS: D6 # GPIO12/MISO/SoftPWM
  PIN_PANEL_DATA:       D7 # GPIO13/MOSI
  PIN_PANEL_CLOCK:      D5 # GPIO14/SCLK
  PIN_PANEL_LATCH:      D2 # GPIO4

esp8266:
  board: d1_mini

esphome:
  name: ikea-obegransad
  friendly_name: IKEA OBEGRÄNSAD

external_components:
  - source: github://janwiesemann/esphome-ikea-obegransad@master
    components: [ ikea_obegraensad ]

light:
  - platform: monochromatic
    name: "Brightness"
    output: matrix_brightness
    restore_mode: RESTORE_DEFAULT_ON
    initial_state:
      state: ON
      brightness: 50%

output:
  - platform: esp8266_pwm
    id: matrix_brightness
    pin:
      number: $PIN_PANEL_BRIGHTNESS
      inverted: True

font:
  - file: "04B03.ttf"
    id: b03
    size: 8

time:
  - platform: sntp
    id: ntp_time
    timezone: "Europe/Berlin"

spi:
  clk_pin: $PIN_PANEL_CLOCK
  mosi_pin: $PIN_PANEL_DATA

display:
  - platform: ikea_obegraensad
    latch_pin: $PIN_PANEL_LATCH
    lambda: |-
      it.strftime(4, 0, id(b03), "%H", id(ntp_time).now());
      it.strftime(4, 8, id(b03), "%M", id(ntp_time).now());
```
