// Maze Generation Using Depth-First Search //

#include <stdio.h>
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define WIDTH 800
#define HEIGHT 800

// Global variables for controlling the animation and drawing
bool draw_path = true;
bool animating = false;
float anim_frame_time = 0.2f; // Time between animation frames 

// Structure representing the maze
typedef struct Maze {
    int width; 
    int height; 
    bool *right_walls; 
    bool *down_walls; 
} Maze;

// Structure representing a point in the maze grid
typedef struct Point {
    int x;
    int y;
} Point;

// Four possible directions
enum DIR {
    DIR_DOWN = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT,
};

// Function to set the right wall of a specific cell in the maze
void set_right_wall(Maze *maze, Point at, bool value) {
    maze->right_walls[at.x + at.y * maze->width] = value;
}

// Function to set the bottom wall of a specific cell in the maze
void set_down_wall(Maze *maze, Point at, bool value) {
    maze->down_walls[at.x + at.y * maze->width] = value;
}

// Structure representing the dynamic array of points
typedef struct PointList {
    Point *items;
    int count;
    int capacity;
} PointList;

// Function to create PointList
PointList make_list(int cap) {
    Point *items = calloc(cap, sizeof(Point));
    PointList ptls = {
        .items = items,
        .count = 0,
        .capacity = cap,
    };

    return ptls;
}

// Macro to append an item to a dynamic array (PointList)
// It reallocates memory if the array is full
#define da_append(arr, item) \
    do {\
        if ((arr)->count >= (arr)->capacity) {\
            (arr)->capacity *= 2;\
            (arr)->items = realloc((arr)->items, (arr)->capacity * sizeof((arr)->items[0]));\
            assert((arr)->items != NULL);\
        }\
        (arr)->items[(arr)->count++] = item;\
    } while(0)\

// Macro to pop the last item from a dynamic array (PointList)
#define da_pop(arr) \
    (assert((arr)->count > 0), (arr)->items[--(arr)->count])

// Function to draw the maze on the screen
void draw_maze(Rectangle rect, Maze maze) {
    float cell_width = rect.width / maze.width;
    float cell_height = rect.height / maze.height;

    DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, WHITE); // Draw the outer border of the maze

    for (int j = 0; j < maze.height; j++) {
        for (int i = 0; i < maze.width; i++) {
            int x = rect.x + cell_width * i;
            int y = rect.y + cell_height * j;

            // Draw the right wall of the cell if it exists
            if (i < maze.width - 1 && maze.right_walls[i + j * maze.width]) {
                DrawLine(x + cell_width, y, x + cell_width, y + cell_height, WHITE);
            }

            // Draw the bottom wall of the cell if it exists
            if (j < maze.height - 1 && maze.down_walls[i + j * maze.width]) {
                DrawLine(x, y + cell_height, x + cell_width, y + cell_height, WHITE);
            }
        }
    }

}

// Function to enable all walls in the maze
void enable_all_walls(Maze *maze) {
    int n_cells = maze->width * maze->height;
    memset(maze->right_walls, 1, sizeof(bool) * n_cells);
    memset(maze->down_walls, 1, sizeof(bool) * n_cells);
}

// Function to randomly shuffle an array of integers
void random_shuffle(int *nums, int length) {
    for (int i = 0; i < length; i++) {
        int shuffle_index = i + (rand() % (length - i)); // Randomly pick an index to swap with

        int temp = nums[i];
        nums[i] = nums[shuffle_index];
        nums[shuffle_index] = temp;
    }

}

// Function to check if a point is within bunds of the maze
bool in_bounds(Maze maze, Point pt) {
    return (pt.x >= 0 && pt.x < maze.width && pt.y >= 0 && pt.y < maze.height);
}

// Function to check if a PointList contains a specific point
bool list_contains(PointList ls, Point needle) {
    for (int i = 0; i < ls.count; i++) {
        Point hay = ls.items[i];
        if (hay.x == needle.x && hay.y == needle.y) {
            return true;
        }
    }

    return false;
}

