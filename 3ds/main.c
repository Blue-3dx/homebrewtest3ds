// 3ds/main.c
// GDWave 3DS port (citro2d) - loads wave.t3x from romfs:/gfx/wave.t3x
// NOTE: include <3ds.h> BEFORE <citro2d.h> so system types (u16, GPU_*, gfxScreen_t, etc.) are defined.

#include <3ds.h>       
#include <citro2d.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define SCREEN_W 400.0f
#define SCREEN_H 240.0f

const float GRAVITY = 1200.0f;
const float THRUST = 1600.0f;
const float MAX_SPEED = 900.0f;
#define TRAIL_LEN 16

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    C2D_SpriteSheet spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/wave.t3x");
    if (!spriteSheet) {
        consoleInit(GFX_BOTTOM, NULL);
        printf("Failed to load romfs:/gfx/wave.t3x\n");
        printf("Make sure the file exists in romfs/gfx and is a valid .t3x produced for citro2d.\n");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
            svcSleepThread(100000000ULL);
        }
        C2D_SpriteSheetFree(spriteSheet);
        C2D_Fini();
        C3D_Fini();
        gfxExit();
        romfsExit();
        return 0;
    }

    C2D_Sprite player;
    C2D_SpriteFromSheet(&player, spriteSheet, 0);
    C2D_SpriteSetCenter(&player, 0.5f, 0.5f);

    C2D_Sprite trail[TRAIL_LEN];
    for (int i = 0; i < TRAIL_LEN; ++i) {
        C2D_SpriteFromSheet(&trail[i], spriteSheet, 0);
        C2D_SpriteSetCenter(&trail[i], 0.5f, 0.5f);
    }

    consoleInit(GFX_BOTTOM, NULL);

    float px = SCREEN_W * 0.28f;
    float py = SCREEN_H * 0.5f;
    float vy = 0.0f;
    bool holding = false;
    bool started = false;

    float trail_x[TRAIL_LEN], trail_y[TRAIL_LEN], trail_angle[TRAIL_LEN];
    for (int i=0;i<TRAIL_LEN;i++){ trail_x[i]=px; trail_y[i]=py; trail_angle[i]=0; }

    TickCounter tc;
    osTickCounterStart(&tc);
    float prev = osTickCounterRead(&tc) / 1000.0f;
    while (aptMainLoop()) {
        hidScanInput();
        u32 kHeld = hidKeysHeld();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;
        holding = (kHeld & KEY_A);

        if (kDown & KEY_A) {
            if (!started) started = true;
        }

        float now = osTickCounterRead(&tc) / 1000.0f;
        float dt = now - prev;
        prev = now;
        if (dt <= 0) dt = 1.0f/60.0f;
        if (dt > 0.05f) dt = 0.05f;

        if (started) {
            float a = holding ? -THRUST : GRAVITY;
            vy += a * dt;
            if (vy > MAX_SPEED) vy = MAX_SPEED;
            if (vy < -MAX_SPEED) vy = -MAX_SPEED;
            py += vy * dt;

            float player_h = player.params.pos.h;
            float floor_y = SCREEN_H - player_h*0.5f - 4.0f;
            float ceil_y = player_h*0.5f + 4.0f;
            bool touching = false;
            if (py > floor_y) { py = floor_y; vy = 0; touching = true; }
            else if (py < ceil_y) { py = ceil_y; vy = 0; touching = true; }

            for (int i = TRAIL_LEN-1; i > 0; --i) {
                trail_x[i] = trail_x[i-1];
                trail_y[i] = trail_y[i-1];
                trail_angle[i] = trail_angle[i-1];
            }
            float angle = 0.0f;
            if (!touching) {
                float t = vy / MAX_SPEED;
                if (t > 1) t = 1;
                if (t < -1) t = -1;
                angle = t * 30.0f;
            } else angle = 0.0f;
            trail_x[0] = px;
            trail_y[0] = py;
            trail_angle[0] = angle;
        }

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32f(0,0,0,1));
        C2D_SceneBegin(top);

        if (!started) {
            printf("\x1b[1;1HGDWave - Press A to start. Hold A to go up. Start to quit.\x1b[K");
        } else {
            for (int i = TRAIL_LEN-1; i >= 0; --i) {
                float p = (float)i / (float)(TRAIL_LEN-1);
                float scale = 1.0f - p * 0.6f;
                C2D_SpriteSetPos(&trail[i], trail_x[i], trail_y[i]);
                C2D_SpriteSetRotation(&trail[i], C3D_Angle(trail_angle[i] * (M_PI/180.0f)));
                C2D_SpriteSetScale(&trail[i], scale, scale);
                C2D_DrawSprite(&trail[i]);
            }
            C2D_SpriteSetPos(&player, px, py);
            C2D_SpriteSetRotation(&player, C3D_Angle(trail_angle[0] * (M_PI/180.0f)));
            C2D_DrawSprite(&player);
        }

        C3D_FrameEnd(0);
    }

    C2D_SpriteSheetFree(spriteSheet);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
