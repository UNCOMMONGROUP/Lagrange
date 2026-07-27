# Lagrange USB-UART Bridge

An open-source USB CDC-ACM to UART bridge built around the Renesas RA2L2 MCU.

## Why another USB-UART adapter?

Because the cheap ones keep letting us down.

If you have spent any time bringing up embedded hardware, you know the failure
modes: the adapter enumerates fine for an hour and then silently stops passing
data, the COM port vanishes mid-flash and takes the bootloader session with it,
throughput collapses under sustained traffic, or the driver decides today is the
day it does not want to install. Mass-market clone adapters — the CH340-class
parts you find on every marketplace — are cheap for a reason, and when a debug
session depends on the serial link staying up, that cost shows up somewhere
else. We got tired of blaming our own firmware for problems that turned out to
be the adapter, so we built one we could actually trust.

This design is meant to be boring in the best way: stable enumeration, reliable
sustained throughput up to 1.5 Mbps, and behaviour you can reason about because
the firmware is right here in this repository.

## Why an MCU instead of a dedicated bridge chip?

We deliberately did **not** use a fixed-function bridge IC such as the FT232.

A dedicated bridge chip does exactly one thing, and if you need it to do
anything else, you buy a different chip and redesign the board. By building on
the RA2L2 — a general-purpose Arm Cortex-M23 MCU with native USB — the
conversion logic lives in firmware instead of in silicon. The same hardware can
be retargeted to other interfaces: I2C, SPI, CAN, RS-485, or a protocol-aware
debug tool that does more than shuffle bytes. Adding an interface becomes a
firmware release rather than a new product.

It also means the behaviour is inspectable and fixable. If something needs to
change — buffering strategy, flow control, line-coding handling — the source is
in this repository and you can change it. That is not an option with a
closed-function bridge chip.

## Features

- USB 2.0 Full-Speed CDC-ACM (virtual COM port, no vendor driver required)
- Baud rates up to 1.5 Mbps with 0% error at the target clock
- DTC-driven UART path with a 1 ms idle flush, so sustained traffic does not
  drown the CPU in per-byte interrupts
- Live reconfiguration of baud rate, parity, stop bits and data bits via CDC
  `SET_LINE_CODING`
- TX and RX activity LEDs
- USB-C connector with proper CC termination
- ESD protection on both the USB and UART sides
- Molex Pico-Lock connector on the UART side
- 3D-printed (SLS) enclosure; design files included

## Hardware

| Item | Detail |
| --- | --- |
| MCU | Renesas RA2L2 (R7FA2L2074CNH), Arm Cortex-M23 |
| System clock | 48 MHz HOCO — no external crystal required |
| UART | SCI1 |
| USB | Full-Speed device, USB-C receptacle |
| UART connector | Molex Pico-Lock 205338-0004 |
| PCB | 40 × 19 mm, single-sided assembly |

## Repository layout

```
firmware/    e2 studio project (Renesas FSP)
hardware/    schematic, PCB design files, enclosure STEP
```

## Building the firmware

The firmware is an e2 studio project based on the Renesas Flexible Software
Package (FSP). Open the project in e2 studio, run **Generate Project Content**
from the FSP Configurator, and build.

## License

- **Firmware:** MIT — see [LICENSE](LICENSE)
- **Hardware:** CERN-OHL-S v2 — see [LICENSE-hardware](LICENSE-hardware)

Portions of the firmware are provided by the Renesas Flexible Software Package
and are licensed under BSD-3-Clause by Renesas Electronics Corporation; the
original copyright notices are retained in those files.
