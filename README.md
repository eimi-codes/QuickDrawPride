# QuickDrawPride

A MacOS9 PPC application for showing Pride Flags. Ported from [@foone](https://github.com/foone)'s [VGAPride](https://github.com/foone/VGAPride) DOS program.

---

![WiP Badge](https://img.shields.io/badge/Status-Work_in_Progress-orange?style=for-the-badge)
[![Made by Human](https://madebyhuman.iamjarl.com/badges/made-black.svg)](https://madebyhuman.iamjarl.com)

---

VGAPride is an MS-DOS program by @foone that draws LGBTQ+ pride flags in VGA graphics. This project ports it to run natively on classic Mac OS (PowerPC), built with the **Retro68** GCC cross-toolchain, with **Metrowerks CodeWarrior** kept in reserve as a period-authentic fallback. The one significant change to the original's behaviour: instead of taking a flag name on the command line (or booting into a linear slideshow), the Mac version presents a **scrolling selector** so the user can jump straight to any flag out of ~60.

---

## Requirements

**To run:**

- A PowerPC Mac running Mac OS 8.6 – 9.2.2 (tested on G3/G4 hardware), **or**
- [SheepShaver](https://sheepshaver.cebix.info/) with a Mac OS 9 install for testing.
- A display set to Thousands or Millions of colours is recommended (256 colours works, but
  flat fills may dither).

**To build:**

- [Retro68](https://github.com/autc04/Retro68) (GCC cross-toolchain with the multiversal interfaces + CMake integration). *Primary toolchain.*
- Optionally, **Metrowerks CodeWarrior Pro** (PowerPC target) if you'd rather build on the Mac itself — kept as a reserve path.

---

## Usage

1. Launch the app.
2. Pick a flag from the list — click it, or use the **up/down arrows**, or start typing a
   flag's name to jump to it.
3. The flag draws in the preview pane, with its designer credit shown beneath.
4. **⌘Q** or Escape to quit.

Full credits for every included flag are shown in the app (per-flag, and collected in the
**About** box under the Apple menu).

---

## How this differs from upstream VGAPride

This is a source-level native port, not an emulator wrapper. The changes:

- **QuickDraw instead of BGI/VGA.** The ~12 BGI drawing calls are reimplemented on top of QuickDraw. Flag geometry is unchanged — it maps 1:1 onto QuickDraw's coordinate system.
- **A selector instead of a command line.** Classic Mac OS has no shell to pass a flag name, so the picker (with live preview) replaces both the `VGAPRIDE <FLAGNAME>` interface and the DOS slideshow.
- **True-colour rendering.** The original's 16-colour VGA palette-mapping layer is gone; QuickDraw draws in RGB directly.
- **Bitmap flags regenerated from source.** The Cool Crab, Trans Cool Crab, and Autistic Pride flags were stored as LZ4-compressed planar VGA data unpacked by x86 assembly. This port regenerates them from the original PNGs (present upstream in `crabs/`) and draws them with `CopyBits`. The LZ4 assembly, bitplane manipulation, and direct-framebuffer code are removed.

---

## License

- The code is licensed under the terms of the GPL, version 3.
- A copy of the license can be found in the LICENSE file.
- It is a derivative work of VGAPRIDE, which is licensed under the terms of the GPL, version 3.
- The Autistic Pride Flag was created by Autistic Empire, and is used under a Creative Commons Attribution-ShareAlike 4.0 International License: https://www.autisticempire.com/autistic-pride/
- **Original DOS build also used**
- **lz4_8088** by **Jim Leonard** (Trixter/Hornet), under the Demoscene License — <http://www.oldskool.org/pc/lz4_8088>. *Not used in this port* (the bitmap flags are regenerated from source images), credited here in acknowledgement.

---

## Acknowledgements

Thanks to @foone for VGAPride and for shipping the flag data in a form that made this port a joy rather than a slog, and to the Retro68 project ([autc04](https://github.com/autc04/Retro68)) for making modern classic-Mac cross-development possible.
