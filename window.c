#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

#define SPEED 300 //frame per second

int main(void) {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    printf("error initialising SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *win =
      SDL_CreateWindow("pingPONG", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if (!win) {
    printf("Error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1, rendererFlags);

  if (!rend) {
    printf("Renderer creation error: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Surface *pad = IMG_Load("resources/paddle.jpeg");
  if (!pad) {
    printf("error creating pad1 surface\n");
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Surface *wall_surface = IMG_Load("resources/wall.png");
  if (!wall_surface) {
    printf("error creating pad1 surface\n");
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, pad);
  SDL_FreeSurface(pad);
  if (!tex) {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Texture *wall_texture = SDL_CreateTextureFromSurface(rend, wall_surface);
  SDL_FreeSurface(wall_surface);
  if (!wall_texture) {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Rect pad1, pad2;
  SDL_QueryTexture(tex, NULL, NULL, &pad1.w, &pad1.h);
    pad1.x = (WINDOW_WIDTH - pad1.w);
  pad2.x = 0;
  pad2.w = pad1.w;
  pad2.h = pad1.h;

float  pad1_pos = (WINDOW_HEIGHT - pad1.h) / 2;
  float pad2_pos = pad1_pos;

  int close_requested = 0;
  float left_vel, right_vel = 0;
  int left_up = 0;
  int left_down = 0;
  int right_up = 0;
  int right_down = 0;

  printf("%d %d\n",left_up, left_down);

    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, wall_texture, NULL, NULL);
    SDL_RenderPresent(rend);
  while (close_requested == 0) {

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
          switch (event.type) {
              case SDL_QUIT:
                  close_requested = 1;
                  break;
              case SDL_KEYDOWN:
                  switch (event.key.keysym.scancode) {
                      case SDL_SCANCODE_W:
                          left_up = 1;
                          break;
                      case SDL_SCANCODE_S:
                          left_down = 1;
                          break;
                      case SDL_SCANCODE_UP:
                          right_up = 1;
                          break;
                      case SDL_SCANCODE_DOWN:
                          right_down = 1;
                          break;
                  }
                  break;
              case SDL_KEYUP:
                  switch (event.key.keysym.scancode) {
                      case SDL_SCANCODE_W:
                          left_up = 0;
                          break;
                      case SDL_SCANCODE_S:
                          left_down = 0;
                          break;
                      case SDL_SCANCODE_UP:
                          right_up = 0;
                      case SDL_SCANCODE_DOWN:
                          right_down = 0;
                          break;
                  }
                  break;
          }
      }

      left_vel = right_vel = 0;

      if ( left_up && !left_down) left_vel = -SPEED;
      if ( left_down && !left_up) left_vel = SPEED;
      if ( right_up && !right_down) right_vel = -SPEED;
      if ( right_down && !right_up) right_vel = SPEED;

      pad1_pos += right_vel / 60;
      pad2_pos += left_vel / 60;

      //collision detection
      
      if (pad1_pos <= 0) pad1_pos = 0;
      if (pad2_pos <= 0) pad2_pos = 0;
      if (pad2_pos >= WINDOW_HEIGHT - pad2.h) pad2_pos = WINDOW_HEIGHT - pad2.h;
      if (pad1_pos >= WINDOW_HEIGHT - pad1.h) pad1_pos = WINDOW_HEIGHT - pad1.h;

      pad1.y = (int) pad1_pos;
      pad2.y = (int) pad2_pos;

    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, wall_texture, NULL, NULL);
    SDL_RenderCopy(rend, tex, NULL, &pad1);
    SDL_RenderCopy(rend, tex, NULL, &pad2);
    SDL_RenderPresent(rend);

    SDL_Delay( 100/6);
  }
  SDL_DestroyTexture(wall_texture);
  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
