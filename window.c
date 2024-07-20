#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

#define SPEED 600 // frame per second

SDL_Texture *player_score_texture(int player_score, TTF_Font *font,
                                  SDL_Rect *rect, SDL_Renderer *rend) {
  SDL_Color color = {255, 255, 255};
  char s[10];
  sprintf(s, "%d", player_score);
  SDL_Surface *surface = TTF_RenderText_Solid(font, s, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
  SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
  SDL_FreeSurface(surface);
  return texture;
}

void RenderStart(SDL_Texture *wall, SDL_Rect wall1_rect, SDL_Rect wall2_rect,
                 SDL_Texture *ball, SDL_Rect ball_rect, SDL_Texture *paddle,
                 SDL_Rect paddle1_rect, SDL_Rect paddle2_rect,
                 SDL_Texture *score, SDL_Rect score_rect, SDL_Texture *p1,
                 SDL_Rect p1_rect, SDL_Texture *p2, SDL_Rect p2_rect,
                 SDL_Renderer *rend) {
  p1_rect.x = 3 * (WINDOW_WIDTH - p1_rect.w) / 4;
  p1_rect.y = (WINDOW_HEIGHT - p1_rect.h) / 2;
  p2_rect.x = (WINDOW_WIDTH - p2_rect.w) / 4;
  p2_rect.y = (WINDOW_HEIGHT - p2_rect.h) / 2;
  SDL_AudioDeviceID audio_device;
  SDL_AudioSpec audiospec;
  Uint8 *wavstart;
  Uint32 wavlength;

  SDL_LoadWAV("resources/start.wav", &audiospec, &wavstart, &wavlength);
  audio_device = SDL_OpenAudioDevice(NULL, 0, &audiospec, NULL,
                                     SDL_AUDIO_ALLOW_ANY_CHANGE);
  SDL_QueueAudio(audio_device, wavstart, wavlength);
  SDL_PauseAudioDevice(audio_device, 0);
  for (int i = 0; i < 6; i++) {
    if (i % 2 == 0) {
      SDL_RenderClear(rend);
      SDL_RenderCopy(rend, wall, NULL, &wall1_rect);
      SDL_RenderCopy(rend, wall, NULL, &wall2_rect);
      SDL_RenderCopy(rend, paddle, NULL, &paddle1_rect);
      SDL_RenderCopy(rend, paddle, NULL, &paddle2_rect);
      SDL_RenderCopy(rend, score, NULL, &score_rect);
      SDL_RenderCopy(rend, p1, NULL, &p1_rect);
      SDL_RenderCopy(rend, p2, NULL, &p2_rect);
      SDL_RenderCopy(rend, ball, NULL, &ball_rect);
      SDL_RenderPresent(rend);
      SDL_Delay(300);
    } else {
      SDL_RenderClear(rend);
      SDL_RenderCopy(rend, wall, NULL, &wall1_rect);
      SDL_RenderCopy(rend, wall, NULL, &wall2_rect);
      SDL_RenderCopy(rend, paddle, NULL, &paddle1_rect);
      SDL_RenderCopy(rend, paddle, NULL, &paddle2_rect);
      SDL_RenderCopy(rend, score, NULL, &score_rect);
      SDL_RenderCopy(rend, p1, NULL, &p1_rect);
      SDL_RenderCopy(rend, p2, NULL, &p2_rect);
      SDL_RenderPresent(rend);
      SDL_Delay(200);
    }
  }
  SDL_CloseAudioDevice(audio_device);
}

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
    printf("error initialising SDL: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() != 0) {
    printf("error initialising ttf\n");
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

  SDL_Surface *ball_surface = IMG_Load("resources/ball.png");
  if (!ball_surface) {
    printf("ball deez nuts: %s", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  SDL_Texture *ball_texture = SDL_CreateTextureFromSurface(rend, ball_surface);
  SDL_FreeSurface(ball_surface);
  if (!ball_texture) {
    printf("ball text dezz nuts: %s", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Rect ball_rect, wall1_rect, wall2_rect;
  SDL_QueryTexture(ball_texture, NULL, NULL, &ball_rect.w, &ball_rect.h);
  ball_rect.x = (WINDOW_WIDTH - ball_rect.w) / 2;
  ball_rect.y = (WINDOW_HEIGHT - ball_rect.h) / 2;
  SDL_QueryTexture(wall_texture, NULL, NULL, &wall1_rect.w, &wall1_rect.h);
  wall2_rect.w = wall1_rect.w;
  wall2_rect.h = wall1_rect.h;
  wall1_rect.x = 0;
  wall2_rect.x = 0;
  wall1_rect.y = 0;
  wall2_rect.y = WINDOW_HEIGHT - wall2_rect.h;

  SDL_AudioDeviceID audio_device;
  SDL_AudioSpec audiospec;
  Uint8 *wavstart;
  Uint32 wavlength;

  SDL_LoadWAV("resources/ball.wav", &audiospec, &wavstart, &wavlength);
  audio_device = SDL_OpenAudioDevice(NULL, 0, &audiospec, NULL,
                                     SDL_AUDIO_ALLOW_ANY_CHANGE);

  SDL_Rect pad1, pad2;
  SDL_QueryTexture(tex, NULL, NULL, &pad1.w, &pad1.h);
  pad1.x = (WINDOW_WIDTH - pad1.w);
  pad1.y = (WINDOW_HEIGHT - pad1.h) / 2;
  SDL_QueryTexture(tex, NULL, NULL, &pad2.w, &pad2.h);
  pad2.x = 0;
  pad2.y = (WINDOW_HEIGHT - pad2.h) / 2;

  float pad1_pos = (WINDOW_HEIGHT - pad1.h) / 2;
  float pad2_pos = pad1_pos;
  float ball_x = ball_rect.x;
  float ball_y = ball_rect.y;

  int close_requested = 0;

  // initial ball speed
  srand(time(NULL));
  float ballx_vel = -1 * rand() % SPEED;
  float bally_vel = rand() % (SPEED / 2);

  float left_vel, right_vel = 0;
  int left_up = 0;
  int left_down = 0;
  int right_up = 0;
  int right_down = 0;
  // Score
  int player1_score = 0;
  int player2_score = 0;
  TTF_Font *font = TTF_OpenFont("resources/font.ttf", 80);
  if (!font)
    printf("error opening font\n");

  SDL_Color color = {255, 255, 255};
  SDL_Surface *score_surface = TTF_RenderText_Solid(font, "score", color);
  SDL_Texture *score_texture =
      SDL_CreateTextureFromSurface(rend, score_surface);
  SDL_Texture *player1_texture;
  SDL_Texture *player2_texture;
  SDL_Rect score_rect, p1_rect, p2_rect;
  score_rect.x = (WINDOW_WIDTH - score_surface->w) / 2;
  score_rect.y = (WINDOW_HEIGHT - score_surface->h) / 6;
  player1_texture = player_score_texture(player1_score, font, &p1_rect, rend);
  player2_texture = player_score_texture(player2_score, font, &p2_rect, rend);
  score_rect.w = score_surface->w;
  score_rect.h = score_surface->h;
  SDL_FreeSurface(score_surface);
  RenderStart(wall_texture, wall1_rect, wall2_rect, ball_texture, ball_rect,
              tex, pad1, pad2, score_texture, score_rect, player1_texture,
              p1_rect, player2_texture, p2_rect, rend);

  while (close_requested == 0) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        close_requested = 1;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
          close_requested = 1;
          break;
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
          ball_x = (WINDOW_WIDTH - ball_rect.w) / 2;
          ball_y = (WINDOW_HEIGHT - ball_rect.h) / 2;
          ballx_vel = 150 + rand() % (SPEED - 150);
          bally_vel = rand() % (SPEED / 2);
          RenderStart(wall_texture, wall1_rect, wall2_rect, ball_texture,
                      ball_rect, tex, pad1, pad2, score_texture, score_rect,
                      player1_texture, p1_rect, player2_texture, p2_rect, rend);
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

    if (left_up && !left_down)
      left_vel = -SPEED;
    if (left_down && !left_up)
      left_vel = SPEED;
    if (right_up && !right_down)
      right_vel = -SPEED;
    if (right_down && !right_up)
      right_vel = SPEED;

    pad1_pos += right_vel / 60;
    pad2_pos += left_vel / 60;

    // collision detection

    if (pad1_pos <= wall1_rect.h)
      pad1_pos = wall1_rect.h;
    if (pad2_pos <= wall1_rect.h)
      pad2_pos = wall1_rect.h;
    if (pad2_pos >= WINDOW_HEIGHT - pad2.h - wall2_rect.h)
      pad2_pos = WINDOW_HEIGHT - pad2.h - wall2_rect.h;
    if (pad1_pos >= WINDOW_HEIGHT - pad1.h - wall2_rect.h)
      pad1_pos = WINDOW_HEIGHT - pad1.h - wall2_rect.h;

    ball_x += ballx_vel / 60;
    ball_y += bally_vel / 60;
    // ball collision
    if (ball_y <= wall1_rect.h) {
      ball_y = wall1_rect.h;
      bally_vel *= -1;
      ballx_vel *= 1.2;
      SDL_QueueAudio(audio_device, wavstart, wavlength);
      SDL_PauseAudioDevice(audio_device, 0);
    }
    if (ball_y >= WINDOW_HEIGHT - wall2_rect.h - ball_rect.h) {
      ball_y = WINDOW_HEIGHT - wall2_rect.h - ball_rect.h;
      bally_vel *= -1;
      ballx_vel *= 1.2;
      SDL_QueueAudio(audio_device, wavstart, wavlength);
      SDL_PauseAudioDevice(audio_device, 0);
    }
    if (ball_x <= pad2.w) {
      if (ball_x + ball_rect.w <= 0) {
        // game over
        ball_x = (WINDOW_WIDTH - ball_rect.w) / 2;
        ball_y = (WINDOW_HEIGHT - ball_rect.h) / 2;
        ball_rect.x = (int)ball_x;
        ball_rect.y = (int)ball_y;
        ballx_vel = 150 + rand() % (SPEED - 150);
        bally_vel = rand() % (SPEED - 150);
        player1_score++;
        player1_texture =
            player_score_texture(player1_score, font, &p1_rect, rend);
        RenderStart(wall_texture, wall1_rect, wall2_rect, ball_texture,
                    ball_rect, tex, pad1, pad2, score_texture, score_rect,
                    player1_texture, p1_rect, player2_texture, p2_rect, rend);
      } else if (ball_y <= pad2_pos + pad2.h && ball_y >= pad2_pos) {
        ball_x = pad2.w;
        ballx_vel *= -1;
        if (left_vel < 0){
        bally_vel = -1*(150 + rand() % (int)(bally_vel - 150));
        } else {
        bally_vel = 150 + rand() % (int)(bally_vel - 150);
                }

        SDL_QueueAudio(audio_device, wavstart, wavlength);
        SDL_PauseAudioDevice(audio_device, 0);
      }
    }

    if (ball_x + ball_rect.w >= pad1.x) {
      if (ball_x >= WINDOW_WIDTH) {
        // game over
        ball_x = (WINDOW_WIDTH - ball_rect.w) / 2;
        ball_y = (WINDOW_HEIGHT - ball_rect.h) / 2;
        ball_rect.x = (int)ball_x;
        ball_rect.y = (int)ball_y;
        ballx_vel = 150 + rand() % (SPEED - 150);
        bally_vel = rand() % (SPEED / 2);
        player2_score++;
        player2_texture =
            player_score_texture(player2_score, font, &p2_rect, rend);
        RenderStart(wall_texture, wall1_rect, wall2_rect, ball_texture,
                    ball_rect, tex, pad1, pad2, score_texture, score_rect,
                    player1_texture, p1_rect, player2_texture, p2_rect, rend);
      } else if (ball_y >= pad1_pos && ball_y <= pad1_pos + pad1.h) {
        ball_x = pad1.x - ball_rect.w;
        ballx_vel *= -1;
        if (right_vel < 0){
        bally_vel = -1*(150 + rand() % (int)(bally_vel - 150));
        } else {
        bally_vel = 150 + rand() % (int)(bally_vel - 150);
                }
        SDL_QueueAudio(audio_device, wavstart, wavlength);
        SDL_PauseAudioDevice(audio_device, 0);
      }
    }

    pad1.y = (int)pad1_pos;
    pad2.y = (int)pad2_pos;
    ball_rect.x = (int)ball_x;
    ball_rect.y = (int)ball_y;

    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, wall_texture, NULL, &wall1_rect);
    SDL_RenderCopy(rend, wall_texture, NULL, &wall2_rect);
    SDL_RenderCopy(rend, tex, NULL, &pad1);
    SDL_RenderCopy(rend, tex, NULL, &pad2);
    SDL_RenderCopy(rend, ball_texture, NULL, &ball_rect);
    SDL_RenderPresent(rend);

    SDL_Delay(100 / 6);
  }
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_FreeWAV(wavstart);
  SDL_CloseAudioDevice(audio_device);
  SDL_DestroyTexture(wall_texture);
  SDL_DestroyTexture(tex);
  SDL_DestroyTexture(ball_texture);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
