# Maze Generation Using Depth-First Search

This project involves creating a maze using the depth-first search (DFS) algorithm, implemented in C. The process starts at a random point on a grid, where the algorithm explores unvisited neighboring cells, marking them as part of the maze while backtracking when needed. This method generates a perfect maze, ensuring no loops or isolated sections, with only one unique path between any two points. The project aims to efficiently implement the algorithm in C and visualize the maze.

### Prerequisites

Here are the requirements for the software and tools needed to build, test, and deploy the project:

- **C Compiler:** A C compiler like GCC to compile the code.
- **Raylib Library:** Install the raylib library for graphics support. Follow the [official raylib installation guide](https://www.raylib.com/).

### Execution

Once raylib is installed, navigate to the directory containing `mazegenerator.c` and use the following command to compile the program:

```bash
cc mazegenerator.c $(pkg-config --libs --cflags raylib) -o mazegenerator
```

After compiling, run the program with
```bash
./mazegenerator
```

### Usage
Once the code is running, a maze window will appear. You can interact with the maze using the following key controls:

- **Key `S`:** Take a single step in the maze generation process.
- **Key `A`:** Automatically complete the maze generation.
- **Key `P`:** Toggle pause/play for the animation.
- **Key `1` and `2`:** Adjust the animation speed. `1` decreases the speed, and `2` increases it.
- **Key `R`:** Reset the maze generation, allowing you to start over.
- **Key `D`:** Toggle the drawing of the path in the maze.
- **Key `F1`:** Take a screenshot of the current maze and save it as `screenshot.png`.



