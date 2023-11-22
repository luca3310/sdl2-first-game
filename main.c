#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "./constants.h"
#include <time.h>

int game_is_running = FALSE;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_DisplayMode displayMode;
   
int last_frame_time = 0;

struct ball
{
   float x;
   float y;
   float width;
   float height;
   int left;
   int up;
   int down;
   int right;
   double dashCd;
} ball;

struct enemy
{
   float x;
   float y;
   float width;
   float height;
} enemy;

struct bullet
{
   float x;
   float y;
   float width;
   float height;
   double dir;
   double prevDir;
   double speedUp;
   int isFired;
} bullet;



// struct enemyBullet
// {
//    float x;
//    float y;
//    float width;
//    float height;
//    double dir;
//    double isFired;
// } enemyBullet;

int initialize_window(void)
{
   if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
   {
      fprintf(stderr, "Error initializing SDL\n");
      return FALSE;
   }
   else
   {
      SDL_GetCurrentDisplayMode(0, &displayMode);

      window = SDL_CreateWindow(
          "game",
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          displayMode.w,
          displayMode.h,
          SDL_WINDOW_FULLSCREEN);
      if (!window)
      {
         fprintf(stderr, "Error creating SDL Window.\n");
         return FALSE;
      }

      renderer = SDL_CreateRenderer(window, -1, 0);
      if (!renderer)
      {
         fprintf(stderr, "Error creating SDL renderer.\n");
         return FALSE;
      }

      return TRUE;
   }
}

int check_collision(SDL_Rect rect1, SDL_Rect rect2)
{
   return (rect1.x < rect2.x + rect2.w &&
           rect1.x + rect1.w > rect2.x &&
           rect1.y < rect2.y + rect2.h &&
           rect1.y + rect1.h > rect2.y);
}

