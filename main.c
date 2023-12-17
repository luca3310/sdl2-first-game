#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./constants.h"
#include <time.h>

int counter = 0;

int game_is_running = FALSE;
int game_is_pause = FALSE;
int game_is_over = FALSE;
int game_is_menu = TRUE;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_DisplayMode displayMode;

TTF_Font *ourFont;
SDL_Surface *surfaceText = NULL;
SDL_Texture *textureText = NULL;

SDL_Surface *imgSurface = NULL;

float fps;

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

SDL_Texture *scoreTextureText = NULL;
SDL_Texture *fpsTextureText = NULL;

SDL_Texture *continueTextureBtn = NULL;
SDL_Texture *quitTextureBtn = NULL;
SDL_Texture *tryAgainTextureBtn = NULL;
SDL_Texture *menuTextureBtn = NULL;
SDL_Texture *startTextureBtn = NULL;

SDL_Texture *playerUpSprite = NULL;
SDL_Texture *playerUpSprite2 = NULL;
SDL_Texture *playerUpSprite3 = NULL;

SDL_Texture *playerDownSprite = NULL;
SDL_Texture *playerDownSprite2 = NULL;
SDL_Texture *playerDownSprite3 = NULL;

SDL_Texture *playerRightSprite = NULL;
SDL_Texture *playerRightSprite2 = NULL;
SDL_Texture *playerRightSprite3 = NULL;

SDL_Texture *playerLeftSprite = NULL;
SDL_Texture *playerLeftSprite2 = NULL;
SDL_Texture *playerLeftSprite3 = NULL;

SDL_Texture *playerUpGhostSprite = NULL;
SDL_Texture *playerDownGhostSprite = NULL;
SDL_Texture *playerRightGhostSprite = NULL;
SDL_Texture *playerLeftGhostSprite = NULL;

SDL_Texture *turretBottomSprite = NULL;
SDL_Texture *turretTopSprite = NULL;
SDL_Texture *turretTop2Sprite = NULL;
SDL_Texture *turretTop3Sprite = NULL;
SDL_Texture *turretTop4Sprite = NULL;

SDL_Texture *turretBottomHitSprite = NULL;
SDL_Texture *turretTopHitSprite = NULL;
SDL_Texture *turretTopHit2Sprite = NULL;
SDL_Texture *turretTopHit3Sprite = NULL;
SDL_Texture *turretTopHit4Sprite = NULL;

SDL_Texture *turretBullet = NULL;

SDL_Texture *bulletSprite = NULL;
SDL_Texture *bullet2Sprite = NULL;

int last_frame_time = 0;
int score = 0;
double hitFrame = 1;

SDL_bool isContinueBtnHovered;
SDL_bool isQuitBtnHovered;
SDL_bool isMenuBtnHovered;
SDL_bool isStartBtnHovered;

struct player
{
   float x;
   float y;
   float width;
   float height;
   int left;
   int up;
   int down;
   int right;
   char *playerDir;
   double dashCd;
   double dashSpeed;
   double attackImmune;
   int health;
   double animationFrame;
   double dashGhostCd;
   float dashGhost1X;
   float dashGhost1y;
   char *dashGhost1Dir;
   int dashGhost1Active;
   float dashGhost2X;
   float dashGhost2y;
   char *dashGhost2Dir;
   int dashGhost2Active;
} player;

struct orb
{
   float x;
   float y;
   float angle;
   float distance;
} orb;

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
   float x;
   float y;
   float width;
   float height;
   double Dir;
};

struct EnemyBulletList
{
   struct EnemyBullet *enemyBullets;
   int size;
} enemyBulletList;

void initEnemyBulletList(struct EnemyBulletList *list)
{
   list->enemyBullets = NULL;
   list->size = 0;
}
void addEnemyBullet(struct EnemyBulletList *list, struct EnemyBullet newEnemyBullet)
{
   list->enemyBullets = realloc(list->enemyBullets, (list->size + 1) * sizeof(struct EnemyBullet));
   if (list->enemyBullets != NULL)
   {
      list->enemyBullets[list->size] = newEnemyBullet;
      list->size++;
   }
   else
   {
      printf("Error: Memory allocation failed at enemyBulletList\n");
   }
}
void removeEnemyBullet(struct EnemyBulletList *list, int index)
{
   if (index >= 0 && index < list->size)
   {
      for (int i = index; i < list->size - 1; i++)
      {
         list->enemyBullets[i] = list->enemyBullets[i + 1];
      }
      list->size--;

      // Resize the allocated memory
      list->enemyBullets = realloc(list->enemyBullets, list->size * sizeof(struct EnemyBullet));
   }
   else
   {
      printf("Error: Invalid index at removeEnemyBullet\n");
   }
}

