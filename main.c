// C headers
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

// SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Definitions
#define WINDOW_WIDTH        960
#define WINDOW_HEIGHT       640
#define TILE_SIZE           40
#define MAX_SNAKE_LENGTH    (WINDOW_WIDTH/TILE_SIZE)*(WINDOW_HEIGHT/TILE_SIZE) 

typedef enum 
{ 
    UP, 
    DOWN, 
    LEFT,
    RIGHT 
} dir;

// Main function
int main(int argc, char *argv[])
{
    // Initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        // SDL did not initialise
        printf("Error initialising SDL : %s\n", SDL_GetError());

        return 1;
    }

    // Create window
    SDL_Window * win = SDL_CreateWindow("Snake",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH,
                                        WINDOW_HEIGHT,
                                        0);

    if(NULL == win)
    {
        // Window was not created successfully
        printf("Error creating window : %s\n", SDL_GetError());

        // Clean up resources before exiting
        SDL_Quit();

        return 1;
    }

    // Create renderer
    SDL_Renderer * rend = SDL_CreateRenderer(win,
                                             -1, 
                                             SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(NULL == rend)
    {
      // Renderer was not created successfully
      printf("Error creating renderer: %s\n", SDL_GetError());
      
      // Clean up resources before exiting
      SDL_DestroyWindow(win);
      SDL_Quit();

      return 1;
    }

    // Load images into main memory
    SDL_Surface * surfSnakeHead = IMG_Load("resources/snakeHead.png");
    SDL_Surface * surfSnakeBody = IMG_Load("resources/snakeBody.png");
    SDL_Surface * surfFood = IMG_Load("resources/food.png");
    
    // May need to improve this.
    // Not sure which one will have failed if it does fail.
    // Therefore won't know which ones need cleaned up.
    if((NULL == surfSnakeHead) || (NULL == surfSnakeBody) || (NULL == surfFood))
    {
        // Image was not loaded successfully
        printf("Error creating surface\n");

        // Clean up resources before exiting
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        
        return 1;
    }

    // Load image data into graphics hardware memory
    SDL_Texture* texSnakeHead = SDL_CreateTextureFromSurface(rend, 
                                                             surfSnakeHead);
    SDL_Texture* texSnakeBody = SDL_CreateTextureFromSurface(rend, 
                                                             surfSnakeBody);
    SDL_Texture* texFood = SDL_CreateTextureFromSurface(rend, 
                                                        surfFood);
    
    // May need to improve this.
    // Not sure which one will have failed if it does fail.
    // Therefore won't know which ones need cleaned up.
    if((NULL == texSnakeHead) || (NULL == texSnakeBody) || (NULL == texFood))
    {
        // Image was not loaded successfully
        printf("Error creating texture: %s\n", SDL_GetError());

        // Clean up resources before exiting
        SDL_FreeSurface(surfSnakeBody);
        SDL_FreeSurface(surfSnakeHead);
        SDL_FreeSurface(surfFood);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();

        return 1;
    }

    // Array to hold position and dimensions of snake body segments
    SDL_Rect snake[MAX_SNAKE_LENGTH];

    // Initialise snake length
    int len = 1;

    // Initialise position and dimensions of snake head and body parts  
    for(int i = 0; i < MAX_SNAKE_LENGTH; i++)
    {
        snake[i].w = TILE_SIZE;
        snake[i].h = TILE_SIZE;
        snake[i].x = 0;
        snake[i].y = 0;
    }

    // Initialise snake direction
    dir d = DOWN;

    // Struct to hold position and dimensions of food
    SDL_Rect food;

    // Seed random number generator for food position
    srand(time(NULL));
    
    // Initialise position and dimensions of food
    food.w = TILE_SIZE;
    food.h = TILE_SIZE;
    // food.x = WINDOW_WIDTH/2;
    // food.y = WINDOW_HEIGHT/2;
    food.x = (rand() % (WINDOW_WIDTH/TILE_SIZE)) * TILE_SIZE;
    food.y = (rand() % (WINDOW_HEIGHT/TILE_SIZE)) * TILE_SIZE;

    // Initialise flags to indicate game state
    bool closedWindow = false;
    bool collisionDetected = false;
    bool gamePaused = false;

    // Game loop
    while (!closedWindow)
    {
        SDL_Event event;
        bool keyRegistered = false;

        // Process events
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT: 
                    // Window as been closed by user
                    closedWindow = true;
                    break;

                case SDL_KEYDOWN:
                    // Key has been pressed
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_W:
                            if((DOWN != d) && (!keyRegistered))
                            {
                                d = UP;
                                keyRegistered = true;
                            }
                            break;

                        case SDL_SCANCODE_DOWN:
                        case SDL_SCANCODE_S:
                            if((UP != d) && ((!keyRegistered)))
                            {
                                d = DOWN;
                                keyRegistered = true;
                            }
                            break;

                        case SDL_SCANCODE_LEFT:
                        case SDL_SCANCODE_A:
                            if((RIGHT != d) && (!keyRegistered))
                            {
                                d = LEFT;
                                keyRegistered = true;
                            }
                            break;

                        case SDL_SCANCODE_RIGHT:
                        case SDL_SCANCODE_D:
                            if((LEFT != d) && (!keyRegistered))
                            {
                                d = RIGHT;
                                keyRegistered = true;
                            }
                            break;

                        case SDL_SCANCODE_P:
                            gamePaused = !gamePaused;
                            break;
                        
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        // Collision detection with wall
        if (snake[0].x < 0)
        {
            collisionDetected = true;
        }
        else if (snake[0].x >= WINDOW_WIDTH)
        {
            collisionDetected = true;
        }
        if (snake[0].y < 0)
        {
            collisionDetected = true;
        }
        else if (snake[0].y >= WINDOW_HEIGHT)
        {
            collisionDetected = true;
        }

        // Collision detection with body
        for(int i = 1; i < len; i++)
        {
            if((snake[0].x == snake[i].x) && (snake[0].y == snake[i].y))
            {
                collisionDetected = true;
            }
        }
        
        // Food detection
        if((snake[0].x == food.x) && (snake[0].y == food.y))
        {   
            // Increase length of snake
            len++;

            // Reposition food
            bool foodCollision = true;

            while(foodCollision)
            {
                // Generate new food position
                food.x = (rand() % (WINDOW_WIDTH/TILE_SIZE)) * TILE_SIZE;
                food.y = (rand() % (WINDOW_HEIGHT/TILE_SIZE)) * TILE_SIZE;

                // Check if new position collides with snake
                foodCollision = false;
                for(int i = 0; i < len; i++)
                {
                    if((food.x == snake[i].x) && (food.y == snake[i].y))
                    {
                        // New position collides with snake
                        foodCollision = true;
                    }
                }
            }
        }

        if((!collisionDetected) && (!gamePaused))
        {
            // Update body positions
            for(int i = len - 1; i > 0; i--)
            {
                snake[i].x = snake[i-1].x;
                snake[i].y = snake[i-1].y;
            }
            
            // Update head position
            switch(d)
            {
                case UP:
                    snake[0].y -= TILE_SIZE;
                    break;

                case DOWN:
                    snake[0].y += TILE_SIZE;
                    break;

                case LEFT:
                    snake[0].x -= TILE_SIZE;
                    break;

                case RIGHT:
                    snake[0].x += TILE_SIZE;
                    break;

                default:
                    printf("Error updating position. Invalid direction assigned.\n");
                    break;
            }
        }
    
        // Clear the window
        SDL_RenderClear(rend);
        
        // Draw the image to the window

        SDL_RenderCopy(rend, texSnakeHead, NULL, &snake[0]);
        for(int i = 1; i < len; i++)
        {
            SDL_RenderCopy(rend, texSnakeBody, NULL, &snake[i]);
        }
        SDL_RenderCopy(rend, texFood, NULL, &food);
        
        // Present updated window
        SDL_RenderPresent(rend);

        // Wait 100 milliseconds (10 frames per second)
        SDL_Delay(100);
    }

    // Clean up resources before exiting
    SDL_DestroyTexture(texFood);
    SDL_DestroyTexture(texSnakeBody);
    SDL_DestroyTexture(texSnakeHead);
    SDL_FreeSurface(surfFood);
    SDL_FreeSurface(surfSnakeBody);
    SDL_FreeSurface(surfSnakeHead);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}