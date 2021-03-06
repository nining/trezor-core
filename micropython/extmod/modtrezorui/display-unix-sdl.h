/*
 * Copyright (c) Pavol Rusnak, SatoshiLabs
 *
 * Licensed under TREZOR License
 * see LICENSE file for details
 */

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define DISPLAY_BORDER 16

static SDL_Renderer *RENDERER = 0;
static SDL_Surface  *BUFFER   = 0;
static SDL_Texture  *TEXTURE  = 0;
static int DATAODD = 0;
static int POSX, POSY, SX, SY, EX, EY = 0;

#define CMD(X) (void)(X);

void DATA(uint8_t x) {
    if (POSX <= EX && POSY <= EY) {
        ((uint8_t *)BUFFER->pixels)[POSX * 2 + POSY * BUFFER->pitch + (DATAODD ^ 1)] = x;
    }
    DATAODD = !DATAODD;
    if (DATAODD == 0) {
        POSX++;
        if (POSX > EX) {
            POSX = SX;
            POSY++;
        }
    }
}

// this should match values used in msg_poll_ui_event() in modtrezormsg/modtrezormsg-stmhal.h
uint32_t trezorui_poll_event(void)
{
    SDL_Event event;
    int x, y;
    SDL_PumpEvents();
    if (SDL_PollEvent(&event) > 0) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONUP:
                x = event.button.x - DISPLAY_BORDER;
                y = event.button.y - DISPLAY_BORDER;
                if (x < 0 || y < 0 || x >= DISPLAY_RESX || y >= DISPLAY_RESY) break;
                switch (event.type) {
                    case SDL_MOUSEBUTTONDOWN:
                        return (0x00 << 24) | (0x01 << 16) | (x << 8) | y; // touch_start
                        break;
                    case SDL_MOUSEMOTION:
                        // remove other SDL_MOUSEMOTION events from queue
                        SDL_FlushEvent(SDL_MOUSEMOTION);
                        if (event.motion.state) {
                            return (0x00 << 24) | (0x02 << 16) | (x << 8) | y; // touch_move
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        return (0x00 << 24) | (0x04 << 16) | (x << 8) | y; // touch_end
                        break;
                }
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    exit(3);
                }
                break;
            case SDL_QUIT:
                exit(3);
                break;
        }
    }
    return 0;
}

void display_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
    }
    SDL_Window *win = SDL_CreateWindow("TREZOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_RESX + 2 * DISPLAY_BORDER, DISPLAY_RESY + 2 * DISPLAY_BORDER, SDL_WINDOW_SHOWN);
    if (!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
    }
    RENDERER = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!RENDERER) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
    }
    SDL_SetRenderDrawColor(RENDERER, BACKLIGHT, BACKLIGHT, BACKLIGHT, 255);
    SDL_RenderClear(RENDERER);
    BUFFER = SDL_CreateRGBSurface(0, DISPLAY_RESX, DISPLAY_RESY, 16, 0xF800, 0x07E0, 0x001F, 0x0000);
    TEXTURE = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, DISPLAY_RESX, DISPLAY_RESY);
    SDL_SetTextureBlendMode(TEXTURE, SDL_BLENDMODE_NONE);
    SDL_SetTextureAlphaMod(TEXTURE, 0);
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    SX = x0; SY = y0;
    EX = x1; EY = y1;
    POSX = SX; POSY = SY;
    DATAODD = 0;
}

void display_refresh(void)
{
    SDL_RenderClear(RENDERER);
    SDL_UpdateTexture(TEXTURE, NULL, BUFFER->pixels, BUFFER->pitch);
    const SDL_Rect r = {DISPLAY_BORDER, DISPLAY_BORDER, DISPLAY_RESX, DISPLAY_RESY};
    SDL_RenderCopyEx(RENDERER, TEXTURE, NULL, &r, ORIENTATION, NULL, 0);
    SDL_RenderPresent(RENDERER);
}

int display_orientation(int degrees)
{
    if (degrees != ORIENTATION) {
        if (degrees == 0 || degrees == 90 || degrees == 180 || degrees == 270) {
            ORIENTATION = degrees;
        }
    }
    return ORIENTATION;
}

int display_backlight(int val)
{
    if (BACKLIGHT != val && val >= 0 && val <= 255) {
        BACKLIGHT = val;
        SDL_SetRenderDrawColor(RENDERER, BACKLIGHT, BACKLIGHT, BACKLIGHT, 255);
    }
    return BACKLIGHT;
}

void display_save(const char *filename)
{
    IMG_SavePNG(BUFFER, filename);
}