void cleanupEnemyBulletList(struct EnemyBulletList *list)
{
   free(list->enemyBullets);
   list->enemyBullets = NULL;
   list->size = 0;
}

struct Enemy
{
   float x;
   float y;
   float width;
   float height;
   float bulletCd;
   double attackImmune;
   int health;
   double attackCd;
   int maxAttackCd;
   double shootingAnimation;
   double dir;
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
      printf("Error: Memory allocation failed at enemyList\n");
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
      printf("Error: Invalid index at removeEnemy\n");
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
         game_is_pause = TRUE;
         break;
      case SDLK_w:
         player.up = 1;
         break;
      case SDLK_s:
         player.down = 1;
         break;
      case SDLK_a:
         player.left = 1;
         break;
      case SDLK_d:
         player.right = 1;
         break;
      case SDLK_SPACE:
         if (player.dashCd == 0)
         {
            player.dashSpeed = 1000;
            player.dashCd = 2;
            player.dashGhostCd = 20;
         }
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
            bullet.dir = atan2(y - player.y, x - player.x);
            bullet.prevDir = atan2(y - player.y, x - player.x);
            bullet.x = player.x;
            bullet.y = player.y;
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
         player.up = 0;
         break;
      case SDLK_s:
         player.down = 0;
         break;
      case SDLK_a:
         player.left = 0;
         break;
      case SDLK_d:
         player.right = 0;
         break;
         ;
      }
      break;
   };
};

SDL_bool is_mouse_over_button(int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight)
{
   return mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
          mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

int collision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
   return x1 < x2 + width2 && x1 + width1 > x2 &&
          y1 < y2 + height2 && y1 + height1 > y2;
}

void player_setup()
{
   player.x = 20;
   player.y = 20;
   player.width = 50;
   player.height = 60;
   player.up = 0;
   player.down = 0;
   player.left = 0;
   player.right = 0;
   player.dashCd = 0;
   player.dashSpeed = 0;
   player.health = 3;
   player.attackImmune = 0;
   player.animationFrame = 3;

   bullet.isFired = 0;
   bullet.width = 25;
   bullet.height = 25;
   bullet.speedUp = 0;

   orb.angle = 0;
   orb.distance = 70;

   player.dashGhostCd = 0;
   player.dashGhost2Active = 0;
   player.dashGhost2Active = 0;
}

void enemyBullet_setup()
{
   initEnemyBulletList(&enemyBulletList);
}

void enemy_setup()
{
   spawner.maxSpawnCooldown = 7;
   spawner.currentSpawnCooldown = 1;
   spawner.spawnAmount = 1;
   spawner.spawnAmountIncreaseCooldown = 200;
   spawner.maxSpawnAmountIncreaseCooldown = 200;

   initEnemyList(&enemyList);
}

void restart_setup()
{
   cleanupEnemyBulletList(&enemyBulletList);
   cleanupEnemyList(&enemyList);
   score = 0;
   enemyBullet_setup();
   player_setup();
   enemy_setup();
   srand(time(NULL));
}

void game_over_process_input()
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
         game_is_pause = FALSE;
         break;
         break;
      }
   case SDL_MOUSEMOTION:
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      isContinueBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2 - 200, 300, 100);
      isMenuBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2, 300, 100);
      isQuitBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2 + 200, 300, 100);
      break;
   case SDL_MOUSEBUTTONDOWN:
      switch (pause_event.button.button)
      {
      case SDL_BUTTON_LEFT:
         if (isContinueBtnHovered)
         {
            restart_setup();
            game_is_over = FALSE;
         }
         if (isMenuBtnHovered)
         {
            restart_setup();
            game_is_over = FALSE;
            game_is_menu = TRUE;
         }
         if (isQuitBtnHovered)
         {
            game_is_running = FALSE;
         }
      }
   }
}

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
         game_is_pause = FALSE;
         break;
         break;
      }
   case SDL_MOUSEMOTION:
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      isContinueBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2 - 200, 300, 100);
      isMenuBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2, 300, 100);
      isQuitBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2 + 200, 300, 100);
      break;
   case SDL_MOUSEBUTTONDOWN:
      switch (pause_event.button.button)
      {
      case SDL_BUTTON_LEFT:
         if (isContinueBtnHovered)
         {
            game_is_pause = FALSE;
         }
         if (isMenuBtnHovered)
         {
            restart_setup();
            game_is_pause = FALSE;
            game_is_menu = TRUE;
         }
         if (isQuitBtnHovered)
         {
            game_is_running = FALSE;
         }
      }
   }
}

