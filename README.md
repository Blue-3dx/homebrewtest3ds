
# GDWave

This repository contains a **desktop prototype** of *GDWave* (a Geometry Dash-like wave) implemented with SDL2.  
It is provided as a working prototype so you can test the gameplay mechanics, visuals (rotation, trail), and tweak parameters easily on your PC. I also included porting notes and a template GitHub Actions workflow for building on 3DS (homebrew) using devkitPro — that workflow is a template and may need tweaks depending on your CI tooling.

## Files
- `src/main.c` — SDL2 prototype (controls below).
- `assets/wave.png` — texture (the PNG you provided).
- `assets/wave.t3x` — your T3x file (kept for reference).
- `Makefile` — builds the desktop prototype on Linux.
- `.github/workflows/3ds-build.yml` — **template** GitHub Actions workflow for 3DS (see notes).
- `3DS_PORT.md` — notes and code snippets for porting to 3DS (citro2d / libctru).

## Controls (desktop prototype)
- Hold `Z` or `A` key to simulate the 3DS A button (wave goes up).
- Release to go down.
- Press `Z` / `A` on the title screen to begin.

## Build (desktop Linux)
Install dependencies:
```bash
# Debian/Ubuntu
sudo apt-get install build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```
Then:
```bash
make
./GDWave
```

## Porting to 3DS (notes)
I included a `3DS_PORT.md` with guidance and a short code sketch showing the same physics logic (vertical acceleration, rotation mapping, trail). You will want to use `citro2d` (or `C3D` + texture loading) and `libctru` for input. Keep the game loop, physics, rotation and trail logic identical; replace SDL drawing calls with appropriate GPU sprite draws on the 3DS.

## GitHub Actions / 3DS CI
I added a **template** workflow at `.github/workflows/3ds-build.yml` that demonstrates how to:
- install devkitPro (you may need to adjust the exact installer and package list)
- build a 3dsx using the standard devkitPro Makefile
This template is meant as a starting point and might need small edits to match current devkitPro installer behavior or package names.

---

If you want, I can:
- Produce a direct 3DS port (C code using libctru + citro2d) and attempt to make a build workflow that uses a known devkitPro docker image — I can provide that next.
- Or I can convert your T3x into a 3DS-compatible texture format (requires details) and plug it into the 3DS engine code.

Tell me which of the above you'd like next and I will continue (I already included the prototype so you can try game feel quickly).


## 3DS build
A `3ds/` folder was added with a citro2d-based 3DS port source and Makefile. Place the `romfs/gfx/wave.t3x` file (already included) and build with devkitPro. The Makefile requires the DEVKITPRO environment variable and citro2d installed.
