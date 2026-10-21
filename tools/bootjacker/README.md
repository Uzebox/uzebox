# Uzebox Bootjacker

Bootjacker upgrades supported pre-V5 Uzebox bootloaders from an SD card, using
the image built by `demos/Bootloader5`. It uses a timer interrupt to return
from the installed bootloader's flash-writing instruction.

## Install

1. Use an SD card readable by the installed loader. GameLoader 0.4.6 requires
   an SDSC card with an MBR, a FAT16 first partition, and contiguous game files.
   Copy `default/BJTOV5.UZE` to its root directory.
2. Run **BJTOV5.UZE**. Leave **BUILT-IN (DEFAULT)** selected and press A, or
   select **SD: BOOTLOADER.HEX** with Up/Down. The updater validates the source,
   displays its version, and checks hardware and interrupt timing without
   writing flash. A failed check prevents the upgrade.
3. After **CHECKS PASSED**, release all controller buttons, then hold
   **START+A for three seconds** to begin. Checks run again before writing.
4. Keep power on while writing. Video pauses during the update.
5. After **ALL 4096 BYTES VERIFIED**, switch power off. Hold **A** while
   switching power back on to enter the V5 menu.

Power loss or a write failure can leave an incomplete bootloader requiring
an ISP programmer. If **UPDATE INCOMPLETE** appears, keep power on and record
its error and page address. Only the full verification message indicates success.
There is no power-loss rollback.

The application contains the default bootloader payload. Neither source requires
SPI RAM, SD writes, or a network connection. Loading the application
replaces the installed game in the usual way. Bootjacker does not write EEPROM,
fuses, or lock bits.

## Optional SD payload

Place `BOOTLOADER.HEX` in the card's root directory and select it explicitly.
The built-in image remains the default and does not read the card. The SD
option supports FAT16/FAT32, the file's long name, and fragmented files. A missing
or invalid file displays an error and returns to source selection with B; it
never silently switches to the built-in image.

The HEX file must be at most 64 KiB, contain checksummed Intel HEX records in
ascending address order, and end with an EOF record. Data must begin at
`0xF000` and remain below `0x10000`, without overlaps. Extended segment/linear
address records and start-address records are accepted. CRLF and LF line endings
are supported. Unspecified flash bytes are filled with `0xFF`. The same API,
version, and flash-helper checks apply as for the built-in payload.

Validation uses a centered three-row display with a page counter, then restores
the full screen for results and confirmation. Video Mode 5 provides 40-column text
and adjustable rendering height without kernel modifications.

The updater validates the entire file before enabling confirmation and again
before installation. It keeps one 256-byte page in SRAM, plus a small input
buffer and checksums. Each page is reread and checked against the original file
and page CRC32 before erasing, then verified against flash. The reset-vector
page is written last; final verification compares every page with the source.
The selected payload's own helper address is used after its last page is verified.

Keep the card inserted throughout the SD update. A read failure or changed file
stops further writes. If writing has begun, this can leave an incomplete
bootloader requiring ISP recovery. CRC checks detect corruption and changes;
they do not authenticate the file. Use a trusted bootloader image.

## Compatibility

- ATmega644 or ATmega644A, device signature `1E 96 09`, at the standard Uzebox clock.
- A 4 KiB boot section at `0xF000` with BOOTRST programmed:
  `(high_fuse & 0x07) == 0x02`.
- WDTON unprogrammed, allowing the watchdog to be disabled.
- Boot read/write lock bits unprogrammed: `(lock_byte & 0x30) == 0x30`.
- A supported `OUT/STS SPMCSR,rN; SPM; SEI/NOP` sequence within one page below
  `0xFF00`, using a register compatible with the dispatch routine.

Bootjacker detects instruction sequences rather than assuming a version or
fixed flash address. Not every pre-V5 loader is compatible. ATmega644P/PA and
ATmega1284 variants are unsupported. Boot lock protection cannot be bypassed;
incompatible fuses or locks require ISP access.

**TARGET V5 ALREADY INSTALLED** means the boot region matches the target image.
**V5 FOUND: LEGACY SITE REQUIRED** means a V5 signature is present but there is
no supported instruction site outside the last page. That configuration requires
ISP to replace the bootloader. Both cases stop without writing.

## Build

From `tools/bootjacker` or its `default/` directory, run:

```sh
make
```

The build uses `demos/Bootloader5/default/Bootloader5.hex`. If that file is
missing, make builds it in the Bootloader5 directory first. A failed bootloader
build stops the updater build. An existing HEX is used as-is; rebuild Bootloader5
first when you want to incorporate changes to its source.

GNU make, a host C++11 compiler, and an AVR GCC toolchain are required.
On Windows, use a MinGW/MSYS shell or WSL with those tools on PATH.
To select another payload:

```sh
make PAYLOAD=/path/to/Bootloader5.hex
```

Set `HOSTCXX`, `AVR_PREFIX`, or `KERNEL_DIR` to override the host compiler,
AVR tool prefix, or Uzebox kernel path. The Makefile builds `pack.cpp` locally
to validate the payload and package the ROMs; no scripting runtime is required.
Relative override paths are resolved from `default/`.

All generated files are in `default/`:

| File | Purpose |
| --- | --- |
| `BJTOV5.UZE` | SD bootloader updater. |
| `payload_meta.h` | Payload version, CRC32, and flash-helper address. |

`make clean` removes generated files without changing the bootloader source or
its HEX. The source tree contains only lowercase source files and `default/`.

## Payload compatibility

The build accepts compatible payloads without pinning a release number. It reads
the version from the HEX and displays it in the updater. Images must start at
`0xF000`, stay within `0xFFFF`, contain the bootloader API signature, and provide
one recognized flash helper entirely within the last page. Unspecified bytes
are filled with `0xFF`.

Bootjacker calls that helper to finish the upgrade. Accepted helpers take the
SPM command in r24, the byte address in Z, and the data in r1:r0, then wait for
SPM completion and return. They either enable interrupts or restore SREG using
a caller-saved register. A release that changes this interface requires updating
the helper check; an unknown sequence is rejected before ROM generation.

## Flash sequence

1. Save peripheral state and mask interrupt sources; use Timer0 COMPA for dispatch.
2. Calibrate against application-space instruction replicas with SPMEN clear,
   then require 32 successful no-write returns from the installed instruction site.
3. Erase, fill, and write `0xFF00`; verify the entire page before using its V5 helper.
4. Disable Timer0 interrupts and use that helper to write `0xF100..0xFEFF`.
5. Write the reset-vector page `0xF000..0xF0FF` last.
6. Compare all 4096 bytes with the payload and restore peripheral state and video.

Writes are restricted to the NRWW boot area. Matching pages are skipped; each
other page has at most three write attempts. All peripheral interrupt sources
remain masked while the payload helper runs.

## Credits and license

GPL-3.0-or-later; see `LICENSE`. Based on Julian Skidmore's BootJacker technique.
Uzebox kernel and GameLoader: Alec Bourque and contributors. Bootloader5:
Sandor Zsuga (Jubatian) and contributors.
The 6x8 font comes from `demos/GameOfLife/data/font-6x8-full.inc`.