void menu_process_input()
{
   SDL_Event pause_event;
   SDL_PollEvent(&pause_event);
   switch (pause_event.type)
   {
   case SDL_QUIT:
      game_is_running = FALSE;
      break;
   case SDL_MOUSEMOTION:
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      isStartBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2 - 100, 300, 100);
      isQuitBtnHovered = is_mouse_over_button(mouseX, mouseY, displayMode.w / 2 - 150, displayMode.h / 2 + 100, 300, 100);
      break;
   case SDL_MOUSEBUTTONDOWN:
      switch (pause_event.button.button)
      {
      case SDL_BUTTON_LEFT:
         if (isStartBtnHovered)
         {
            game_is_menu = FALSE;
         }
         if (isQuitBtnHovered)
         {
            game_is_running = FALSE;
         }
      }
   }
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

   surfaceText = TTF_RenderText_Solid(ourFont, "Score: ", white);
   scoreTextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "Fps: ", white);
   fpsTextureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "Continue", white);
   continueTextureBtn = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "Quit", white);
   quitTextureBtn = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "Try Again", white);
   tryAgainTextureBtn = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "Menu", white);
   menuTextureBtn = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   surfaceText = TTF_RenderText_Solid(ourFont, "Start", white);
   startTextureBtn = SDL_CreateTextureFromSurface(renderer, surfaceText);
   SDL_FreeSurface(surfaceText);

   imgSurface = SDL_LoadBMP("./images/playerDown.bmp");
   playerDownSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerDown2.bmp");
   playerDownSprite2 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerDown3.bmp");
   playerDownSprite3 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerUp.bmp");
   playerUpSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerUp2.bmp");
   playerUpSprite2 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerUp3.bmp");
   playerUpSprite3 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerRight.bmp");
   playerRightSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerRight2.bmp");
   playerRightSprite2 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerRight3.bmp");
   playerRightSprite3 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerLeft.bmp");
   playerLeftSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerLeft2.bmp");
   playerLeftSprite2 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerLeft3.bmp");
   playerLeftSprite3 = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerUpGhost.bmp");
   playerUpGhostSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerDownGhost.bmp");
   playerDownGhostSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerLeftGhost.bmp");
   playerLeftGhostSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/playerRightGhost.bmp");
   playerRightGhostSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/bullet.bmp");
   bulletSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/bullet2.bmp");
   bullet2Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretBottom.bmp");
   turretBottomSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTop.bmp");
   turretTopSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTop2.bmp");
   turretTop2Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTop3.bmp");
   turretTop3Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTop4.bmp");
   turretTop4Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretBottomHit.bmp");
   turretBottomHitSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTopHit.bmp");
   turretTopHitSprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTop2Hit.bmp");
   turretTopHit2Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTopHit3.bmp");
   turretTopHit3Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretTop4Hit.bmp");
   turretTopHit4Sprite = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);

   imgSurface = SDL_LoadBMP("./images/turretBullet.bmp");
   turretBullet = SDL_CreateTextureFromSurface(renderer, imgSurface);
   SDL_FreeSurface(imgSurface);
}
void setup()
{
   player_setup();
   enemyBullet_setup();
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

void cooldown_decrease(double *cd, double delta_time, int multiplier)
{
   if (*cd > 0)
   {
      *cd -= delta_time * multiplier;
      if (*cd < 0)
      {
         *cd = 0;
      }
   }
}

void cap_frame_rate(Uint64 starting_tick, int max_fps)
{
   Uint64 target_ticks = starting_tick + SDL_GetPerformanceFrequency() / max_fps;

   Uint64 current_tick = SDL_GetPerformanceCounter();
   if (current_tick < target_ticks)
   {
      Uint64 delay_ticks = target_ticks - current_tick;
      Uint32 delay_ms = (delay_ticks * 1000) / SDL_GetPerformanceFrequency();
      SDL_Delay(delay_ms);
   }
}

double setup_update(int max_fps)
{
   static Uint64 last_counter = 0;
   Uint64 counter = SDL_GetPerformanceCounter();

   double delta_time = (double)(counter - last_counter) / SDL_GetPerformanceFrequency();
   last_counter = counter;

   cap_frame_rate(counter, max_fps);

   return delta_time;
}

void calculate_fps(double delta_time)
{
   static double accumulated_time = 0.0;
   static int frames = 0;

   accumulated_time += delta_time;
   frames++;

   if (accumulated_time >= 1.0)
   {
      fps = frames / accumulated_time;
      accumulated_time = 0.0;
      frames = 0;
   }
}

void enemyBullet_update(SDL_Rect player_rect, double delta_time)
{
   for (int i = 0; i < enemyBulletList.size; ++i)
   {
      enemyBulletList.enemyBullets[i].x += cos(enemyBulletList.enemyBullets[i].Dir) * 300 * delta_time;
      enemyBulletList.enemyBullets[i].y += sin(enemyBulletList.enemyBullets[i].Dir) * 300 * delta_time;

      SDL_Rect enemyBullet_rect = {enemyBulletList.enemyBullets[i].x, enemyBulletList.enemyBullets[i].y, enemyBulletList.enemyBullets[i].width, enemyBulletList.enemyBullets[i].height};
      if (check_collision(player_rect, enemyBullet_rect) && player.dashSpeed == 0 && player.attackImmune == 0)
      {
         player.attackImmune = 2;
         player.health -= 1;
         if (player.health == 0)
         {
            game_is_over = TRUE;
         }
      }

      if (enemyBulletList.enemyBullets[i].x < 0 || enemyBulletList.enemyBullets[i].x > displayMode.w || enemyBulletList.enemyBullets[i].y < 0 || enemyBulletList.enemyBullets[i].y > displayMode.h)
      {
         removeEnemyBullet(&enemyBulletList, i);
      }
   }
}

void enemy_update(SDL_Rect player_rect, double delta_time)
{
   for (int i = 0; i < enemyList.size; ++i)
   {
      cooldown_decrease(&enemyList.enemies[i].attackImmune, delta_time, 1);
      cooldown_decrease(&enemyList.enemies[i].attackCd, delta_time, 1);
      enemyList.enemies[i].dir = atan2(player.y - enemyList.enemies[i].y, player.x - enemyList.enemies[i].x) * 180.0 / M_PI;

      if (enemyList.enemies[i].shootingAnimation > 0)
      {
         cooldown_decrease(&enemyList.enemies[i].shootingAnimation, delta_time, 10);
      }

      if (enemyList.enemies[i].attackCd == 0)
      {
         enemyList.enemies[i].shootingAnimation = 4;
         enemyList.enemies[i].attackCd = enemyList.enemies[i].maxAttackCd;
         struct EnemyBullet enemyBulletInstance = {enemyList.enemies[i].x + 15, enemyList.enemies[i].y + 15, 20, 20, atan2(player.y - enemyList.enemies[i].y, player.x - enemyList.enemies[i].x)};
         addEnemyBullet(&enemyBulletList, enemyBulletInstance);
      }
   }
}

void bullet_update(SDL_Rect player_rect, double delta_time)
{
   if (bullet.isFired == 1 || bullet.isFired == 2)
   {
      float bullet_speed = 400.0f + bullet.speedUp;
      SDL_Rect bullet_rect = {(int)bullet.x, (int)bullet.y, (int)bullet.width, (int)bullet.height};

      if (bullet.isFired == 2)
      {
         bullet_speed = 700.0f + bullet.speedUp;
         bullet.dir = atan2(player.y - bullet.y, player.x - bullet.x);
         if (check_collision(bullet_rect, player_rect))
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
            if (enemyList.enemies[i].attackImmune == 0)
            {
               if (enemyList.enemies[i].health > 1)
               {
                  enemyList.enemies[i].attackImmune = 0.5;
                  enemyList.enemies[i].health -= 1;
               }
               else
               {
                  removeEnemy(&enemyList, i);
                  score += 1;
               }
            }
         }
      }

      if (bullet.x < 0 || bullet.x > displayMode.w || bullet.y < 0 || bullet.y > displayMode.h)
      {
         bullet.isFired = 2;
      }
   }
}

