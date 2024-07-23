#include "raylib.h"
#include "entity.h"
#include <vector>
#include <algorithm>
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

Entity player;
void MovePlayer();
bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2);

Color detectArea_color = { 0,0,0, 50 };

int testMap[10][10] = 
{
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,0,0,0,1,
    0,0,0,0,0,0,0,0,0,1,
    0,0,0,0,0,0,0,0,0,1,
    0,0,0,0,0,0,0,0,0,1,
    0,0,0,0,0,0,0,0,0,1,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    1,2,1,2,1,2,1,2,1,2,
};

int tileSize = 30;

int main(void)
{
    
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "swept AABB test");

    Entity block;

    player.position = {591,166};
    player.velocity = { 0,0 };
    player.width = 30;
    player.height = 45;
    player.collision = false;

    can_jump = false;

    block.width = 200;
    block.height = 200;

    block.position = { (float)(screenWidth - block.width ) / 2, (float)(screenHeight - block.height ) / 2 };
    block.velocity = { 0,0 };

    Color ray_color = BLUE;

    Rectangle floor = { 0, screenHeight - 30, screenWidth,30};
    Rectangle rekt = { screenWidth-100, screenHeight - 60, 100,30 };

    Rectangle maptile;

    float detectionRadius = 10.0f * tileSize;  // Example radius
    Rectangle detectionArea;

    detectionArea = {
        player.position.x - detectionRadius,
        player.position.y - detectionRadius,
        detectionRadius * 2,
        detectionRadius * 2
    };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        Rectangle bloque = { block.position.x, block.position.y, block.width, block.height };


        detectionArea.x = player.position.x - detectionRadius;
        detectionArea.y = player.position.y - detectionRadius;

        player.velocity.y += 100;
        if (player.velocity.y == 400)
        {
            player.velocity.y = 400;
        }

        MovePlayer();

        ResolveFrameCollisions(&player);
        
        Vector2 contactPoint{};
        float normalX, normalY;
        float time = 0;

        //create a vector the first parameter is a unique index for each map tile based on its position in the grid.
        //the second is time, time of collision
        std::vector<std::pair<int, float>> collisions;

        //loop through the matrix or array
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                Rectangle maptile = { (float)j * tileSize, (float)i * tileSize, tileSize, tileSize };

                if (CheckCollisionRecs(detectionArea, maptile))
                {
                    if (testMap[i][j] != 0)
                    {
                        if (DynamicRectVsRect(&player, &maptile, contactPoint, normalX, normalY, time, GetFrameTime()))
                        {
                            collisions.push_back({ i * 10 + j, time });
                        }
                    }
                }
            }
        }

        // Step 2: Sort Collisions by Time
        std::sort(collisions.begin(), collisions.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
        {
            return a.second < b.second;
        });

        // Step 3: Resolve Collisions in Sorted Order
        for (const auto& collision : collisions)
        {
            int index = collision.first;
            int row = index / 10;
            int col = index % 10;

            Rectangle maptile = { (float)col * tileSize, (float)row * tileSize, tileSize, tileSize };
            ResolveDynamicRectVsRect(&player, &maptile);
        }

        
        ResolveDynamicRectVsRect(&player, &bloque);
        ResolveDynamicRectVsRect(&player, &floor);
        ResolveDynamicRectVsRect(&player, &rekt);

        UpdateEntity(&player, GetFrameTime());
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawEntity(&block, ray_color);

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                if (testMap[i][j] != 0)
                {
                    if (testMap[i][j] % 2 == 0)
                    {
                        DrawRectangle(j * tileSize, i * tileSize, tileSize, tileSize, DARKPURPLE);
                    }
                    else
                    {
                        DrawRectangle(j * tileSize, i * tileSize, tileSize, tileSize, PURPLE);
                    }
                    
                }
                
            }
        }

        DrawRectangle(floor.x, floor.y, floor.width, floor.height, GREEN);
        DrawRectangle(rekt.x, rekt.y, rekt.width, rekt.height, DARKBLUE);

        DrawRectangle(detectionArea.x, detectionArea.y, detectionArea.width, detectionArea.height, detectArea_color);
        DrawEntity(&player, DARKGREEN);
        

        

        

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void MovePlayer()
{
    float move_speed = 500;
    if (IsKeyDown(KEY_D))
    {
        player.velocity.x = move_speed;
    }
    else if (IsKeyDown(KEY_A))
    {
        player.velocity.x = -move_speed;
    }
    else
    {
        player.velocity.x = 0;
    }


    if (IsKeyDown(KEY_W))
    {
        player.velocity.y =  -move_speed;
    }
    else if (IsKeyDown(KEY_S))
    {
        player.velocity.y = move_speed;
    }
    else if (IsKeyPressed(KEY_SPACE) && can_jump)
    {
        player.velocity.y = -1500;
        can_jump = false;
    }
}

bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    return (rec1.x < rec2.x + rec2.width &&
        rec1.x + rec1.width > rec2.x &&
        rec1.y < rec2.y + rec2.height &&
        rec1.y + rec1.height > rec2.y);
}