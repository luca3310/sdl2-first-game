#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "./constants.h"
#include <time.h>

int counter = 0;

int game_is_running = FALSE;
int game_is_pause = FALSE;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_DisplayMode displayMode;

// SDL_Surface *image;
// SDL_Texture *ourPNG;

TTF_Font *ourFont;
SDL_Surface *surfaceText = NULL;
SDL_Texture *textureText = NULL;

float fps;
double fpsCd = 1;

SDL_Texture *number0TextureText = NULL;
SDL_Texture *number1TextureText = NULL;
SDL_Texture *number2TextureText = NULL;
SDL_Texture *number3TextureText = NULL;
SDL_Texture *number4TextureText = NULL;
SDL_Texture *number5TextureText = NULL;
SDL_Texture *number6TextureText = NULL;
SDL_Texture *number7TextureText = NULL;
SDL_Texture *number8TextureText = NULL;
SDL_Texture *number9TextureText = NULL;

int last_frame_time = 0;
int score = 0;

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
   double dashSpeed;
} ball;

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

struct EnemyBullet
{
   float bx;
   float by;
   float bWidth;
   float bHeight;
   double bDir;
   int isFired;
};

struct Enemy
{
   float x;
   float y;
   float width;
   float height;
   float bulletCd;
   struct EnemyBullet eBullet;
};

struct EnemyList
{
   struct Enemy *enemies;
   int size;
} enemyList;

void initEnemyList(struct EnemyList *list)
{
   list->enemies = NULL;
   list->size = 0;
}

void addEnemy(struct EnemyList *list, struct Enemy newEnemy)
{
   list->enemies = realloc(list->enemies, (list->size + 1) * sizeof(struct Enemy));
   if (list->enemies != NULL)
   {
      list->enemies[list->size] = newEnemy;
      list->size++;
   }
   else
   {
      printf("Error: Memory allocation failed\n");
   }
}

void removeEnemy(struct EnemyList *list, int index)
{
   if (index >= 0 && index < list->size)
   {
      // Shift elements to fill the gap
      for (int i = index; i < list->size - 1; i++)
      {
         list->enemies[i] = list->enemies[i + 1];
      }
      list->size--;

      // Resize the allocated memory
      list->enemies = realloc(list->enemies, list->size * sizeof(struct Enemy));
   }
   else
   {
      printf("Error: Invalid index\n");
   }
}

void cleanupEnemyList(struct EnemyList *list)
{
   free(list->enemies);
   list->enemies = NULL;
   list->size = 0;
}

struct spawner
{
   double currentSpawnCooldown;
   int maxSpawnCooldown;
   int spawnAmount;
   double spawnAmountIncreaseCooldown;
   int maxSpawnAmountIncreaseCooldown;
} spawner;

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
          0);
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

      if (TTF_Init() == -1)
      {
         fprintf(stderr, "Error initializing TTF\n");
         return FALSE;
      }

      ourFont = TTF_OpenFont("./fonts/IMMORTAL.ttf", 32);
      if (ourFont == NULL)
      {
         fprintf(stderr, "Error opening font\n");
         return FALSE;
      }

      // int flags = IMG_INIT_PNG;
      // int initStatus = IMG_Init(flags);

      // if ((initStatus && flags) != flags)
      // {
      //    printf("SDL2_Image format not available");
      // }

      // image = IMG_Load("./images/mario.png");
      // if (!image)
      // {
      //    printf("Image not loaded...");
      // }

      // ourPNG = SDL_CreateTextureFromSurface(renderer, image);

      return TRUE;
   }
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
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         game_is_running = FALSE;
         break;
      case SDLK_w:
         ball.up = 1;
         break;
      case SDLK_s:
         ball.down = 1;
         break;
      case SDLK_a:
         ball.left = 1;
         break;
      case SDLK_d:
         ball.right = 1;
         break;
      case SDLK_SPACE:
         if (ball.dashCd == 0)
         {
            ball.dashSpeed = 1000;
            ball.dashCd = 2;
         }
         break;
      case SDLK_x:
         game_is_pause = TRUE;
         break;
      }
      break;
   case SDL_MOUSEBUTTONDOWN:
      switch (event.button.button)
      {
      case SDL_BUTTON_LEFT:
         if (bullet.isFired == 0)
         {
            int x, y;
            SDL_GetMouseState(&x, &y);
            bullet.dir = atan2(y - ball.y, x - ball.x);
            bullet.prevDir = atan2(y - ball.y, x - ball.x);
            bullet.x = ball.x;
            bullet.y = ball.y;
            bullet.isFired = 1;
            bullet.speedUp = 1000;
         }
         else if (bullet.isFired == 2)
         {
            if (event.button.button == SDL_BUTTON_LEFT && bullet.isFired == 2)
            {
               bullet.isFired = 1;
               bullet.speedUp = 1000;
            }
         }
         break;
      case SDL_BUTTON_RIGHT:
         if (bullet.isFired == 1)
         {
            bullet.isFired = 2;
            bullet.speedUp = 1000;
            break;
         }
      }
      break;

   case SDL_KEYUP:
      switch (event.key.keysym.sym)
      {
      case SDLK_w:
         ball.up = 0;
         break;
      case SDLK_s:
         ball.down = 0;
         break;
      case SDLK_a:
         ball.left = 0;
         break;
      case SDLK_d:
         ball.right = 0;
         break;
         ;
      }
      break;
   };
};