void update_spawner()
{
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
         float spawnX, spawnY;

         do
         {
            spawnX = random_float(0, displayMode.w - 15);
            spawnY = random_float(0, displayMode.h - 15);
         } while (collision(spawnX, spawnY, 15, 15, player.x - 350, player.y - 250, 700, 500));
         struct Enemy enemyInstance = {spawnX, spawnY, 60, 60, 2.0, 0, 2, 2, 5, 0};
         addEnemy(&enemyList, enemyInstance);
      }
   }
}

void player_movement(double delta_time)
{
   int player_speed = 200 + player.dashSpeed;

   if (player.up && player.y > 0)
   {
      player.y -= player_speed * delta_time;
   }
   if (player.down && player.y < displayMode.h - player.height)
   {
      player.y += player_speed * delta_time;
   }
   if (player.left && player.x > 0)
   {
      player.x -= player_speed * delta_time;
   }
   if (player.right && player.x < displayMode.w - player.width)
   {
      player.x += player_speed * delta_time;
   }

   if (player.x < 0)
   {
      player.x = 0;
   }
   if (player.x > displayMode.w - player.width)
   {
      player.x = displayMode.w - player.width;
   }
   if (player.y < 0)
   {
      player.y = 0;
   }
   if (player.y > displayMode.h - player.height)
   {
      player.y = displayMode.h - player.height;
   }
}

