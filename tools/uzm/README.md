# uzm — Uzeamp's video files (.UZM)

`uzm` turns any video FFmpeg can read into a `.UZM` movie for Uzeamp (`demos/Uzeamp`), and
checks, unpacks and makes test `.UZM` files. It is one program in C, linked with FFmpeg's
libraries: no Python, no `ffmpeg` program, no script of any kind at run time.

It writes the same files as `demos/Uzeamp/tools/uzm.py`, byte for byte, when both use the same
FFmpeg version (see [Checked](#checked)).

## Use

```
uzm encode IN OUT.UZM [options]     a video file (anything FFmpeg reads)
uzm test OUT.UZM [seconds] [--rate R] [--v1|--v2]
                                    a test pattern: colour bars, ramps, a moving box, the
                                    picture's number, a 440 Hz tone (default 10 s at 60)
uzm info FILE.UZM                   the header, and a check of every sector
uzm frame FILE.UZM N OUT.png [--frame]
                                    picture N as an image, 4x (--frame: N is a console frame)
uzm sound FILE.UZM OUT.wav          the sound as the player plays it, frame by frame
uzm cadence IN [--start S] [--duration D]
                                    which rate IN's pictures really have (--ivtc or not)
uzm version                         this program's and FFmpeg's versions
```

`uzm help` lists the encoder's options: `--rate auto|60|30|25|24`, `--ivtc`, `--v1`/`--v2`,
`--title`, `--artist`, `--start`, `--duration`, `--dither ours|bayerN|fs|sierra2|sierra2_4a|
sierra3|burkes|atkinson|none`, `--temporal`, `--black N`, `--cut N`, `--pre F`, `--post F`,
`--scaler S`, `--letterbox`, `--crop W:H:X:Y`, `--loudness N|off`, `--compress`, `--audio N`.
They mean what they mean in `uzm.py` (its notes on each are worth reading). `-v` before the
command shows FFmpeg's own messages.

Examples:

```
uzm encode intro.mkv INTRO.UZM --title "Intro" --letterbox
uzm encode cartoon.mp4 TOON.UZM --ivtc                 # 24 fps animation carried at 60
uzm encode game.mp4 GAME.UZM --pre scale=384:224:flags=area --post unsharp=3:3:0.6
uzm info INTRO.UZM
```

## Build

You need a C compiler, `make`, `pkg-config` and FFmpeg's development libraries (libavformat,
libavcodec, libavfilter, libavutil), version 6.0 or newer.

**Windows** — in an [MSYS2](https://www.msys2.org) UCRT64 shell:

```
pacman -S make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-ffmpeg
make
```

The Makefile also finds FFmpeg when only MSYS's own `pkg-config` is installed.

**One standalone program** (any system): `make dist` downloads FFmpeg's source (8.0, from
GitHub), builds it trimmed to what reading video needs (FFmpeg's own demuxers, decoders,
parsers and filters, dav1d for AV1, zlib; no encoder, muxer, device, network or other library)
and links it into `dist/uzm` (`dist/uzm.exe`: one 29 MB file that needs only Windows' own DLLs;
FFmpeg's build takes about 10 minutes). It needs `nasm` and dav1d too
(MSYS2: `mingw-w64-ucrt-x86_64-nasm mingw-w64-ucrt-x86_64-dav1d`; Debian: `nasm
libdav1d-dev`; Homebrew: `nasm dav1d`). `make distclean` removes FFmpeg's build.

**Linux** (Debian, Ubuntu 24.04 or newer):

```
sudo apt install build-essential pkg-config libavformat-dev libavcodec-dev libavfilter-dev libavutil-dev
make && sudo make install
```

**macOS** (Homebrew):

```
xcode-select --install
brew install pkg-config ffmpeg
make && sudo make install
```

`make check` writes the test pattern at every rate and checks every sector.

## How it matches uzm.py

`uzm.py` ran the `ffmpeg` program; `uzm` calls the same libraries directly, so it has to do what
the program does around them. Where that changes a byte, `src/media.c` follows FFmpeg 8.0's
`fftools`:

- **the streams:** the video with the most pixels, the audio with the most channels;
- **`--start`/`--duration`:** the seek made with every stream present (the format seeks by its
  video), timestamps moved back by the start, a `trim` filter in front of the graph;
- **the graph:** built in the program's order (the chain, the source, the output's format filter,
  the sink): the order decides where FFmpeg puts its conversions, and so the sound's last bit;
- **frame timestamps:** video by the decoder's best effort, audio through `av_rescale_delta`;
- **the pictures out:** made constant-rate as for the `rawvideo` muxer (a picture repeated or
  dropped by its time), and the rotation in a phone video's display matrix applied first.

The dither (`src/dither.c`) is `uzmdither.c`'s, with the same doubles. It is built with
`-ffp-contract=off`, so that compilers that fuse multiply-adds (clang on Apple silicon) round
as everyone else does.

Differences from `uzm.py`:

- a video with no sound gets silence (uzm.py stopped there);
- `--gpu` decodes on the CPU (uzm.py's notes: it was no faster);
- the test pattern's artist is "uzm";
- `frame` writes its PNG itself.

## Checked

Against `uzm.py` with FFmpeg 8.0 on Windows (MSYS2 UCRT64), whole files compared byte for byte,
**all identical**:

- clips made by FFmpeg at 60, 30 (29.97), 25 and 24 fps, H.264/AAC in MP4 and VP9/Opus in MKV,
  mono and stereo: default; `--v1`; `--rate 30`/`24` from 60; `--letterbox`; `--start` and
  `--duration` (six combinations); `--temporal` at 24 and 30; `--dither sierra2_4a`, `bayer2`,
  `fs`; `--black` with `--cut`; `--loudness off`; `--loudness -20 --compress`; `--crop`, `--pre`,
  `--post`, `--scaler`; UTF-8 title and artist;
- film carried at 60 and at 29.97 with `--ivtc`, and with `--ivtc --temporal`;
- videos rotated by their display matrix (90°, 180° flipped);
- real videos: a 5.4 minute music video (151 MB out), an 88 s laserdisc game capture, an AVI, a
  game intro with `--rate 30 --cut 24`, an HD intro with `--letterbox --start --duration
  --compress`, a clip with `--temporal --pre --post`;
- `test` at every rate and `--v1` (the header's artist aside); `cadence`'s report, word for word.

The Linux and macOS builds were not tried on those systems: the sources compile as strict C11
with POSIX, and nothing in them is Windows-only outside `#ifdef _WIN32` (UTF-8 file names and
arguments). Byte-identical output across systems also needs the same FFmpeg version, since its
scalers and resamplers change between versions.