void pause_process_input()
{
   SDL_Event pause_event;
   SDL_PollEvent(&pause_event);
   switch (pause_event.type)
   {
   case SDL_QUIT:
      game_is_running = FALSE;
      break;
   case SDL_KEYDOWN:
      switch (pause_event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         game_is_running = FALSE;
         break;
      case SDLK_x:
         game_is_pause = FALSE;
         break;
      }
      break;
   }
}

void player_setup()
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
   ball.dashSpeed = 0;

   bullet.isFired = 0;
   bullet.width = 10;
   bullet.height = 10;
   bullet.speedUp = 0;
}

void enemy_setup()
{
   spawner.maxSpawnCooldown = 5;
   spawner.currentSpawnCooldown = 1;
   spawner.spawnAmount = 1;
   spawner.spawnAmountIncreaseCooldown = 20;
   spawner.maxSpawnAmountIncreaseCooldown = 20;

   initEnemyList(&enemyList);
}
void texture_setup()
{
   SDL_Color white = {0xFF, 0xFF, 0xFF, 0};

   surfaceText = TTF_RenderText_Solid(ourFont, "0", white);
   number0TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "1", white);
   number1TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "2", white);
   number2TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "3", white);
   number3TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "4", white);
   number4TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "5", white);
   number5TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "6", white);
   number6TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "7", white);
   number7TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "8", white);
   number8TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "9", white);
   number9TextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);
}
void setup()
{
   player_setup();
   enemy_setup();
   texture_setup();
   srand(time(NULL));
};

int check_collision(SDL_Rect rect1, SDL_Rect rect2)
{
   return (rect1.x < rect2.x + rect2.w &&
           rect1.x + rect1.w > rect2.x &&
           rect1.y < rect2.y + rect2.h &&
           rect1.y + rect1.h > rect2.y);
}



void cap_frame_rate(Uint64 starting_tick)
{
    Uint64 target_ticks = starting_tick + SDL_GetPerformanceFrequency() / MAX_FPS;

    Uint64 current_tick = SDL_GetPerformanceCounter();
    if (current_tick < target_ticks)
    {
        Uint64 delay_ticks = target_ticks - current_tick;
        Uint32 delay_ms = (delay_ticks * 1000) / SDL_GetPerformanceFrequency();
        SDL_Delay(delay_ms);
    }
}

double setup_update()
{
    static Uint64 last_counter = 0;
    Uint64 counter = SDL_GetPerformanceCounter();

    double delta_time = (double)(counter - last_counter) / SDL_GetPerformanceFrequency();
    last_counter = counter;

    cap_frame_rate(counter);

    return delta_time;
}

void calculate_fps(double delta_time) {
    static double accumulated_time = 0.0;
    static int frames = 0;

    accumulated_time += delta_time;
    frames++;

    if (accumulated_time >= 1.0) {
        fps = frames / accumulated_time;
        accumulated_time = 0.0;
        frames = 0;
    }
}