void update()
{
   double delta_time = setup_update(MAX_FPS);

   calculate_fps(delta_time);

   SDL_Rect player_rect = {(int)player.x, (int)player.y, (int)player.width, (int)player.height};
   enemyBullet_update(player_rect, delta_time);
   enemy_update(player_rect, delta_time);
   bullet_update(player_rect, delta_time);
   update_spawner();
   player_movement(delta_time);

   cooldown_decrease(&spawner.currentSpawnCooldown, delta_time, 1);
   cooldown_decrease(&spawner.spawnAmountIncreaseCooldown, delta_time, 1);
   cooldown_decrease(&player.dashCd, delta_time, 1);
   cooldown_decrease(&bullet.speedUp, delta_time, 1300);
   cooldown_decrease(&player.dashSpeed, delta_time, 3000);
   cooldown_decrease(&player.attackImmune, delta_time, 1);

   cooldown_decrease(&player.animationFrame, delta_time, 3);

   if (ceil(player.animationFrame) == 0)
   {
      player.animationFrame = 3;
   }

   // orb.x = (player.x + 25 + random_float(-10, 10)) + orb.distance * cos(orb.angle);
   // orb.y = (player.y + 30 + random_float(-10, 10)) + orb.distance * sin(orb.angle);

   orb.x = (player.x + 25) + orb.distance * cos(orb.angle);
   orb.y = (player.y + 30) + orb.distance * sin(orb.angle);

   orb.angle += 5 * delta_time;

   if (ceil(player.dashGhostCd) == 20 && !player.dashGhost1Active)
   {
      player.dashGhost1X = player.x;
      player.dashGhost1y = player.y;
      player.dashGhost1Active = 1;

      if (player.up)
      {
         player.dashGhost1Dir = "u";
      }
      else if (player.down)
      {
         player.dashGhost1Dir = "d";
      }
      else if (player.left)
      {
         player.dashGhost1Dir = "l";
      }
      else if (player.right)
      {
         player.dashGhost1Dir = "r";
      }
   }
   if (ceil(player.dashGhostCd) == 10 && player.dashGhost1Active)
   {
      player.dashGhost1Active = 0;
   }

   if (ceil(player.dashGhostCd) == 18 && !player.dashGhost2Active)
   {
      player.dashGhost2X = player.x;
      player.dashGhost2y = player.y;
      player.dashGhost2Active = 1;
      if (player.up)
      {
         player.dashGhost2Dir = "u";
      }
      else if (player.down)
      {
         player.dashGhost2Dir = "d";
      }
      else if (player.left)
      {
         player.dashGhost2Dir = "l";
      }
      else if (player.right)
      {
         player.dashGhost2Dir = "r";
      }
   }

   if (ceil(player.dashGhostCd) == 1 && player.dashGhost2Active)
   {
      player.dashGhost2Active = 0;
   }

   cooldown_decrease(&player.dashGhostCd, delta_time, 16);

   cooldown_decrease(&hitFrame, delta_time, 8);
   if (hitFrame == 0)
   {
      hitFrame = 3;
   }
   printf("%f\n", ceil(hitFrame));
};

void game_over_update()
{
   double delta_time = setup_update(GAME_OVER_MAX_FPS);
   calculate_fps(delta_time);
}

void pause_update()
{
   double delta_time = setup_update(GAME_OVER_MAX_FPS);
   calculate_fps(delta_time);
}

void menu_update()
{
   double delta_time = setup_update(GAME_OVER_MAX_FPS);
   calculate_fps(delta_time);
}