float random_float(float min, float max)
{
   return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void process_input()
{
   SDL_Event event;
   SDL_PollEvent(&event);

   switch (event.type)
   {
   case SDL_QUIT:
      game_is_running = FALSE;
      break;
   case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
         game_is_running = FALSE;
      if (event.key.keysym.sym == SDLK_w)
      {
         ball.up = 1;
      };
      if (event.key.keysym.sym == SDLK_s)
      {
         ball.down = 1;
      };
      if (event.key.keysym.sym == SDLK_a)
      {
         ball.left = 1;
      };
      if (event.key.keysym.sym == SDLK_d)
      {
         ball.right = 1;
      };
      if (event.key.keysym.sym == SDLK_e && bullet.isFired == 0)
      {
         int x, y;
         SDL_GetMouseState(&x, &y);
         bullet.dir = atan2(y - ball.y, x - ball.x);
         bullet.prevDir = atan2(y - ball.y, x - ball.x);
         bullet.x = ball.x;
         bullet.y = ball.y;
         bullet.isFired = 1;
         bullet.speedUp = 1000;
      };
      if (event.key.keysym.sym == SDLK_e && bullet.isFired == 2) {
         bullet.isFired = 1;
         bullet.speedUp = 1000;
      }
      if (event.key.keysym.sym == SDLK_q && bullet.isFired == 1)
      {
         bullet.isFired = 2;
         bullet.speedUp = 1000;
      };
      if (event.key.keysym.sym == SDLK_SPACE && ball.dashCd == 0)
      {
         int multiDer;
         if (ball.up + ball.down + ball.left + ball.right == 2)
         {
            multiDer = 300;
         }
         else
         {
            multiDer = 600;
         }

         if (ball.up)
         {
            ball.y -= multiDer;
         }
         if (ball.down)
         {
            ball.y += multiDer;
         }
         if (ball.left)
         {
            ball.x -= multiDer;
         }
         if (ball.right)
         {
            ball.x += multiDer;
         }
         ball.dashCd = 6;
      };
      break;

   case SDL_KEYUP:
      if (event.key.keysym.sym == SDLK_w)
      {
         ball.up = 0;
      };
      if (event.key.keysym.sym == SDLK_s)
      {
         ball.down = 0;
      };
      if (event.key.keysym.sym == SDLK_a)
      {
         ball.left = 0;
      };
      if (event.key.keysym.sym == SDLK_d)
      {
         ball.right = 0;
      };
   };
};

void setup()
{
   ball.x = 20;
   ball.y = 20;
   ball.width = 15;
   ball.height = 15;
   ball.up = 0;
   ball.down = 0;
   ball.left = 0;
   ball.right = 0;
   ball.dashCd = 0;

   enemy.x = 300;
   enemy.y = 300;
   enemy.width = 15;
   enemy.height = 15;

   bullet.isFired = 0;
   bullet.width = 10;
   bullet.height = 10;
   bullet.speedUp = 0;

   srand(time(NULL));

   // enemyBullet.isFired = 0;
   // enemyBullet.width = 10;
   // enemyBullet.height = 10;
};

void update()
{
   while (!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME))
      ;

   float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

   last_frame_time = SDL_GetTicks();


   SDL_Rect enemy_rect = {(int)enemy.x, (int)enemy.y, (int)enemy.width, (int)enemy.height};
   SDL_Rect ball_rect = {(int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height};
   if (check_collision(ball_rect, enemy_rect))
   {
      ball.x = 20;
      ball.y = 20;
      bullet.isFired = 0;
      enemy.x = 300;
      enemy.y = 300;
   }

   if (bullet.isFired == 1 || bullet.isFired == 2)
   {
      float bullet_speed = 400.0f + bullet.speedUp;
         SDL_Rect bullet_rect = {(int)bullet.x, (int)bullet.y, (int)bullet.width, (int)bullet.height};

      if (bullet.isFired == 2)
      {
         bullet_speed = 700.0f + bullet.speedUp;
         bullet.dir = atan2(ball.y - bullet.y, ball.x - bullet.x);
         if (check_collision(bullet_rect, ball_rect)) {
            bullet.isFired = 0;
         }
         bullet.x += cos(bullet.dir) * bullet_speed * delta_time;
         bullet.y += sin(bullet.dir) * bullet_speed * delta_time;
      } else {
         bullet.x += cos(bullet.prevDir) * bullet_speed * delta_time;
         bullet.y += sin(bullet.prevDir) * bullet_speed * delta_time;
      }
      


      if (check_collision(bullet_rect, enemy_rect))
      {
         enemy.x = random_float(0, displayMode.w - enemy.width);
         enemy.y = random_float(0, displayMode.h - enemy.height);
      }

      if (bullet.x < 0 || bullet.x > displayMode.w || bullet.y < 0 || bullet.y > displayMode.h) {
         bullet.isFired = 2;
      }

   }

   // if (enemyBullet.isFired == 0) {
   //    enemyBullet.x = enemy.x;
   //    enemyBullet.y = enemy.y;
   //    enemyBullet.dir = atan2(enemyBullet.x - ball.y, enemyBullet.x - ball.x);
   //    enemyBullet.x += cos(enemyBullet.dir) * 400.0f * delta_time;
   //    enemyBullet.y += sin(enemyBullet.dir) * 400.0f * delta_time;
   // }

   if (ball.dashCd > 0)
   {
      ball.dashCd -= delta_time;
      if (ball.dashCd < 0)
      {
         ball.dashCd = 0;
      }
   }

   if (bullet.speedUp > 0)
   {
      bullet.speedUp -= delta_time * 1300;
      if (bullet.speedUp < 0)
      {
         bullet.speedUp = 0;
      }
   }

   int multiDer;
    if (ball.up + ball.down + ball.left + ball.right == 2)
    {
        multiDer = 100;
    }
    else
    {
        multiDer = 200;
    }

    if (ball.up && ball.y > 0)
    {
        ball.y -= multiDer * delta_time;
    }
    if (ball.down && ball.y < displayMode.h - ball.height)
    {
        ball.y += multiDer * delta_time;
    }
    if (ball.left && ball.x > 0)
    {
        ball.x -= multiDer * delta_time;
    }
    if (ball.right && ball.x < displayMode.w - ball.width)
    {
        ball.x += multiDer * delta_time;
    }

    if (ball.x < 0)
    {
        ball.x = 0;
    }
    if (ball.x > displayMode.w - ball.width)
    {
        ball.x = displayMode.w - ball.width;
    }
    if (ball.y < 0)
    {
        ball.y = 0;
    }
    if (ball.y > displayMode.h - ball.height)
    {
        ball.y = displayMode.h - ball.height;
    }
};

void render()
{
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);

   SDL_Rect ball_rect = {
       (int)ball.x,
       (int)ball.y,
       (int)ball.width,
       (int)ball.height};
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
   SDL_RenderFillRect(renderer, &ball_rect);

   SDL_Rect enemy_rect = {
       (int)enemy.x,
       (int)enemy.y,
       (int)enemy.width,
       (int)enemy.height};
   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
   SDL_RenderFillRect(renderer, &enemy_rect);

   if (bullet.isFired == 1 || bullet.isFired == 2)
   {
      SDL_Rect bullet_rect = {
          (int)bullet.x,
          (int)bullet.y,
          (int)bullet.width,
          (int)bullet.height};
      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
      SDL_RenderFillRect(renderer, &bullet_rect);
   }

   SDL_RenderPresent(renderer);
};

void destroy_window()
{
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}

int main(int argc, char *argv[])
{
   game_is_running = initialize_window();

   setup();

   while (game_is_running)
   {
      process_input();
      update();
      render();
   }

   destroy_window();

   return 0;
}