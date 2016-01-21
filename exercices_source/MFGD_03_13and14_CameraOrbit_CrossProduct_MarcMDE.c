/*******************************************************************************************

*
*   MFGD_03 13 and 14 Camera orbit and cross product
*
*   This program was made by Marc Montagut - @MarcMDE
*
*   It is a practical implementation of the 13th and 14th "Math for Game Developers" videos
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
*
*
*   This program has been created using raylib 1.3.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   raylib [core] example - 3d camera first person
*
*   This example has been created using raylib 1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define MAX_COLUMNS 20

typedef struct EulerAngle
{
    float pitch;
    float yaw;
    float roll;
}EulerAngle;

void UpdateCameraPlayerMovement();
Vector3 EulerAngleToVector3(EulerAngle a);
void EulerAngleNormalize(EulerAngle *a);
Vector3 Vector3Sub(Vector3 a, Vector3 b);
Vector3 Vector3Add(Vector3 a, Vector3 b);
void Vector3Scale(Vector3 *v, float scale);
Vector3 Vector3FloatProduct(Vector3 v, float scale);
Vector3 Vector3Zero();
Vector3 Vector3CrossProduct(Vector3 a, Vector3 b);
float Vector3Length(Vector3 v);
void Vector3Normalize(Vector3 *v);
Vector3 Vector3Product(Vector3 a, Vector3 b);

Camera camera = {{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }};

Vector3 playerSize = {1, 2, 1}; // Cube size
Vector3 playerPosition = { 4, 1, 4 }; // Define player position
Vector3 playerDirection = {0, 0, 0}; // Forward direction
Vector3 playerInputDirection = {0, 0, 0}; // Input direction (WASD)
Vector3 playerSpeed = {0.15f, 0, 0.15f}; // Change value
Vector3 playerVelocity = {0, 0, 0}; // Speed * direction

Vector2 mouseLastPosition = {0, 0};
Vector2 mouseCurrentPosition = {0, 0};
EulerAngle cameraEulerAngle = {0, 0, 0};
float cameraDistance = 5.5f;
Vector3 cameraOffset = {0, 2+0.75f, 0};
float mouseSensivity = 0.01f;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "MFGD_03_13and14_CameraOrbit_CrossProduct - @MarcMDE");

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCameraPlayerMovement();
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            HideCursor();
            
            Begin3dMode(camera);
                DrawPlane((Vector3){ 0, 0, 0 }, (Vector2){ 32, 32 }, LIGHTGRAY); // Draw ground
                DrawCube((Vector3){ -16, 2.5, 0 }, 1, 5, 32, BLUE);     // Draw a blue wall
                DrawCube((Vector3){ 16, 2.5, 0 }, 1, 5, 32, LIME);      // Draw a green wall
                DrawCube((Vector3){ 0, 2.5, 16 }, 32, 5, 1, GOLD);      // Draw a yellow wall
                DrawCube((Vector3){ 0, 2.5, -16 }, 32, 5, 1, RED);      // Draw a red wall
                
                DrawCube(playerPosition, 1, 2, 1, PURPLE); // Draw Player
                
            End3dMode();
            
                DrawText("Move with WASD", 10, 10, 20, DARKGRAY);
                DrawText("Move mouse to look around", 10, 30, 20, DARKGRAY);
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

void UpdateCameraPlayerMovement()
{   
    playerInputDirection = (Vector3){0, 0, 0};
    if (IsKeyDown('W')) playerInputDirection.x = 1;
    else if (IsKeyDown('S')) playerInputDirection.x = -1;
    
    if (IsKeyDown('A')) playerInputDirection.z = 1;
    else if (IsKeyDown('D')) playerInputDirection.z = -1;
    
    playerDirection = EulerAngleToVector3(cameraEulerAngle);
    playerDirection.y = 0;
    Vector3Normalize(&playerDirection);    
    
    playerVelocity = Vector3Add(Vector3FloatProduct(playerDirection, playerSpeed.x*playerInputDirection.x), Vector3FloatProduct(Vector3CrossProduct((Vector3){0, 1, 0}, playerDirection), playerSpeed.z*playerInputDirection.z));
    playerPosition = Vector3Add(playerPosition, playerVelocity);
    
    camera.target = playerPosition;
    
    mouseCurrentPosition = GetMousePosition();
    
    cameraEulerAngle.yaw += (mouseCurrentPosition.x-mouseLastPosition.x)*mouseSensivity;
    cameraEulerAngle.pitch -= (mouseCurrentPosition.y-mouseLastPosition.y)*mouseSensivity;
 
    EulerAngleNormalize(&cameraEulerAngle);
    if (cameraEulerAngle.pitch>0.7f) cameraEulerAngle.pitch = 0.7f;
    else if (cameraEulerAngle.pitch<-0.7f) cameraEulerAngle.pitch = -0.7f;
    mouseLastPosition = mouseCurrentPosition;
    printf("%.3f\n", cameraEulerAngle.pitch);
    
    camera.position = Vector3Sub(Vector3Add(playerPosition, cameraOffset), Vector3FloatProduct(EulerAngleToVector3(cameraEulerAngle), cameraDistance));
}

Vector3 EulerAngleToVector3(EulerAngle a)
{
    return (Vector3){cos(a.yaw)*cos(a.pitch), sin(a.pitch), sin(a.yaw)*cos(a.pitch)};
}

void EulerAngleNormalize(EulerAngle *a)
{
    
    if (a->pitch>89) a->pitch=89;
    if (a->pitch<-89) a->pitch=-89;
    
    while (a->yaw < -180) a->yaw += 360;
    while (a->yaw > 180) a->yaw -= 350;
    
}

Vector3 Vector3Sub(Vector3 a, Vector3 b)
{
    return (Vector3){a.x-b.x, a.y-b.y, a.z-b.z};
}

Vector3 Vector3Add(Vector3 a, Vector3 b)
{
    return (Vector3){a.x+b.x, a.y+b.y, a.z+b.z};
}

void Vector3Scale(Vector3 *v, float scale)
{
    *v = (Vector3){v->x*scale, v->y*scale, v->z*scale};
}

Vector3 Vector3FloatProduct(Vector3 v, float scale)
{
    return (Vector3){v.x*scale, v.y*scale, v.z*scale};
}

Vector3 Vector3Zero()
{
    return (Vector3){0, 0, 0};
}

Vector3 Vector3CrossProduct(Vector3 a, Vector3 b)
{
    return (Vector3){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

float Vector3Length(Vector3 v)
{

    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void Vector3Normalize(Vector3 *v)
{
    float length, ilength;

    length = Vector3Length(*v);

    if (length == 0) length = 1;

    ilength = 1.0/length;

    v->x *= ilength;
    v->y *= ilength;
    v->z *= ilength;
}

Vector3 Vector3Product(Vector3 a, Vector3 b)
{
    return (Vector3){a.x*b.x, a.y*b.y, a.z*b.z};
}