void render_player()
{
   SDL_Rect player_rect = {
       (int)player.x,
       (int)player.y,
       (int)player.width,
       (int)player.height};
   if (player.dashSpeed > 0 || player.attackImmune > 0)
   {

      if (player.dashGhost1Active)
      {
         SDL_Rect ghost1_rect = {
             player.dashGhost1X,
             player.dashGhost1y,
             player.width,
             player.height};
         if (strcmp(player.dashGhost1Dir, "u") == 0)
         {
            SDL_RenderCopy(renderer, playerUpGhostSprite, NULL, &ghost1_rect);
         }
         else if (strcmp(player.dashGhost1Dir, "d") == 0)
         {
            SDL_RenderCopy(renderer, playerDownGhostSprite, NULL, &ghost1_rect);
         }
         else if (strcmp(player.dashGhost1Dir, "l") == 0)
         {
            SDL_RenderCopy(renderer, playerLeftGhostSprite, NULL, &ghost1_rect);
         }
         else
         {
            SDL_RenderCopy(renderer, playerRightGhostSprite, NULL, &ghost1_rect);
         }
      }

      if (player.dashGhost2Active)
      {
         SDL_Rect ghost2_rect = {
             player.dashGhost2X,
             player.dashGhost2y,
             player.width,
             player.height};
         if (strcmp(player.dashGhost2Dir, "u") == 0)
         {
            SDL_RenderCopy(renderer, playerUpGhostSprite, NULL, &ghost2_rect);
         }
         else if (strcmp(player.dashGhost2Dir, "d") == 0)
         {
            SDL_RenderCopy(renderer, playerDownGhostSprite, NULL, &ghost2_rect);
         }
         else if (strcmp(player.dashGhost2Dir, "l") == 0)
         {
            SDL_RenderCopy(renderer, playerLeftGhostSprite, NULL, &ghost2_rect);
         }
         else
         {
            SDL_RenderCopy(renderer, playerRightGhostSprite, NULL, &ghost2_rect);
         }
      }

      if (player.up)
      {
         SDL_RenderCopy(renderer, playerUpGhostSprite, NULL, &player_rect);
      }
      else if (player.down)
      {
         SDL_RenderCopy(renderer, playerDownGhostSprite, NULL, &player_rect);
      }
      else if (player.right)
      {
         SDL_RenderCopy(renderer, playerRightGhostSprite, NULL, &player_rect);
      }
      else if (player.left)
      {
         SDL_RenderCopy(renderer, playerLeftGhostSprite, NULL, &player_rect);
      }
      else
      {
         SDL_RenderCopy(renderer, playerDownGhostSprite, NULL, &player_rect);
      }
   }
   else
   {
      if (player.up)
      {
         if (ceil(player.animationFrame) == 3)
         {
            SDL_RenderCopy(renderer, playerUpSprite2, NULL, &player_rect);
         }
         else if (ceil(player.animationFrame) == 2)
         {
            SDL_RenderCopy(renderer, playerUpSprite, NULL, &player_rect);
         }
         else
         {
            SDL_RenderCopy(renderer, playerUpSprite3, NULL, &player_rect);
         }
      }
      else if (player.down)
      {
         if (ceil(player.animationFrame) == 3)
         {
            SDL_RenderCopy(renderer, playerDownSprite2, NULL, &player_rect);
         }
         else if (ceil(player.animationFrame) == 2)
         {
            SDL_RenderCopy(renderer, playerDownSprite, NULL, &player_rect);
         }
         else
         {
            SDL_RenderCopy(renderer, playerDownSprite3, NULL, &player_rect);
         }
      }
      else if (player.right)
      {
         if (ceil(player.animationFrame) == 3)
         {
            SDL_RenderCopy(renderer, playerRightSprite2, NULL, &player_rect);
         }
         else if (ceil(player.animationFrame) == 2)
         {
            SDL_RenderCopy(renderer, playerRightSprite, NULL, &player_rect);
         }
         else
         {
            SDL_RenderCopy(renderer, playerRightSprite3, NULL, &player_rect);
         }
      }
      else if (player.left)
      {
         if (ceil(player.animationFrame) == 3)
         {
            SDL_RenderCopy(renderer, playerLeftSprite2, NULL, &player_rect);
         }
         else if (ceil(player.animationFrame) == 2)
         {
            SDL_RenderCopy(renderer, playerLeftSprite, NULL, &player_rect);
         }
         else
         {
            SDL_RenderCopy(renderer, playerLeftSprite3, NULL, &player_rect);
         }
      }
      else
      {
         SDL_RenderCopy(renderer, playerDownSprite, NULL, &player_rect);
      }
   }
   if (bullet.isFired == 1)
   {
      SDL_Rect bullet_rect = {
          (int)bullet.x,
          (int)bullet.y,
          (int)bullet.width,
          (int)bullet.height};
      SDL_RenderCopy(renderer, bullet2Sprite, NULL, &bullet_rect);
   }
   else if (bullet.isFired == 2)
   {
      SDL_Rect bullet_rect = {
          (int)bullet.x,
          (int)bullet.y,
          (int)bullet.width,
          (int)bullet.height};
      SDL_RenderCopy(renderer, bulletSprite, NULL, &bullet_rect);
   }
   else
   {

      SDL_Rect bullet_rect = {
          (int)orb.x,
          (int)orb.y,
          (int)bullet.width,
          (int)bullet.height};
      SDL_RenderCopy(renderer, bulletSprite, NULL, &bullet_rect);
   }
}

void render_enemyBullets()
{
   for (int i = 0; i < enemyBulletList.size; i++)
   {
      SDL_Rect enemyBullet_rect = {
          (int)enemyBulletList.enemyBullets[i].x,
          (int)enemyBulletList.enemyBullets[i].y,
          (int)enemyBulletList.enemyBullets[i].width,
          (int)enemyBulletList.enemyBullets[i].height};

      SDL_RenderCopy(renderer, turretBullet, NULL, &enemyBullet_rect);
   }
}

