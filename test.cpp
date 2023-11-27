#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

int numSharks = 30;
int numFish = 5;
int FishBreed = 1000;
int SharkBreed = 200;
int Starve = 75;
int GridSize = 10;
int Threads = 2;

int xdim = 100;
int ydim = 100;
int WindowXSize = 1000;
int WindowYSize = 1000;
int cellXSize = WindowXSize / xdim;
int cellYSize = WindowYSize / ydim;

class Fish {
public:
    Fish(int x, int y);
    void move(std::vector<std::vector<int>>& grid);
    void reproduce(std::vector<Fish>& fishList);
    int getX() const { return x; }
    int getY() const { return y; }

private:
    int x, y;
    int reproductionTime;
};

class Shark {
public:
    Shark(int x, int y);
    void move(std::vector<std::vector<int>>& grid);
    void reproduce(std::vector<Shark>& sharkList);
    void decreaseEnergy();
    int getX() const { return x; }
    int getY() const { return y; }



private:
    int x, y;
    int reproductionTime;
    int energy;
};
void Shark::decreaseEnergy() {
    energy--;

    if (energy <= 0) {
        energy = 0;
    }
}

std::vector<Fish> fishList;

Fish::Fish(int x, int y) : x(x), y(y), reproductionTime(0) {}

void Fish::move(std::vector<std::vector<int>>& grid) {
    int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right

    int newX = x;
    int newY = y;

    switch (direction) {
        case 0:
            newY = (y > 0) ? y - 1 : y;
            break;
        case 1:
            newY = (y < grid[0].size() - 1) ? y + 1 : y;
            break;
        case 2:
            newX = (x > 0) ? x - 1 : x;
            break;
        case 3:
            newX = (x < grid.size() - 1) ? x + 1 : x;
            break;
    }

    // Check if the new position is within the grid boundaries and unoccupied
    if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size() && grid[newX][newY] == 0) {
        grid[x][y] = 0;  // Clear current position
        x = newX;
        y = newY;
        grid[x][y] = 1;  // Set new position
    }
}

void Fish::reproduce(std::vector<Fish>& fishList) {
    if (reproductionTime >= FishBreed) {
        fishList.push_back(Fish(x, y));
        reproductionTime = 0;
    } else {
        reproductionTime++;
    }
}

Shark::Shark(int x, int y) : x(x), y(y), reproductionTime(0) {}

void Shark::move(std::vector<std::vector<int>>& grid) {
    int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right

    int newX = x;
    int newY = y;

    switch (direction) {
        case 0:
            newY = (y > 0) ? y - 1 : y;
            break;
        case 1:
            newY = (y < grid[0].size() - 1) ? y + 1 : y;
            break;
        case 2:
            newX = (x > 0) ? x - 1 : x;
            break;
        case 3:
            newX = (x < grid.size() - 1) ? x + 1 : x;
            break;
    }
    // Check if the new position is within the grid boundaries
    if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size()) {
        // If there's a fish at the new position, eat it
        if (grid[newX][newY] == 1) {
            energy += 5;
            // Remove the fish from the grid and fishList
            grid[newX][newY] = 0;
            auto it = std::remove_if(fishList.begin(), fishList.end(),
                [newX, newY](const Fish& fish) {
                    return fish.getX() == newX && fish.getY() == newY;
                });
            fishList.erase(it, fishList.end());
        }

        // Move the shark to the new position
        grid[x][y] = 0;  // Clear current position
        x = newX;
        y = newY;
        grid[x][y] = 2;  // Set new position
    }

    decreaseEnergy();
}
void Shark::reproduce(std::vector<Shark>& sharkList) {
    if (reproductionTime >= SharkBreed) {
        sharkList.push_back(Shark(x, y));
        reproductionTime = 0;
    } else {
        reproductionTime++;
    }
}

void initializeGrid(std::vector<std::vector<int>>& grid, int xdim, int ydim) {
    grid.resize(xdim, std::vector<int>(ydim, 0));
}

void updateGrid(std::vector<Fish>& fishList, std::vector<Shark>& sharkList, std::vector<std::vector<int>>& grid) {
    // Temporarily store new fish and sharks to avoid concurrent modifications
    std::vector<Fish> newFishList;
    std::vector<Shark> newSharkList;

    // Update fish positions on the grid
    for (const auto& fish : fishList) {
        grid[fish.getX()][fish.getY()] = 1;
    }

    for (const auto& shark : sharkList) {
        grid[shark.getX()][shark.getY()] = 0;
    }

    // Move and update fish
    for (int i = 0; i < fishList.size(); ++i) {
        Fish newFish = fishList[i];
        newFish.move(grid);
        newFish.reproduce(newFishList);

        newFishList.push_back(newFish);
    }

    // Move and update sharks
    for (int i = 0; i < sharkList.size(); ++i) {
        Shark newShark = sharkList[i];
        newShark.move(grid);
        newShark.reproduce(newSharkList);

        newSharkList.push_back(newShark);
    }

    // Update shark positions on the grid after moving
    for (const auto& shark : newSharkList) {
        grid[shark.getX()][shark.getY()] = 2;
    }

    // Update fish and shark lists with the new lists
    fishList = newFishList;
    sharkList = newSharkList;
}

void drawGrid(sf::RenderWindow& window, std::vector<std::vector<int>>& grid, int cellXSize, int cellYSize) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[0].size(); ++j) {
            sf::RectangleShape cell(sf::Vector2f(static_cast<float>(cellXSize), static_cast<float>(cellYSize)));
            cell.setPosition(i * cellXSize, j * cellYSize);

            if (grid[i][j] == 1) {
                cell.setFillColor(sf::Color::Green); // Fish
            } else if (grid[i][j] == 2) {
                cell.setFillColor(sf::Color::Red); // Shark
            } else {
                cell.setFillColor(sf::Color::Blue); // Empty
            }

            window.draw(cell);
        }
    }
}

int main() {
    std::vector<Shark> sharkList;
    std::vector<std::vector<int>> grid;

    initializeGrid(grid, xdim, ydim);

    // Populate initial fish and shark populations
    for (int i = 0; i < numFish; ++i) {
        fishList.push_back(Fish(rand() % xdim, rand() % ydim));
    }

    for (int i = 0; i < numSharks; ++i) {
        sharkList.push_back(Shark(rand() % xdim, rand() % ydim));
    }

    sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "SFML Wa-Tor world");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // Update and draw the simulation
        updateGrid(fishList, sharkList, grid);
        drawGrid(window, grid, cellXSize, cellYSize);

        window.display();
    }

    return 0;
}
