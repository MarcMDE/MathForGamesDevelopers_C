/*******************************************************************************************

*
*   MFGD_04 15 and 16 Bullet Collision (AABB/Vector)
*
*   This program was made by Marc Montagut - @MarcMDE
*
*   This is a practical implementation of the 15th and 16th "Math for Game Developers" videos
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

typedef struct AABox
{
    Vector3 position;
    int width;
}AABox;

typedef struct Box
{
    Vector3 position;
    float scale;
    float width; 
    bool isActive;
}Box;

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
void SwapValues(float *a, float *b);

bool ClipLine(int d, AABox box, Vector3 v0, Vector3 v1, float *fLow, float *fHigh);
bool LineAABBIntersect(AABox box, Vector3 v0, Vector3 v1, Vector3 *intersectionPoint);
void UpdatePlayerShot();
void DrawPlayerShot();

Camera camera = {{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }};

Vector3 playerSize = {1, 2, 1}; // Cube size
Vector3 playerPosition = { 4, 3, 4 }; // Define player position

Vector3 playerDirection = {0, 0, 0}; // Forward direction
Vector3 playerInputDirection = {0, 0, 0}; // Input direction (WASD)
Vector3 playerSpeed = {0.15f, 0, 0.15f}; // Change value
Vector3 playerVelocity = {0, 0, 0}; // Speed * direction

Vector2 mouseLastPosition = {0, 0};
Vector2 mouseCurrentPosition = {0, 0};
EulerAngle cameraEulerAngle = {0, 0, 0};
float cameraDistance = 0.1f;
Vector3 cameraOffset = {0, 0, 0};
float mouseSensivity = 0.01f;

// Shot variables
AABox box = {{1, 1, 1}, 1};


int shotRange = 50;
Vector3 shotOrigin;
Vector3 shotEnd;
bool isShooting = false;
int shotCounter = 0;
int shotCd = 30;
bool drawShot = false;

Box shotBox = {{0, 0, 0}, 0, 0.4f, false};

const int screenWidth = 800;
const int screenHeight = 450;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "MFGD_04_15and16_BulletCollision - @MarcMDE");

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCameraPlayerMovement();
        UpdatePlayerShot();
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            HideCursor();
            
            DrawRectangle(screenWidth/2, screenHeight/2, 5, 5, BLACK);
            
            Begin3dMode(camera);
                DrawPlane((Vector3){ 0, 0, 0 }, (Vector2){ 32, 32 }, LIGHTGRAY); // Draw ground
                DrawCube((Vector3){ -16, 2.5, 0 }, 1, 5, 32, BLUE);     // Draw a blue wall
                DrawCube((Vector3){ 16, 2.5, 0 }, 1, 5, 32, LIME);      // Draw a green wall
                DrawCube((Vector3){ 0, 2.5, 16 }, 32, 5, 1, GOLD);      // Draw a yellow wall
                DrawCube((Vector3){ 0, 2.5, -16 }, 32, 5, 1, RED);      // Draw a red wall
                
                DrawCube(box.position, box.width*2, box.width*2, box.width*2, GREEN);
                
                //DrawCube(playerPosition, 1, 2, 1, PURPLE); // Draw Player
                
                DrawPlayerShot();
                
                //DrawRay((Ray){playerPosition, playerDirection}, BLUE);
                
            End3dMode();
            
                DrawText("Move with WASD", 10, 10, 20, GRAY);
                DrawText("Move mouse to look around", 10, 30, 20, GRAY);
                DrawText("Left Click to shoot the green box", 10, 50, 20, GRAY);
                DrawText("Created by @MarcMDE", 20, screenHeight-25, 20, DARKGRAY);
                
                DrawFPS(500, 10);

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
    
    if (mouseCurrentPosition.x<0||mouseCurrentPosition.x>screenWidth) 
    {
        SetMousePosition((Vector2){screenWidth/2, mouseCurrentPosition.y});
        mouseLastPosition = GetMousePosition();  
    }
    if (mouseCurrentPosition.y<0 || mouseCurrentPosition.y>screenHeight)
    {
        SetMousePosition((Vector2){mouseCurrentPosition.x, screenHeight/2});
        mouseLastPosition = GetMousePosition();
    }
    
    //camera.position = Vector3Sub(Vector3Add(playerPosition, cameraOffset), Vector3FloatProduct(EulerAngleToVector3(cameraEulerAngle), cameraDistance));
    camera.position = Vector3Sub(playerPosition, EulerAngleToVector3(cameraEulerAngle));
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

bool ClipLine(int d, AABox box, Vector3 v0, Vector3 v1, float *fLow, float *fHigh)
{
    // d == current dimension. Where 0 == x / 1 == y / 2 == z
    
    // Results for curent dimension (x, y or z)
    float fDimLow, fDimHigh; 
    
    // Find the unidimensional colliding points
    if (d==0)
    {
        fDimLow = ((box.position.x - box.width) - v0.x)/(v1.x - v0.x);
        fDimHigh = ((box.position.x + box.width) - v0.x)/(v1.x - v0.x);
        //printf("X-Collision\n");
    }
    else if (d==1)
    {
        fDimLow = ((box.position.y - box.width) - v0.y)/(v1.y - v0.y);
        fDimHigh = ((box.position.y + box.width) - v0.y)/(v1.y - v0.y);
        //printf("Y-Collision\n");
    }
    else if (d==2)
    {
        fDimLow = ((box.position.z - box.width) - v0.z)/(v1.z - v0.z);
        fDimHigh = ((box.position.z + box.width) - v0.z)/(v1.z - v0.z);
        //printf("Z-Collision\n");
    }
    else return false;
    
    // Make sure low<high
    if (fDimHigh<fDimLow) SwapValues(&fDimHigh, &fDimLow);
    
    // if dimension high is less thant the low or dimension low is higher than high, it missed
    if (fDimHigh<*fLow || fDimLow>*fHigh) return false;
    
    // Clip the f if posible 
    if (fDimLow>*fLow) *fLow = fDimLow;
    if (fDimHigh<*fHigh) *fHigh = fDimHigh;
    
    if (*fLow > *fHigh) return false;
    
    return true;
}

bool LineAABBIntersect(AABox box, Vector3 v0, Vector3 v1, Vector3 *intersectionPoint)
{
    float fLow = 0;
    float fHigh = 1;
    
    // Check 3 dimensions collision
    if (!ClipLine(0, box, v0, v1, &fLow, &fHigh)) return false; // x
    if (!ClipLine(1, box, v0, v1, &fLow, &fHigh)) return false; // y
    if (!ClipLine(2, box, v0, v1, &fLow, &fHigh)) return false; // z
    
    // Calculate line
    Vector3 line = Vector3Sub(v1, v0);
    
    // Set collision point
    *intersectionPoint = Vector3Add(v0, Vector3FloatProduct(line, fLow));
    
    return true;
}

void UpdatePlayerShot()
{
    if (IsMouseButtonPressed(0))
    {
        shotCounter = 0;
        shotBox.isActive = false;
        shotBox.scale = 0;
        isShooting = false;

        shotOrigin = camera.position;
        shotEnd = EulerAngleToVector3(cameraEulerAngle);
        Vector3Normalize(&shotEnd);
        Vector3Scale(&shotEnd, shotRange);
        shotEnd = Vector3Add(shotOrigin, shotEnd);
        if (LineAABBIntersect(box, shotOrigin, shotEnd, &shotBox.position))
        {       
            isShooting = true;
            shotCounter = shotCd;
            shotBox.isActive = true;   
        }
    }
    else if (isShooting)
    {
        if (shotCounter<=0)
        {
            shotCounter = 0;
            shotBox.isActive = false;
            shotBox.scale = 0;
            isShooting = false;
        }
        else shotCounter--;
        
        if (shotBox.scale < 1) shotBox.scale += 1.0f/shotCd;
    }
}

void DrawPlayerShot()
{
    if (isShooting)
    {   
        Vector3 dire = Vector3Sub(shotBox.position, shotOrigin);
        Vector3Normalize(&dire);
        DrawRay((Ray){shotOrigin, Vector3FloatProduct(dire, 50000)}, BLACK);
        DrawCube(shotBox.position, shotBox.width*shotBox.scale, shotBox.width*shotBox.scale, shotBox.width*shotBox.scale, RED);
    }
}

void SwapValues(float *a, float *b)
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}