void render_enemies()
{
   for (int i = 0; i < enemyList.size; i++)
   {
      SDL_Rect enemy_rect = {
          (int)enemyList.enemies[i].x,
          (int)enemyList.enemies[i].y,
          (int)enemyList.enemies[i].width,
          (int)enemyList.enemies[i].height};
      SDL_Rect turret_rect = {
          (int)enemyList.enemies[i].x - 20,
          (int)enemyList.enemies[i].y + 10,
          100,
          40};
      // enemyList.enemies[i].attackImmune > 0

      if (ceil(enemyList.enemies[i].shootingAnimation) == 0)
      {
         if (enemyList.enemies[i].attackImmune > 0 && !(ceil(hitFrame) == 2))
         {
            SDL_RenderCopy(renderer, turretBottomHitSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTopHitSprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
         else
         {
            SDL_RenderCopy(renderer, turretBottomSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTopSprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
      }
      else if (ceil(enemyList.enemies[i].shootingAnimation) == 4)
      {
         if (enemyList.enemies[i].attackImmune > 0 && !(ceil(hitFrame) == 2))
         {
            SDL_RenderCopy(renderer, turretBottomHitSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTopHit2Sprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
         else
         {
            SDL_RenderCopy(renderer, turretBottomSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTop2Sprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
      }
      else if (ceil(enemyList.enemies[i].shootingAnimation) == 3 || ceil(enemyList.enemies[i].shootingAnimation) == 2)
      {
         if (enemyList.enemies[i].attackImmune > 0 && !(ceil(hitFrame) == 2))
         {

            SDL_RenderCopy(renderer, turretBottomHitSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTopHit3Sprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
         else
         {
            SDL_RenderCopy(renderer, turretBottomSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTop3Sprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
      }
      else if (ceil(enemyList.enemies[i].shootingAnimation) == 1)
      {
         if (enemyList.enemies[i].attackImmune > 0 && !(ceil(hitFrame) == 2))
         {
            SDL_RenderCopy(renderer, turretBottomHitSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTopHit4Sprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
         else
         {
            SDL_RenderCopy(renderer, turretBottomSprite, NULL, &enemy_rect);
            SDL_RenderCopyEx(renderer, turretTop4Sprite, NULL, &turret_rect, enemyList.enemies[i].dir, NULL, SDL_FLIP_NONE);
         }
      }

      // if (enemyList.enemies[i].attackImmune > 0)
      // {
      //    SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255);
      // }
      // else
      // {
      //    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      // }
   }
}

void draw_number(int number, int xpos, int ypos)
{
   if (number == 0)
   {
      SDL_Rect number_rect = {(int)xpos, (int)ypos, (int)20, (int)40};
      SDL_RenderCopy(renderer, number0TextureText, NULL, &number_rect);
      return;
   }

   int digitCount = 0;
   int temp = number;
   int originalTemp = number;

   while (temp != 0)
   {
      temp /= 10;
      ++digitCount;
   }

   temp = originalTemp;
   int digits[5];

   for (int i = digitCount - 1; i >= 0; --i)
   {
      digits[i] = temp % 10;
      temp /= 10;
   }

   for (int i = 0; i < digitCount; ++i)
   {
      int digit = digits[i];

      SDL_Rect number_rect = {(int)xpos, (int)ypos, (int)20, (int)40};

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
      xpos += 20;
   }
}

void drawText(SDL_Texture *textTexture, int xpos, int ypos)
{
   int textWidth, textHeight;
   SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
   SDL_Rect text_rect = {(int)xpos, (int)ypos, (int)textWidth, (int)textHeight};

   SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);
}

void draw_text_with_number(SDL_Texture *textTexture, int number, int xpos, int ypos)
{
   int textWidth, textHeight;
   SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
   SDL_Rect text_rect = {(int)xpos, (int)ypos, (int)textWidth, (int)textHeight};
   SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);
   xpos += textWidth;

   draw_number(number, xpos, ypos);
}

void draw_hearths(int xpos, int ypos, int amount, int health)
{
   for (int i = 0; i < amount; ++i)
   {
      SDL_Rect hearth_rect = {(int)xpos, (int)ypos, (int)100, (int)100};
      if (health > i)
      {
         SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
         SDL_RenderFillRect(renderer, &hearth_rect);
      }
      else
      {
         SDL_SetRenderDrawColor(renderer, 0, 50, 0, 255);
         SDL_RenderFillRect(renderer, &hearth_rect);
      }
      xpos += 150;
   }
}

void render_ui()
{
   draw_text_with_number(fpsTextureText, ceil(fps), displayMode.w - 200, 30);
   draw_text_with_number(scoreTextureText, score, displayMode.w - 200, 80);
   SDL_Rect dashCdBar = {(int)player.x - 5, (int)player.y + 70, player.dashCd * 20, 2};
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
   SDL_RenderFillRect(renderer, &dashCdBar);
   draw_hearths(30, 30, 3, player.health);
}

void render()
{
   SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
   SDL_RenderClear(renderer);
   render_enemyBullets();
   render_enemies();
   render_player();
   render_ui();

   SDL_RenderPresent(renderer);
};

void draw_button(SDL_Texture *textTexture, int height, int width, int xpos, int ypos, SDL_bool isHovered)
{
   SDL_Rect button_rect = {(int)xpos, (int)ypos, (int)width, (int)height};
   if (isHovered)
   {
      SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
   }
   else
   {
      SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
   }
   SDL_RenderFillRect(renderer, &button_rect);

   int textWidth, textHeight;
   SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

   int textX = xpos + (width - textWidth) / 2;
   int textY = ypos + (height - textHeight) / 2;

   SDL_Rect text_rect = {textX, textY, textWidth, textHeight};
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);
}

void pause_render()
{
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
   SDL_RenderClear(renderer);

   draw_button(continueTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2 - 200, isContinueBtnHovered);
   draw_button(menuTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2, isMenuBtnHovered);
   draw_button(quitTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2 + 200, isQuitBtnHovered);

   draw_text_with_number(fpsTextureText, ceil(fps), displayMode.w - 200, 30);

   SDL_RenderPresent(renderer);
}

void game_over_render()
{
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
   SDL_RenderClear(renderer);

   draw_button(tryAgainTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2 - 200, isContinueBtnHovered);
   draw_button(menuTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2, isMenuBtnHovered);
   draw_button(quitTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2 + 200, isQuitBtnHovered);

   draw_text_with_number(fpsTextureText, ceil(fps), displayMode.w - 200, 30);

   SDL_RenderPresent(renderer);
}

void menu_render()
{
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
   SDL_RenderClear(renderer);

   draw_button(startTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2 - 100, isStartBtnHovered);
   draw_button(quitTextureBtn, 100, 300, displayMode.w / 2 - 150, displayMode.h / 2 + 100, isQuitBtnHovered);

   draw_text_with_number(fpsTextureText, ceil(fps), displayMode.w - 200, 30);
   SDL_RenderPresent(renderer);
}
void destroy_window()
{
   cleanupEnemyBulletList(&enemyBulletList);
   cleanupEnemyList(&enemyList);

   SDL_DestroyTexture(playerDownSprite);
   SDL_DestroyTexture(playerDownSprite2);
   SDL_DestroyTexture(playerDownSprite3);

   SDL_DestroyTexture(playerUpSprite);
   SDL_DestroyTexture(playerUpSprite2);
   SDL_DestroyTexture(playerUpSprite3);

   SDL_DestroyTexture(playerRightSprite);
   SDL_DestroyTexture(playerRightSprite2);
   SDL_DestroyTexture(playerRightSprite3);

   SDL_DestroyTexture(playerLeftSprite);
   SDL_DestroyTexture(playerLeftSprite2);
   SDL_DestroyTexture(playerLeftSprite3);

   SDL_DestroyTexture(playerDownGhostSprite);
   SDL_DestroyTexture(playerUpGhostSprite);
   SDL_DestroyTexture(playerRightGhostSprite);
   SDL_DestroyTexture(playerLeftGhostSprite);

   SDL_DestroyTexture(bulletSprite);
   SDL_DestroyTexture(bullet2Sprite);

   SDL_DestroyTexture(turretBottomSprite);
   SDL_DestroyTexture(turretTopSprite);
   SDL_DestroyTexture(turretTop2Sprite);
   SDL_DestroyTexture(turretTop3Sprite);
   SDL_DestroyTexture(turretTop4Sprite);

   SDL_DestroyTexture(turretBottomHitSprite);
   SDL_DestroyTexture(turretTopHitSprite);

   SDL_DestroyTexture(turretBullet);

   SDL_DestroyTexture(scoreTextureText);
   SDL_DestroyTexture(fpsTextureText);

   SDL_DestroyTexture(continueTextureBtn);
   SDL_DestroyTexture(quitTextureBtn);
   SDL_DestroyTexture(tryAgainTextureBtn);
   SDL_DestroyTexture(menuTextureBtn);
   SDL_DestroyTexture(startTextureBtn);

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
   // main loop
   // game over screen
   // pause screen
   // main menu
   // options
   while (game_is_running)
   {
      if (game_is_menu)
      {
         menu_process_input();
         menu_update();
         menu_render();
      }
      else if (game_is_over)
      {
         game_over_process_input();
         game_over_update();
         game_over_render();
      }
      else if (game_is_pause)
      {
         pause_process_input();
         pause_update();
         pause_render();
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