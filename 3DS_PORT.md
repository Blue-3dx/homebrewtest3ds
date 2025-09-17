
# 3DS Port notes (citro2d / libctru)

This section explains how to port the SDL2 prototype to Nintendo 3DS homebrew.

## High level
- Use `libctru` for system init and input (hidKeysDown/hidKeysHeld).
- Use `citro2d` (C2D) for 2D textured sprites and simple transformations (rotation).
- Keep the same physics:
  - `vy += holding ? -THRUST : GRAVITY`
  - clamp vy to +/- MAX_SPEED
  - `py += vy * dt`
  - collision with floor/ceiling: set vy = 0 and py = limit; when touching, set rotation = 0.
- Trail: keep fixed-length circular buffer of previous positions and angles; draw them with decreasing alpha.

## Example pseudocode (C with citro2d, NOT full compile-ready):
```c
// inside game loop:
float dt = get_delta_time();
if (hidKeysHeld() & KEY_A) holding = true; else holding = false;
if (started && !paused) {
    float a = holding ? -THRUST : GRAVITY;
    vy += a * dt;
    if (vy > MAX_SPEED) vy = MAX_SPEED;
    if (vy < -MAX_SPEED) vy = -MAX_SPEED;
    py += vy * dt;
    // clamp with floor/ceiling
    if (py > floor_y) { py = floor_y; vy = 0; touching = true; } else if (py < ceil_y) { py = ceil_y; vy = 0; touching = true; } else touching = false;
    float angle = touching ? 0.0f : (vy / MAX_SPEED) * 30.0f;
    // push into trail buffer
    push_trail(px, py, angle);
}
// Drawing with citro2d:
C2D_SceneBegin(top);
for (i = trail_len-1; i >= 0; --i) {
    float a = (float)i / (trail_len-1);
    C2D_DrawSprite(&sprite, C2D_WithColor(alpha= (1.0f-a)*0.6f ... ), position, rotation=trail[i].angle);
}
C2D_DrawSprite(&playerSprite, ..., position, rotation=current_angle);
C3D_FrameEnd(0);
```

## Makefile
Use the standard devkitPro Makefile include. Example:
```
TARGET := GDWave
BUILD := build
SOURCES := source/main.c
INCLUDES :=
LIBS :=
include $(DEVKITPRO)/3ds/samples/Makefile
```
Adjust paths to match your repo structure and toolchain.