void enemy_update(SDL_Rect ball_rect, double delta_time) {
   for (int i = 0; i < enemyList.size; i++)
   {
      if (enemyList.enemies[i].eBullet.isFired == 0)
      {

         enemyList.enemies[i].eBullet.isFired = 1;
         enemyList.enemies[i].eBullet.bx = enemyList.enemies[i].x;
         enemyList.enemies[i].eBullet.by = enemyList.enemies[i].y;
         enemyList.enemies[i].eBullet.bDir = atan2(ball.y - enemyList.enemies[i].eBullet.by, ball.x - enemyList.enemies[i].eBullet.bx);
      }
      else
      {
         enemyList.enemies[i].eBullet.bx += 400.0f * cos(enemyList.enemies[i].eBullet.bDir) * delta_time;
         enemyList.enemies[i].eBullet.by += 400.0f * sin(enemyList.enemies[i].eBullet.bDir) * delta_time;
         if (enemyList.enemies[i].eBullet.bx < 0 || enemyList.enemies[i].eBullet.bx > displayMode.w - enemyList.enemies[i].eBullet.bWidth || enemyList.enemies[i].eBullet.by < 0 || enemyList.enemies[i].eBullet.by > displayMode.h - enemyList.enemies[i].eBullet.bHeight)
         {
            enemyList.enemies[i].eBullet.isFired = 0;
         }
      }
      SDL_Rect eBullet_rect = {(int)enemyList.enemies[i].eBullet.bx, (int)enemyList.enemies[i].eBullet.by, 10, 10};
      if (check_collision(ball_rect, eBullet_rect))
      {
         // game over
      }
   }
}

void bullet_update(SDL_Rect ball_rect, double delta_time) {
   if (bullet.isFired == 1 || bullet.isFired == 2)
   {
      float bullet_speed = 400.0f + bullet.speedUp;
      SDL_Rect bullet_rect = {(int)bullet.x, (int)bullet.y, (int)bullet.width, (int)bullet.height};

      if (bullet.isFired == 2)
      {
         bullet_speed = 700.0f + bullet.speedUp;
         bullet.dir = atan2(ball.y - bullet.y, ball.x - bullet.x);
         if (check_collision(bullet_rect, ball_rect))
         {
            bullet.isFired = 0;
         }
         bullet.x += cos(bullet.dir) * bullet_speed * delta_time;
         bullet.y += sin(bullet.dir) * bullet_speed * delta_time;
      }
      else
      {
         bullet.x += cos(bullet.prevDir) * bullet_speed * delta_time;
         bullet.y += sin(bullet.prevDir) * bullet_speed * delta_time;
      }

      for (int i = 0; i < enemyList.size; i++)
      {
         SDL_Rect enemy_rect = {(int)enemyList.enemies[i].x, (int)enemyList.enemies[i].y, (int)enemyList.enemies[i].width, (int)enemyList.enemies[i].height};
         if (check_collision(bullet_rect, enemy_rect))
         {
            removeEnemy(&enemyList, i);
            score += 1;
         }
      }

      if (bullet.x < 0 || bullet.x > displayMode.w || bullet.y < 0 || bullet.y > displayMode.h)
      {
         bullet.isFired = 2;
      }
   }
}

void update_spawner () {
   if (spawner.spawnAmountIncreaseCooldown == 0)
   {
      spawner.spawnAmountIncreaseCooldown = spawner.maxSpawnAmountIncreaseCooldown;
      spawner.spawnAmount += 1;
   }

   if (spawner.currentSpawnCooldown == 0)
   {
      spawner.currentSpawnCooldown = spawner.maxSpawnCooldown;
      for (int i = 0; i < spawner.spawnAmount; ++i)
      {
         struct Enemy enemyInstance = {random_float(0, displayMode.w - 15), random_float(0, displayMode.h - 15), 15, 15, 2.0, {0, 0, 10, 10, 0, 0}};
         addEnemy(&enemyList, enemyInstance);
      }
   }
}

