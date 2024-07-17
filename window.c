#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>
#include <time.h>
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

  SDL_Surface* ball_surface = IMG_Load("resources/ball.png");
  if (!ball_surface) {
      printf("ball deez nuts: %s", SDL_GetError());
      SDL_DestroyRenderer(rend);
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
  }
  SDL_Texture* ball_texture = SDL_CreateTextureFromSurface(rend, ball_surface);
  SDL_FreeSurface(ball_surface);
  if (!ball_texture) {
      printf("ball text dezz nuts: %s", SDL_GetError());
      SDL_DestroyRenderer(rend);
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
  }

  SDL_Rect ball_rect;
  SDL_QueryTexture(ball_texture, NULL, NULL, &ball_rect.w, &ball_rect.h);
  ball_rect.x = (WINDOW_WIDTH - ball_rect.w)/2;
  ball_rect.y = (WINDOW_HEIGHT - ball_rect.h) /2;

  SDL_Rect pad1, pad2;
  SDL_QueryTexture(tex, NULL, NULL, &pad1.w, &pad1.h);
    pad1.x = (WINDOW_WIDTH - pad1.w);
  pad2.x = 0;
  pad2.w = pad1.w;
  pad2.h = pad1.h;

float  pad1_pos = (WINDOW_HEIGHT - pad1.h) / 2;
  float pad2_pos = pad1_pos;
  float ball_x = ball_rect.x;
  float ball_y = ball_rect.y;

  int close_requested = 0;

  //initial ball speed
  srand(time(NULL));
  float ballx_vel = -1*rand() % SPEED;
  float bally_vel = rand() % SPEED;

  float left_vel, right_vel = 0;
  int left_up = 0;
  int left_down = 0;
  int right_up = 0;
  int right_down = 0;

  printf("%d %d\n",left_up, left_down);

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
                      case SDL_SCANCODE_R:
                          ball_x = (WINDOW_WIDTH - ball_rect.w) /2;
                          ball_y = (WINDOW_HEIGHT - ball_rect.h) /2;
                          ballx_vel = -1 * rand() % SPEED;
                          bally_vel = -1 * rand() % SPEED;
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

      ball_x += ballx_vel / 60;
      ball_y += bally_vel /60;
      //ball collision
      if (ball_y <= 10) {
          ball_y = 10;
          bally_vel *= -1;
          ballx_vel *= 1.2;
      }
      if (ball_y >= WINDOW_HEIGHT - 10) {
          ball_y = WINDOW_HEIGHT - 10;
          bally_vel *= -1;
          ballx_vel *= 1.2;
      }
      if (ball_x <= pad2.w){
          if ( ball_x <= 0) {
              //game over
              ballx_vel = 0;
              bally_vel = 0;
          }
          else if ( ball_y <= pad2.y + pad2.h && ball_y >= pad2.y){
              ball_x = pad2.w;
              ballx_vel *= -0.8;
          }
      }

      
      if (ball_x + ball_rect.w >= pad1.x) {
          if (ball_x + ball_rect.w >= WINDOW_WIDTH) {
              //game over
              ballx_vel = 0;
              bally_vel = 0;
          }
          else if ( ball_y >= pad2.y && ball_y <= pad2.y + pad2.h){
              ball_x = pad1.x - ball_rect.w;
              ballx_vel *= -0.8;
      }
      }

      ball_rect.x = (int) ball_x;
      ball_rect.y = (int) ball_y;

    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, wall_texture, NULL, NULL);
    SDL_RenderCopy(rend, tex, NULL, &pad1);
    SDL_RenderCopy(rend, tex, NULL, &pad2);
    SDL_RenderCopy(rend, ball_texture, NULL, &ball_rect);
    SDL_RenderPresent(rend);

    SDL_Delay( 100/6);
  }
  SDL_DestroyTexture(wall_texture);
  SDL_DestroyTexture(tex);
  SDL_DestroyTexture(ball_texture);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
