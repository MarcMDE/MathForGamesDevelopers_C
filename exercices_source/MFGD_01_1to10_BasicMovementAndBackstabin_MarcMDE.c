/*******************************************************************************************
*
*   MFGD_01_1to10 Basic Movement and Backstabing
*
*   This program was made by Marc Montagut - @MarcMDE
*
*   It is a practical implementation of the 1 to 10 "Math for Game Developers" videos
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

// Functions declaration
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
    
    // Player variables.
    float playerRadius = 25;
    Vector2 playerPosition = {50, 140};
    Vector2 playerDirection = {0, 0};
    Vector2 playerSpeed = {3, 3};
    Color playerColor = YELLOW;
    
    // Enemy variables.
    float enemyRadius = 25;
    Vector2 enemyPosition = {enemyRadius, screenHeight/2};
    Vector2 enemyDirection = {1, 0};
    int enemySpeedX = 2;
    Color enemyColorOnIdle = BLUE;
    Color enemyColorOnHit = RED;
    Color enemyColor = enemyColorOnIdle;
    
    // Backstab variables.
    int backstabDistance = playerRadius+enemyRadius+30;
    float backstabFOV = -0.9f; // 1(front) to -1 (behind)
    bool isPlayerOnBackstabRange = 0;
    bool isPlayerOnBackstabPosition = 0;
    bool isEnemyBackstabed = 0;
    int backstabsCounter = 0;
    Color onBackstabPositionLineColor = ORANGE;
    Color onBackstabPositionAndRangeLineColor = GREEN;
    Color outBackstabLineColor = RED;
    Color backstabLineColor = outBackstabLineColor;
    
    // Map bounds.
    Rectangle bounds = {0, 0, screenWidth, screenHeight};
    

    InitWindow(screenWidth, screenHeight, "MFGD_01_1to10 Basic Movement and Backstabing - @MarcMDE");
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
        else playerDirection.x = 0;
        
        if (IsKeyDown('W')) playerDirection.y = -1;
        else if (IsKeyDown('S')) playerDirection.y = 1;
        else playerDirection.y = 0;
        
        // Player movement
        playerPosition = V2Sum(playerPosition, V2Mult(playerDirection, playerSpeed));
        
        // Player-map collision.
        if (playerPosition.x-playerRadius<=bounds.x) playerPosition.x = bounds.x+playerRadius;
        else if (playerPosition.x+playerRadius>=bounds.width) playerPosition.x = bounds.width-playerRadius;
        
        if (playerPosition.y-playerRadius<=bounds.y) playerPosition.y = bounds.y+playerRadius;
        else if (playerPosition.y>=bounds.height) playerPosition.y = bounds.y-playerRadius;
        
        // Backstab logic.
        // Check player-enemy position.
        if (V2DotProduct(V2Normalize(enemyDirection), V2Normalize(V2Sub(playerPosition, enemyPosition)))<backstabFOV) isPlayerOnBackstabPosition = 1;
        else isPlayerOnBackstabPosition = 0;

        // Check player-enemy distance.
        if (V2Distance(playerPosition, enemyPosition)<=backstabDistance) isPlayerOnBackstabRange = 1;
        else isPlayerOnBackstabRange = 0;

        // Set backstab line color.
        if (isPlayerOnBackstabPosition)
        {
            backstabLineColor = onBackstabPositionLineColor;
            if (isPlayerOnBackstabPosition&&isPlayerOnBackstabRange)
            {
                backstabLineColor = onBackstabPositionAndRangeLineColor;
            }
        }
        else backstabLineColor = outBackstabLineColor;
        
        // Check backstab input.
        if (IsKeyPressed(KEY_SPACE)&&isPlayerOnBackstabPosition&&isPlayerOnBackstabRange) 
        {
            isEnemyBackstabed = 1;
            backstabsCounter++;
        }
        else isEnemyBackstabed = 0;
        
        // Set enemy color.
        if (isEnemyBackstabed) enemyColor = enemyColorOnHit;
        else enemyColor = enemyColorOnIdle;
        
        // Enemy direction and map collision.
        if (enemyPosition.x-enemyRadius<=bounds.x) 
        {
            enemyPosition.x = bounds.x+enemyRadius;
            enemyDirection.x = 1;
        }
        else if (enemyPosition.x+enemyRadius>=bounds.width) 
        {
            enemyPosition.x = bounds.width-playerRadius;
            enemyDirection.x = -1;
        }
        
        // Enemy movement.
        if (!isEnemyBackstabed) enemyPosition = V2Sum(enemyPosition, V2IntMult(enemyDirection, enemySpeedX));
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // Draw backstab position and distance checker line
            DrawLineV(playerPosition, enemyPosition, backstabLineColor);
            
            // Draw enemy.
            DrawCircleV(enemyPosition, enemyRadius, enemyColor);
            
            // Draw player.
            DrawCircleV(playerPosition, playerRadius, playerColor);
            
            // Draw instructions.
            DrawText("MOVE THE PLAYER WITH W,A,S,D", 10, 5, 20, LIGHTGRAY);
            DrawText("CHASE THE ENEMY (BLUE), AND PRESS SPACE", 10, 25, 20, LIGHTGRAY);
            DrawText("WHEN THE BACKSTAB LINE BECOMES GREEN.", 10, 45, 20, LIGHTGRAY);
            DrawText("REMEMBER: YOU MUST BE BEHIND THE ENEMY!", 10, 70, 20, LIGHTGRAY);
            
            // Draw backstabs counter.
            DrawText(FormatText("%i", backstabsCounter), screenWidth-50, 10, 40, GRAY);
            
            // Draw Exit
            DrawText("Esc - EXIT", screenWidth-125, screenHeight-25, 20, LIGHTGRAY);
            
            // Draw MarcMDE
            DrawText("Created by @MarcMDE", 20, screenHeight-25, 20, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
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