void player_movement (double delta_time) {
   int player_speed = 200 + ball.dashSpeed;

   if (ball.up && ball.y > 0)
   {
      ball.y -= player_speed * delta_time;
   }
   if (ball.down && ball.y < displayMode.h - ball.height)
   {
      ball.y += player_speed * delta_time;
   }
   if (ball.left && ball.x > 0)
   {
      ball.x -= player_speed * delta_time;
   }
   if (ball.right && ball.x < displayMode.w - ball.width)
   {
      ball.x += player_speed * delta_time;
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
}

void cooldown_decrease (double *cd, double delta_time, int multiplier) {
      if (*cd > 0) {
         *cd -= delta_time * multiplier;
         if (*cd < 0) {
            *cd = 0;
         }
      }
}


void update()
{
   double delta_time = setup_update();

   calculate_fps(delta_time);

   SDL_Rect ball_rect = {(int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height};

   enemy_update(ball_rect, delta_time);
   bullet_update(ball_rect, delta_time);
   update_spawner();
   player_movement(delta_time);  

   cooldown_decrease(&spawner.currentSpawnCooldown, delta_time, 1);
   cooldown_decrease(&ball.dashCd, delta_time, 1);
   cooldown_decrease(&spawner.currentSpawnCooldown, delta_time, 1);
   cooldown_decrease(&bullet.speedUp, delta_time, 1300);
   cooldown_decrease(&ball.dashSpeed, delta_time, 3000);
};

void render_player() {

   SDL_Rect ball_rect = {
       (int)ball.x,
       (int)ball.y,
       (int)ball.width,
       (int)ball.height};
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
   SDL_RenderFillRect(renderer, &ball_rect);

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
}

void render_enemies() {
   for (int i = 0; i < enemyList.size; i++)
   {
      SDL_Rect enemy_rect = {
          (int)enemyList.enemies[i].x,
          (int)enemyList.enemies[i].y,
          (int)enemyList.enemies[i].width,
          (int)enemyList.enemies[i].height};
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderFillRect(renderer, &enemy_rect);

      SDL_Rect enemy_bullet_rect = {
          (int)enemyList.enemies[i].eBullet.bx,
          (int)enemyList.enemies[i].eBullet.by,
          (int)enemyList.enemies[i].eBullet.bWidth,
          (int)enemyList.enemies[i].eBullet.bHeight};

      SDL_RenderFillRect(renderer, &enemy_bullet_rect);
   }
}

void draw_number(int number, int xpos) {
   if (number == 0) {
         SDL_Rect number_rect = {(int)xpos, (int)30, (int)30, (int)30};
         SDL_RenderCopy(renderer, number0TextureText, NULL, &number_rect);
         return;
   }

   int digitCount = 0;
   int temp = number;
   int originalTemp = number;

   // Calculate the number of digits in the given number
   while (temp != 0)
   {
      temp /= 10;
      ++digitCount;
   }

   temp = originalTemp;
   for (int i = 0; i < digitCount; ++i)
   {  
      int digit = temp % 10;

      SDL_Rect number_rect = {(int)xpos, (int)30, (int)30, (int)30};


      // Ensure that the rendering position is within the visible range

      switch (digit)
      {
      case 0:
         SDL_RenderCopy(renderer, number0TextureText, NULL, &number_rect);
         break;
      case 1:
         SDL_RenderCopy(renderer, number1TextureText, NULL, &number_rect);
         break;
      case 2:
         SDL_RenderCopy(renderer, number2TextureText, NULL, &number_rect);
         break;
      case 3:
         SDL_RenderCopy(renderer, number3TextureText, NULL, &number_rect);
         break;
      case 4:
         SDL_RenderCopy(renderer, number4TextureText, NULL, &number_rect);
         break;
      case 5:
         SDL_RenderCopy(renderer, number5TextureText, NULL, &number_rect);
         break;
      case 6:
         SDL_RenderCopy(renderer, number6TextureText, NULL, &number_rect);
         break;
      case 7:
         SDL_RenderCopy(renderer, number7TextureText, NULL, &number_rect);
         break;
      case 8:
         SDL_RenderCopy(renderer, number8TextureText, NULL, &number_rect);
         break;
      case 9:
         SDL_RenderCopy(renderer, number9TextureText, NULL, &number_rect);
         break;
      }

      // Update the rendering position for the next digit
      xpos -= 30;
      temp /= 10;
   }
}


void render_ui() {

   draw_number(ceil(ball.dashCd), 900);
   draw_number(ceil(fps), 600);
   draw_number(score, 300);

}

void render()
{  
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);
   render_player();
   render_enemies();
   render_ui();
   SDL_RenderPresent(renderer);
};

void pause_render()
{
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
   SDL_RenderClear(renderer);
   SDL_RenderPresent(renderer);
}

void destroy_window()
{
   cleanupEnemyList(&enemyList);

   // SDL_FreeSurface(image);
   // SDL_DestroyTexture(ourPNG);

   SDL_DestroyTexture(number0TextureText);
   SDL_DestroyTexture(number1TextureText);
   SDL_DestroyTexture(number2TextureText);
   SDL_DestroyTexture(number3TextureText);
   SDL_DestroyTexture(number4TextureText);
   SDL_DestroyTexture(number5TextureText);
   SDL_DestroyTexture(number6TextureText);
   SDL_DestroyTexture(number7TextureText);
   SDL_DestroyTexture(number8TextureText);
   SDL_DestroyTexture(number9TextureText);
   SDL_DestroyTexture(textureText);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   TTF_CloseFont(ourFont);
   TTF_Quit();
   SDL_Quit();
}
int main(int argc, char *argv[])
{
   game_is_running = initialize_window();

   setup();

   while (game_is_running)
   {
      if (game_is_pause)
      {
         pause_process_input();
         render();
      }
      else
      {
         process_input();
         update();
         render();
      }
   }

   destroy_window();

   return 0;
}