// Function to take one step in the maze generation process
bool one_step(Maze maze, PointList visited, Point *current) {
    int dirs[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    random_shuffle(dirs, 4);

    // Try moving in each direction
    for (int i = 0; i < 4; i++) {
        int dir = dirs[i];

        switch (dir) {
            case DIR_UP:
                {
                    Point candidate = {
                        .x = current->x,
                        .y = current->y - 1,
                    };
                    if (in_bounds(maze, candidate) && !list_contains(visited, candidate)) {
                        *current = candidate;
                        return true;
                    }
                    break;
                }
            case DIR_DOWN:
                {
                    Point candidate = {
                        .x = current->x,
                        .y = current->y + 1,
                    };
                    if (in_bounds(maze, candidate) && !list_contains(visited, candidate)) {
                        *current = candidate;
                        return true;
                    }
                    break;
                }
            case DIR_LEFT:
                {
                    Point candidate = {
                        .x = current->x - 1,
                        .y = current->y,
                    };
                    if (in_bounds(maze, candidate) && !list_contains(visited, candidate)) {
                        *current = candidate;
                        return true;
                    }
                    break;
                }
            case DIR_RIGHT:
                {
                    Point candidate = {
                        .x = current->x + 1,
                        .y = current->y,
                    };
                    if (in_bounds(maze, candidate) && !list_contains(visited, candidate)) {
                        *current = candidate;
                        return true;
                    }
                    break;
                }
        }
    }

    return false; // If no valid move is found, return false
}

// Function to remove the wall between two adjacent cells
void remove_wall_between(Maze *maze, Point old, Point new) {
    enum DIR dir;
    if (old.x + 1 == new.x) {
        dir = DIR_RIGHT;
    } else if (old.x - 1 == new.x) {
        dir = DIR_LEFT;
    } else if (old.y + 1 == new.y) {
        dir = DIR_DOWN;
    } else if (old.y - 1 == new.y) {
        dir = DIR_UP;
    }

    switch (dir) {
        case DIR_RIGHT:
            {
                set_right_wall(maze, old, false);
                break;
            }
        case DIR_LEFT:
            {
                // NEW | OLD
                set_right_wall(maze, new, false);
                break;
            }
        case DIR_UP:
            {
                set_down_wall(maze, new, false);
                break;
            }
        case DIR_DOWN:
            {
                set_down_wall(maze, old, false);
                break;
            }
    }
}

// Function to perform one step in the maze generation process
void maze_gen_step(Maze *maze, Point *current, PointList *visited, PointList *path, PointList *backtracked) {
   if (!list_contains(*visited, *current)) {
       da_append(visited, *current);
   }
   Point old = *current;
   bool succeeded = one_step(*maze, *visited, current);
   if (succeeded) {
       da_append(path, old); 
       remove_wall_between(maze, old, *current);
   } else {
       da_append(backtracked, *current); 
       *current = da_pop(path);
   }
}

int main(void) {
    srand(time(NULL));

    // Initialize the window
    InitWindow(WIDTH, HEIGHT, "Maze Generator");

    int width, height;
    width = height = 20;
    bool *right_walls = calloc(width * height, sizeof(bool)); 
    bool *down_walls = calloc(width * height, sizeof(bool));
    // Initialize the maze structure
    Maze maze = {
        .width = width,
        .height = height,
        .right_walls = right_walls,
        .down_walls = down_walls,
    };
    enable_all_walls(&maze);

    // Define the window dimensions
    Rectangle window = {
        .x = 0.0,
        .y = 0.0,
        .width = WIDTH,
        .height = HEIGHT,
    };

    // Define the maze window within the main window, with margins
    float margin = 20.0;
    Rectangle maze_window = {
        .x = margin,
        .y = margin,
        .width = window.width - 2.0 * margin,
        .height = window.height - 2.0 * margin,
    };

    // Start at the top-left corner of the maze
    Point current = {
        .x = 0,
        .y = 0,
    };

    // Initialize lists to track visited cells, the path, and backtracking
    PointList visited = make_list(maze.width * maze.height);
    PointList path = make_list(maze.width * maze.height); 
    PointList backtracked = make_list(maze.width * maze.height); 
    float time_since_last_anim_frame = 0.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        draw_maze(maze_window, maze); 

        float cell_width = maze_window.width / maze.width;
        float cell_height = maze_window.height / maze.height;
        float circle_radius = cell_width / 2.0;

        // Display the path if option is enabled
        if (draw_path) {
            for (int i = 0; i < visited.count; i++) {
                Point pt = visited.items[i];
                int margin = 8;
                int x = pt.x * cell_width + maze_window.x;
                int y = pt.y * cell_height + maze_window.y;

                DrawRectangle(x + margin, y + margin, cell_width - margin * 2, cell_height - margin * 2, BLUE);
            }

            for (int i = 0; i < backtracked.count; i++) {
                Point pt = backtracked.items[i];
                int margin = 16;
                int x = pt.x * cell_width + maze_window.x;
                int y = pt.y * cell_height + maze_window.y;

                DrawRectangle(x + margin,
                              y + margin,
                              cell_width - margin * 2,
                              cell_height - margin * 2,
                              RED);
            }

            // Draw the current position in the maze
            DrawCircle(maze_window.x + circle_radius + current.x * cell_width, 
                       maze_window.y + circle_radius + current.y * cell_height,
                       circle_radius,
                       GOLD);
        }

        // If animating, perform a step in the maze generation
        if (animating) {
            time_since_last_anim_frame += GetFrameTime();
            if (time_since_last_anim_frame > anim_frame_time) {
                if (visited.count < maze.width * maze.height) {
                    maze_gen_step(&maze, &current, &visited, &path, &backtracked);
                    time_since_last_anim_frame = 0.0f;
                }
            }
        }

        // Key handling for single step, auto step, and pause
        if (IsKeyReleased(KEY_S)) {
            maze_gen_step(&maze, &current, &visited, &path, &backtracked);
        }
        if (IsKeyReleased(KEY_A)) {
            while (visited.count < maze.width * maze.height) {
                maze_gen_step(&maze, &current, &visited, &path, &backtracked);
            }
        }

        // Toggle animation pause/play
        if (IsKeyReleased(KEY_P)) {
            animating = !animating;
            time_since_last_anim_frame = 0.0f;
        }

        // Adjust animation speed
        if (IsKeyReleased(KEY_ONE)) {
            anim_frame_time *= 1.2f;
        }
        if (IsKeyReleased(KEY_TWO)) {
            anim_frame_time /= 1.2f;
        }
        // if (IsKeyReleased(KEY_I)) {
        //     printf("visited count: %d\n", visited.count);
        // }
        
        // Reset the maze generation
        if (IsKeyReleased(KEY_R)) {
            enable_all_walls(&maze);
            current.x = 0;
            current.y = 0;
            visited.count = 0;
            path.count = 0;
            backtracked.count = 0;
        }

        // Toggle drawing of the path
        if (IsKeyReleased(KEY_D)) {
            draw_path = !draw_path;
        }

        EndDrawing();

       // Take screenshot when F1 is pressed
        if (IsKeyReleased(KEY_F1)) {
            TakeScreenshot("screenshot.png");
        }
    }

    CloseWindow();
    return 0;
}
