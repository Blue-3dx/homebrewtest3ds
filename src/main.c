
// GDWave - SDL2 prototype (desktop)
// Controls:
//  - Press Z or A key to hold "A" button (go up). Release to go down.
//  - Press Z / A on the title screen to start.
// Notes:
//  - This is a rapid prototype to test mechanics and visuals on desktop (SDL2).
//  - Porting notes for 3DS are included in README.md.
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_W 800
#define WINDOW_H 600

// Wave physics
const float GRAVITY = 1200.0f;      // pixels / s^2 downward
const float THRUST = 1600.0f;       // upward acceleration when holding
const float MAX_SPEED = 900.0f;     // cap vertical speed for rotation calculation
const float HORIZ_SPEED = 240.0f;   // pixels / s - used for "autoscroll" feel

// Trail
#define TRAIL_LEN 18
typedef struct { float x,y; float angle; } TrailPoint;

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
        // continue, but image may fail
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    }

    SDL_Window *win = SDL_CreateWindow("GDWave - prototype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    SDL_Renderer *rnd = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load texture
    SDL_Surface *surf = IMG_Load("assets/wave.png");
    if (!surf) {
        fprintf(stderr, "Failed to load assets/wave.png: %s\n", IMG_GetError());
        // create a simple triangle surface as fallback
        surf = SDL_CreateRGBSurface(0,64,32,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
        SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 255,255,255,255));
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(rnd, surf);
    int tex_w = surf->w, tex_h = surf->h;
    SDL_FreeSurface(surf);

    // Font for messages
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        // try to continue without font
    }

    bool running = true;
    bool holding = false;
    bool started = false;
    Uint64 prev = SDL_GetPerformanceCounter();
    float py = WINDOW_H * 0.5f;
    float vx = 0.0f;
    float vy = 0.0f;

    // Keep the player at 30% width from left
    float px = WINDOW_W * 0.28f;
    float player_w = tex_w, player_h = tex_h;

    // trail init
    TrailPoint trail[TRAIL_LEN];
    for (int i=0;i<TRAIL_LEN;i++){ trail[i].x=px; trail[i].y=py; trail[i].angle=0; }

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_Z || e.key.keysym.scancode == SDL_SCANCODE_A) {
                    holding = true;
                    if (!started) started = true;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
            }
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.scancode == SDL_SCANCODE_Z || e.key.keysym.scancode == SDL_SCANCODE_A) {
                    holding = false;
                }
            }
        }

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (now - prev) / (float)SDL_GetPerformanceFrequency();
        prev = now;
        if (dt > 0.05f) dt = 0.05f;

        if (started) {
            // apply physics
            float a = holding ? -THRUST : GRAVITY;
            vy += a * dt;

            // cap speed (for rotation calculations)
            if (vy > MAX_SPEED) vy = MAX_SPEED;
            if (vy < -MAX_SPEED) vy = -MAX_SPEED;

            // simple vertical integrate
            py += vy * dt;

            // collision with floor and ceiling
            float floor_y = WINDOW_H - player_h*0.5f - 10.0f; // 10px padding
            float ceil_y = player_h*0.5f + 10.0f;
            bool touching = false;
            if (py > floor_y) {
                py = floor_y;
                vy = 0;
                touching = true;
            } else if (py < ceil_y) {
                py = ceil_y;
                vy = 0;
                touching = true;
            }

            // autoscroll simulation (not moving obstacles here)
            vx = HORIZ_SPEED;

            // update trail: shift right, insert current
            for (int i=TRAIL_LEN-1;i>0;i--) trail[i]=trail[i-1];
            // rotation calculation: when touching ceiling/floor -> 0 angle.
            float angle = 0.0f;
            if (!touching) {
                // map vy to angle range [-30, +30] degrees (upwards negative vy)
                float t = vy / MAX_SPEED;
                if (t>1) t=1;
                if (t<-1) t=-1;
                angle = (t) * 30.0f; // positive vy => rotate down
            } else {
                angle = 0.0f;
            }
            trail[0].x = px;
            trail[0].y = py;
            trail[0].angle = angle;
        } // if started

        // render
        SDL_SetRenderDrawColor(rnd, 0,0,0,255);
        SDL_RenderClear(rnd);

        if (!started) {
            // draw title + message
            if (font) {
                SDL_Color white = {255,255,255,255};
                SDL_Surface *s1 = TTF_RenderUTF8_Solid(font, "GDWave", white);
                SDL_Surface *s2 = TTF_RenderUTF8_Solid(font, "Press Z / A to start", white);
                if (s1 && s2) {
                    SDL_Texture *t1 = SDL_CreateTextureFromSurface(rnd, s1);
                    SDL_Texture *t2 = SDL_CreateTextureFromSurface(rnd, s2);
                    SDL_Rect r1 = { (WINDOW_W - s1->w)/2, (WINDOW_H/2)-60, s1->w, s1->h };
                    SDL_Rect r2 = { (WINDOW_W - s2->w)/2, (WINDOW_H/2), s2->w, s2->h };
                    SDL_RenderCopy(rnd, t1, NULL, &r1);
                    SDL_RenderCopy(rnd, t2, NULL, &r2);
                    SDL_DestroyTexture(t1); SDL_DestroyTexture(t2);
                }
                if (s1) SDL_FreeSurface(s1);
                if (s2) SDL_FreeSurface(s2);
            } else {
                // fallback: draw a simple rectangle as indicator
                SDL_SetRenderDrawColor(rnd, 255,255,255,255);
                SDL_Rect r = { (WINDOW_W/2)-160, (WINDOW_H/2)-16, 320, 32 };
                SDL_RenderFillRect(rnd, &r);
            }
        } else {
            // draw trail with fading alpha
            for (int i=TRAIL_LEN-1;i>=0;i--) {
                float p = (float)i / (float)(TRAIL_LEN-1);
                Uint8 alpha = (Uint8)(255 * (1.0f - p) * 0.6f); // older -> more faded
                SDL_SetTextureAlphaMod(tex, alpha);
                SDL_Rect dst = { (int)(trail[i].x - player_w*0.5f), (int)(trail[i].y - player_h*0.5f), (int)player_w, (int)player_h };
                SDL_RenderCopyEx(rnd, tex, NULL, &dst, trail[i].angle, NULL, SDL_FLIP_NONE);
            }
            // reset alpha
            SDL_SetTextureAlphaMod(tex, 255);

            // draw player (no rotation if touching floor/ceiling)
            float current_angle = trail[0].angle;
            SDL_Rect player_dst = { (int)(px - player_w*0.5f), (int)(py - player_h*0.5f), (int)player_w, (int)player_h };
            SDL_RenderCopyEx(rnd, tex, NULL, &player_dst, current_angle, NULL, SDL_FLIP_NONE);
        }

        SDL_RenderPresent(rnd);
    }

    if (tex) SDL_DestroyTexture(tex);
    if (font) TTF_CloseFont(font);
    if (rnd) SDL_DestroyRenderer(rnd);
    if (win) SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
