/*******************************************************************************************
*
*   MFGD_02_11and12 Jump, Gravity and Smooth Movement
*
*   This program was made by Marc Montagut - @MarcMDE
*
*   It is a practical implementation of the 11 and 12 "Math for Game Developers" videos
*   created by Jorge Rodriguez (https://www.youtube.com/playlist?list=PLW3Zl3wyJwWOpdhYedlD-yCB7WQoHf-My)
*   
*   Copyright (c) 2016 Marc Montagut
*
*   This software is provided 'as-is', without any express or implied
*   warranty. In no event will the authors be held liable for any damages
*   arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose,
*   including commercial applications, and to alter it and redistribute it
*   freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not
*      claim that you wrote the original software. If you use this software
*      in a product, an acknowledgement in the product documentation would be
*      appreciated but is not required.
*   2. Altered source versions must be plainly marked as such, and must not be
*      misrepresented as being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*   
*   This program has been created using raylib 1.3.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
********************************************************************************************/

#include "raylib.h"
#include <math.h>

// I am going to create a small "useful" arithmetic operations C library.
float FlLerp(float current, float target, float change);
float V2Distance(Vector2 a, Vector2 b);
float V2DotProduct(Vector2 a, Vector2 b);
Vector2 V2Normalize(Vector2 a);
Vector2 V2Mult(Vector2 a, Vector2 b);
Vector2 V2IntMult(Vector2 a, int b);
Vector2 V2Div(Vector2 a, Vector2 b);
Vector2 V2FloatDiv(Vector2 a, float b);
Vector2 V2Sum (Vector2 a, Vector2 b);
Vector2 V2Sub (Vector2 a, Vector2 b);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    float playerRadius = 20;
    Vector2 playerPosition = {50, screenHeight-playerRadius};
    Vector2 playerDirection = {0, 0};
    Vector2 playerSpeed = {5, 5};
    Vector2 playerVelocity = {0, 0};
    // Set acceleration duration in 1.5 secs (90frames) 
    float playerAcceleration = playerSpeed.x/90; 
    int playerGravityCounter = 0;
    bool playerIsJumping = 0;
    
    Vector2 gravity = {0, -1.5f};
    
    // Map bounds.
    Rectangle bounds = {0, 0, screenWidth, screenHeight};

    InitWindow(screenWidth, screenHeight, "MFGD_02_11and12_SmoothMovement_Jump - @MarcMDE");
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        
        // Player direction.
        if (IsKeyDown('A')) playerDirection.x = -1;
        else if (IsKeyDown('D')) playerDirection.x = 1;
        else 
        {
            playerDirection.x = 0;
            
            // Set smooth movement variable
            playerVelocity.x = 0;
        }
        
        if (IsKeyDown(KEY_SPACE)&&!playerIsJumping)
        {
            playerDirection.y = -1;
            
            // Set jump variables
            playerIsJumping = 1;
            playerVelocity.y = playerSpeed.y;
        }
        
        // Player movement
        playerPosition.x += playerDirection.x*playerVelocity.x;
        playerVelocity.x = FlLerp(playerVelocity.x, playerSpeed.x, playerAcceleration);
        
        // Player-map collision.
        if (playerPosition.x-playerRadius<=bounds.x) playerPosition.x = bounds.x+playerRadius;
        else if (playerPosition.x+playerRadius>=bounds.width) playerPosition.x = bounds.width-playerRadius;

        if (playerPosition.y-playerRadius<=bounds.y) playerPosition.y = bounds.y+playerRadius;
        else if (playerPosition.y+playerRadius>=bounds.height) playerPosition.y = bounds.height-playerRadius;   
        
        // If player is on the air (jumping)
        if (playerIsJumping) 
        {
            playerPosition.y += playerDirection.y*playerVelocity.y;
            playerVelocity.y+=gravity.y*(float)playerGravityCounter/60;
            playerGravityCounter++;
        }
        
        // When player grounds
        if (playerIsJumping&&playerPosition.y+playerRadius>=bounds.height) 
        {
            playerIsJumping = false;
            playerVelocity.y = 0;
            playerDirection.y = 0;
            playerGravityCounter = 0;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // Draw Player
            DrawCircleV(playerPosition, playerRadius, YELLOW); 
            
            // Draw instructions.
            DrawText("MOVE THE PLAYER WITH W,A,S,D", 10, 5, 20, LIGHTGRAY);
            DrawText("JUMP WITH SPACE", 10, 25, 20, LIGHTGRAY);
            
            // Draw Exit
            DrawText("Esc - EXIT", screenWidth-125, 5, 20, LIGHTGRAY);
            
            // Draw MarcMDE
            DrawText("Created by @MarcMDE", 10, 50, 20, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Lerp btw 2 float values
float FlLerp(float current, float target, float change)
{
    if (target-current>change) 
    {
        if (current+change<target) return current+change;
    }
    if (target-current<change) 
    {
        if (current-change>target) return current-change;
    }
    return target;
}

// Returns distance between Vectors2
float V2Distance(Vector2 a, Vector2 b)
{
    return fabs(sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)));
}

// Returns DotProduct
float V2DotProduct(Vector2 a, Vector2 b)
{
    return a.x*b.x+a.y*b.y;
}

// Returns a normalized Vector2
Vector2 V2Normalize(Vector2 a)
{
    return V2FloatDiv(a, sqrt((a.x*a.x)+(a.y*a.y)));
}

// Returns Vector2*Vector2
Vector2 V2Mult(Vector2 a, Vector2 b)
{
    return (Vector2){a.x*b.x, a.y*b.y};
}

// Returns Vector2*Int
Vector2 V2IntMult(Vector2 a, int b)
{
    return (Vector2){a.x*b, a.y*b};
}

// Returns Vector2/Vector2
Vector2 V2Div(Vector2 a, Vector2 b)
{
    return (Vector2){a.x/b.x, a.y/b.y};
}

// Returns Vector2/float
Vector2 V2FloatDiv(Vector2 a, float b)
{
    return (Vector2){a.x/b, a.y/b};
}

// Returns Vector2+Vector2
Vector2 V2Sum (Vector2 a, Vector2 b)
{
    return (Vector2){a.x+b.x, a.y+b.y};
}

// Returns Vector2-Vector2
Vector2 V2Sub (Vector2 a, Vector2 b)
{
    return (Vector2){a.x-b.x, a.y-b.y};
}