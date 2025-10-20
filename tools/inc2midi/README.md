
# inc2midi

Convert Uzebox `.inc` arrays produced by **midiconv** (or **mconvert**’s compressed stream) back to a standard MIDI file.

- **Default timing is exact to the original 60 Hz frames.**
- **By default, all events align cleanly to 1/16 notes**: we use a time‑preserving grid with `--quant 60`, `--bpm 225.0`, and derive a PPQ so that **4 frames = 1/16 note** (PPQ ends up 960).  
- You can output **MIDI Type 0** (single track) or **MIDI Type 1** (per-channel tracks; track 0 is the conductor/tempo/markers).

## Build

```bash
cc -O2 -o inc2midi inc2midi.c
```

## Usage

```text
inc2midi input.inc output.mid [options]

  -i N, --index N        Select N-th array initializer (default 0)

Timing (defaults map 60 Hz frames to musical grid):
  --quant Q              Ticks per 60 Hz frame (time-preserving). Default 60
  --bpm B                BPM for grid math (fractional ok). Default 225.0
  --ppq P                Explicit PPQ (only if --quant not provided)
  --grid N               Make PPQ a multiple of N (no timing change)
  --snap N               Snap events to nearest 1/N note (changes timing)

MIDI file:
  --type 0|1             Output MIDI Type 0 (default) or Type 1 (per-channel tracks)

Input format:
  --compressed           Treat bytes as mconvert compressed stream (default is raw midiconv)

Behavior:
  --no-mono              Disable per-channel monophonic cleanup (default: enabled)
  --summary              Print per-channel note counts
  --verbose              Print mapping and diagnostic info
```

### Typical commands

- **Raw midiconv array → MIDI Type 0** (defaults; 60 Hz exact; aligns to 1/16 notes)
  ```bash
  ./inc2midi song.inc song.mid
  ```

- **Compressed (mconvert) array → MIDI Type 1, snapped to 1/16 for editing**
  ```bash
  ./inc2midi compressed.inc song.mid --compressed --type 1 --snap 16
  ```

- **Force PPQ to be multiple of 16 (no timing change)**
  ```bash
  ./inc2midi song.inc song.mid --grid 16
  ```

## How timing works (exact by default)

- Original data are spaced by **60 Hz frames**. With `--quant 60` we assign **60 ticks per frame**.
- We then choose **PPQ** to make frames land on exact musical positions for the chosen **BPM**.
- Defaults: `--bpm 225.0` → one quarter note is 0.266…s → **16 frames per quarter**.  
  Therefore **1/16 note = 4 frames**. This aligns frame times to 1/16 grid lines exactly (and finer: a frame is 1/64 note).
- The tool **derives PPQ automatically** from `--quant` and `--bpm` so everything is exact. With the defaults PPQ = **960**.

### `--grid N` (no timing change)

Sometimes DAWs prefer PPQ divisible by certain numbers (e.g., 16 or 48). `--grid N` scales **both** PPQ and the internal ticks-per-frame so that:
- Event times in **seconds** and **musical positions** do **not** change.
- Resulting PPQ becomes a multiple of `N`.

### `--snap N` (changes timing)

Snaps every event to the nearest 1/`N` note by adjusting its absolute time. This **will** slightly alter timing. Use for easier editing in a DAW if desired. Omit it for bit‑exact timing to the source.

## Formats supported

- **Raw midiconv** stream (default): VLQ delta times plus MIDI channel events and meta:
  - Note On (with velocity); Note Off as Note On with velocity 0.
  - Program Change, basic controllers (volume/expression), and meta 0x06 markers (typically `'S'`/`'E'` loop hints) are passed through.

- **mconvert compressed** (`--compressed`): the packed stream used by Uzebox’s streaming player. The decoder reconstructs the same raw event stream (including `'S'`/`'E'` markers and End‑of‑Track).

## Monophonic cleanup

Uzebox channels are effectively monophonic. By default, when a new note starts on a channel and a previous note is still “on”, the tool inserts a **Note Off** (Note On with velocity 0) for the previous one on **that channel only**. Disable with `--no-mono`.

## Type 0 vs Type 1

- **Type 0 (default):** single track; includes tempo and markers.
- **Type 1:** track 0 is the **conductor** (tempo + all meta markers). Each used MIDI channel gets its own track with its events.

## Known limitations

- The tool focuses on the subset of MIDI that midiconv/mconvert produce (channel events, program change, basic controllers, and the 0x06 marker). Other meta or system messages are ignored.
- If your compressed source has no `'S'`/`'E'` markers, none will be emitted.
- `--snap` can cause overlaps or very short notes on dense passages; use judiciously.

## Defaults recap

- **Exact 60 Hz timing:** `--quant 60`
- **Musical alignment:** `--bpm 225.0` → frames align to 1/16 grid (4 frames per 1/16)
- **Derived PPQ:** 960 (or scaled by `--grid` if requested)
- **File type:** Type 0 unless `--type 1` is specified
- **Input:** raw midiconv unless `--compressed` is specified
