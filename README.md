
![flipflop](docs/logo-top-grey.png)

Flipflop aims to be a lightweight serial bootloader for PIC(L)F1919x devices.
Basic functionality includes reading, writing, and erasing. UART communications
is stable at baud rates ranging from 300 to 1M over both 2-wire and 1-wire UART.

Flipflop resides in a section of memory at the beginning of Program Flash
Memory (PFM). This means the Reset (0x0) and Interrupt (0x4) vectors normally
used by an application are taken up by the bootloader.
The bootloader does not use interrupts so the hardware interrupt vector is
mapped to simply jump to the application's interrupt vector.
The Reset vector is used for all bootloader functionality. This means a
Reset is needed to enter the bootloader. On startup the bootloader decides
whether to start the bootloader or jump to the application, based on the source
of the reset.

## Usage

The flipflop bootloader must be flashed to a device using an ICSP programmer.
Afterwards, flipflop can be used to upload user applications to the device.

### Entering the bootloader
Upon a reset event, flipflop determines if the watchdog timer caused the reset.
If it was not, flipflop starts listening for the sync character ('U') from the
UART. If the sync character is not received before the watchdog timeout, a
reset is triggered. Flipflop then detects a watchdog reset, which causes a jump
to the user's application reset vector.

### Uploading Applications
Applications can be uploaded with [picchick](https://github.com/Rex--/picchick)
or your own software. A python script is provided with examples to communicate
with flipflop using pyserial.

For simple sessions you can use a serial monitor program such as `screen` to
send and receive commands. Note that the arguments to the read/write/erase
commands take a byte value, so really the only usable commands are Reset ('U'),
Start ('X'), and Version ('V').

### Updating the bootloader
To update the bootloader, `flipflop-upload` can be uploaded as an user
application. This application can then be used to upload a new bootloader.

### Bootloader commands
| Command | Arguments (len bytes) | Data (len bytes) | Description |
|:-------:|-----------------------|------------------|-------------|
| 0x55 'U'||| Sync/Reset |
| 0x58 'X'||| Start user application |
| 0x52 'R'| Address (2) Length (2) || Read Length bytes at Address |
| 0x57 'W'| Address (2) Length (2) | Data (Length) CRC (2)| Write Data to Address |
| 0x44 'D'| Address (2) || Erase row |
| 0xXX 'V'||| Return version in base-36 |

Some commands take a few of the same argument. Note that all address and word
data are formatted as 2-bytes in big endian (MSB sent first). 

## Building
Microchip's
[xc8](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc8)
compiler should be used when building both flipflip and user
applications. Other compilers are untested, but may work.

### Building flipflop
To build flipflop, `xc8` and `make` must be installed, along with some common
*nix utils such as `mkdir` and `rm`. After installing the dependencies, running
`make flipflop` in the project directory will generate `build/flipflop.hex`.
This hexfile should be flashed onto the device with your favorite ICSP
programmer, [picchick](https://github.com/Rex--/picchick) supports this
protocol with an external programmer.

An updater application can be compiled using `make flipflop-update`. Thiswill
generate `build/flipflop-update.hex` which can be uploaded using flipflop.

Both binaries can be compiled with a `make all`.

### Building user applications
User applications built with `xc8` require minimal configuration to be used
with flipflop. \
When compiling/linking your project, you need to tell the linker to offset the
starting address with `-mcodeoffset` by the bootloader offset `0x400`:

    xc8 -mcodeoffset=0x400 <sources.c> -o <output.hex>

This will produce a hex file that can be directly uploaded with flipflop.

#### Configuration Words
Configuration words 3-5 are unavailable for use by the user application. This
includes the watchdog (CONFIG3), boot block (CONFIG4), and flash protection
(CONFIG5).
- The watchdog is configured to enable software control of the watchdog timer.
    It is disabled before starting a user application.
- Boot block is disabled.
- UserNVM code protection is disabled.

## Copying

Copyright (c) 2022-2024 Rex McKinnon \
This software is released under the permissive University of Illinios/NCSA Open
Source License. Check the LICENSE file for more details.
