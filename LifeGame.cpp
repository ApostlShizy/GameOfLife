#include <iostream>
#include <vector>
#include <SDL3/SDL.h>

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 700;
const int CELL_SIZE = 10;
const int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;

void processMouseClick(float mouseX, float mouseY, float& lastMouseX, float& lastMouseY, std::vector<std::vector<bool>>& grid) {
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    int gridX = mouseX / CELL_SIZE;
    int gridY = mouseY / CELL_SIZE;

    if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
        grid[gridY][gridX] = !grid[gridY][gridX];
    }
}

bool isCellAlive(int x, int y, const std::vector<std::vector<bool>>& grid) {
    int aliveNeighbors = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }
            int nearX = x + i;
            int nearY = y + j;
            if (nearX >= 0 && nearX < GRID_WIDTH && nearY >= 0 && nearY < GRID_HEIGHT) {
                aliveNeighbors += grid[nearY][nearX];
            }
        }
    }
    if (grid[y][x]) {
        return aliveNeighbors == 2 || aliveNeighbors == 3;
    }
    else {
        return aliveNeighbors == 3;
    }
}

void updateGrid(std::vector<std::vector<bool>>& grid) {
    std::vector<std::vector<bool>> tempGrid = grid;

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            tempGrid[y][x] = isCellAlive(x, y, grid);
        }
    }

    grid = tempGrid;
}

void renderGrid(SDL_Renderer* renderer, const std::vector<std::vector<bool>>& grid) {

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            SDL_FRect cell;
            cell.x = x * CELL_SIZE;
            cell.y = y * CELL_SIZE;
            cell.w = CELL_SIZE;
            cell.h = CELL_SIZE;

            if (grid[y][x]) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }
            SDL_RenderFillRect(renderer, &cell);            
            SDL_RenderRect(renderer, &cell);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL3: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Game of Life", WINDOW_WIDTH, WINDOW_HEIGHT, Uint32());
    if (!window) {
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        std::cerr << "Could not create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    std::vector<std::vector<bool>> grid(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));

    bool mouseState = false;
    bool isRunning = true;
    bool isPaused = true;
    SDL_Event event;

    int delay = 0;
    float lastMouseX = 0.f;
    float lastMouseY = 0.f;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_EVENT_QUIT:
                    isRunning = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_P && isPaused) {
                        isPaused = false;
                        delay = 100;
                    }
                    else if (event.key.key == SDLK_P && !isPaused) {
                        isPaused = true;
                        delay = 0;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN :
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        mouseState = true;
                        float mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        processMouseClick(mouseX, mouseY, lastMouseX, lastMouseY, grid);
                    }       
                    break;

                case SDL_EVENT_MOUSE_MOTION :
                    if (mouseState) {
                        float mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        if (abs(lastMouseX - mouseX) >= CELL_SIZE || abs(lastMouseY - mouseY) >= CELL_SIZE) {
                            processMouseClick(mouseX, mouseY, lastMouseX, lastMouseY, grid);
                        }
                    }                    
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    mouseState = false;
                    break;

                default:
                    break;
            }
        }

        if (!isPaused) {
            updateGrid(grid);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderGrid(renderer, grid);

        SDL_RenderPresent(renderer);

        SDL_Delay(delay);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}