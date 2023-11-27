#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

int numSharks = 15;
int numFish = 25;
int FishBreed = 80;
int SharkBreed = 150;
int Starve = 50;
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
    void move(std::vector<std::vector<int>>& grid, std::vector<Fish>& fishList);
    void reproduce(std::vector<Shark>& sharkList);
    void decreaseEnergy();
    bool isDead();
    int getX() const { return x; }
    int getY() const { return y; }

private:
    int x, y;
    int reproductionTime;
    int energy;
};


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



// Breed fish 

void Fish::reproduce(std::vector<Fish>& fishList) {
    if (reproductionTime >= FishBreed) {
        fishList.push_back(Fish(x, y));
        reproductionTime = 0;
    } else {
        reproductionTime++;
    }
}


Shark::Shark(int x, int y) : x(x), y(y), reproductionTime(0), energy(10) {}

void Shark::move(std::vector<std::vector<int>>& grid, std::vector<Fish>& fishList) {
    int newX = x + rand() % 3 - 1; // Move randomly in x direction
    int newY = y + rand() % 3 - 1; // Move randomly in y direction

    // Check if the new position is within the grid boundaries
    if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size()) {
        x = newX;
        y = newY;
    }

    // Check if there's a fish at the new position and eat it
for (auto it = fishList.begin(); it != fishList.end(); ++it) {
    if (it->getX() == x && it->getY() == y) {
        energy += 5;
        it = fishList.erase(it);
        break; // Shark eats only one fish per iteration
    }
}


    decreaseEnergy();
}


void Shark::reproduce(std::vector<Shark>& sharkList) {
    if (reproductionTime >= 5 && energy >= 10) {
        sharkList.push_back(Shark(x, y));
        reproductionTime = 0;
        energy -= 5; // Energy cost for reproduction
    } else {
        reproductionTime++;
    }
}

void Shark::decreaseEnergy() {
    energy--;

    if (energy <= 0) {
        energy = 0;
    }
}

bool Shark::isDead() {
    return energy <= 0;
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

    // Update shark positions on the grid
    for (const auto& shark : sharkList) {
        grid[shark.getX()][shark.getY()] = 2;
    }

    // Move and update fish
    #pragma omp parallel for
for (int i = 0; i < fishList.size(); ++i) {
    Fish newFish = fishList[i];
    newFish.move(grid);
    newFish.reproduce(newFishList); // Uncomment this line to enable reproduction
    
    #pragma omp critical
    {
        newFishList.push_back(newFish);
    }
    }

    // Move, eat, and update sharks
    #pragma omp parallel for
    for (int i = 0; i < sharkList.size(); ++i) {
        Shark newShark = sharkList[i];
        newShark.move(grid, fishList);
        newShark.reproduce(newSharkList);
        if (!newShark.isDead()) {
            #pragma omp critical
            {
                newSharkList.push_back(newShark);
            }
        }
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
            } else  /*if (grid[i][j] == 3) */{ 
                cell.setFillColor(sf::Color::Blue); // Empty
            }

            window.draw(cell);
        }
    }
}
/*
void drawGrid(sf::RenderWindow& window, std::vector<std::vector<int>>& grid, int cellXSize, int cellYSize) {
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[0].size(); ++j) {
            if (grid[i][j] == 1) {
                sf::RectangleShape fishPixel(sf::Vector2f(1.f, 1.f)); // Create a pixel-sized rectangle
                fishPixel.setPosition(i * cellXSize, j * cellYSize);
                fishPixel.setFillColor(sf::Color::Green); // Fish
                window.draw(fishPixel);
            } else if (grid[i][j] == 2) {
                sf::RectangleShape sharkPixel(sf::Vector2f(1.f, 1.f)); // Create a pixel-sized rectangle
                sharkPixel.setPosition(i * cellXSize, j * cellYSize);
                sharkPixel.setFillColor(sf::Color::Red); // Shark
                window.draw(sharkPixel);
            }
        }
    }
}

*/

int main() {
    

    std::vector<Fish> fishList;
    std::vector<Shark> sharkList;
    std::vector<std::vector<int>> grid;

    initializeGrid(grid, xdim, ydim);

    // Populate initial fish and shark populations
    for (int i = 0; i < 10; ++i) {
        fishList.push_back(Fish(rand() % xdim, rand() % ydim));
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
