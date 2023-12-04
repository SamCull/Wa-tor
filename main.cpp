/**
 * @file main.cpp
 * Sam Cullen - C00250093
 * @brief {Doxygen documentation for the Wa-tor simulation 2023
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "wator.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <time.h>
#include <omp.h>
#include <fstream>

// Constants
const int numFish = 500;
const int numShark = 250;
const int fishBreed = 5;
const int sharkBreed = 7;
const int STARVE = 5;
const int TIME = 50;
const int ITERATIONS = 7000;
const int oceanWidth = 80;
const int oceanHeight = 80;
const int screenWidth = 800;
const int screenHeight = 480;
const int OCEAN = 0;
const int FISH = 1;
const int SHARK = -1;

// Global variables
int fishPop = numFish;
int sharkPop = numShark;
int chronon = 0;
int x, y, xMove, yMove;

sf::Text fishCounter;
sf::Text sharkCounter;
sf::Font font;
Cell ocean[oceanWidth][oceanHeight];
std::vector<std::tuple<int, int>> moves;
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Wa-Tor Simulation");

// Function prototypes
void addCreature(int creature, int nCreatures);
void drawOcean();
int checkCellType(int x, int y, int cellType);
void findMove(int *xPosition, int *yPosition, int creature);
void moveFish();
void moveShark();
void startSimulation();
void initializeText();
void updateCounters();
void drawCounters();

// Function implementations

void addCreature(int creature, int nCreatures) {
    for (int i = 0; i < nCreatures; ++i) {
        x = rand() % oceanWidth;
        y = rand() % oceanHeight;
        if (ocean[x][y].cellType == OCEAN) {
            ocean[x][y].age = rand() % fishBreed;
            ocean[x][y].cellType = creature;

            if (creature == FISH) {
                ocean[x][y].cell.setFillColor(sf::Color::Green);
            } else {
                ocean[x][y].cell.setFillColor(sf::Color::Red);
            }
        } else {
            --i;
        }
    }
}

int checkCellType(int x, int y, int cellType) {
    int counter = 0;
    const int directions[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; // North, East, South, West

    for (const auto& direction : directions) {
        int newX = (x + direction[0] + oceanWidth) % oceanWidth;
        int newY = (y + direction[1] + oceanHeight) % oceanHeight;

        if (ocean[newX][newY].cellType == cellType) {
            moves.push_back(std::make_tuple(newX, newY));
            ++counter;
        }
    }

    return counter;
}

void findMove(int *xPosition, int *yPosition, int creature) {
    int counter = checkCellType(*xPosition, *yPosition, creature);

    if (counter > 0) {
        int move = rand() % counter;

        (*xPosition) = std::get<0>(moves[move]);
        (*yPosition) = std::get<1>(moves[move]);
    }

    moves.clear();
}

void moveFish() {
    #pragma omp parallel for collapse(2) num_threads(4)
    for (x = 0; x < oceanWidth; ++x) {
        for (y = 0; y < oceanHeight; ++y) {
            if (ocean[x][y].cellType == FISH && ocean[x][y].madeMove == false) {
                xMove = x;
                yMove = y;
                findMove(&xMove, &yMove, FISH);

                if (xMove == x && yMove == y) {
                    ocean[x][y].age++;
                } else {
                    ocean[xMove][yMove].cell.setFillColor(sf::Color::Green);
                    ocean[xMove][yMove].cellType = FISH;
                    ocean[xMove][yMove].madeMove = true;
                    if (ocean[x][y].age < fishBreed) {
                        ocean[x][y].cell.setFillColor(sf::Color::Blue);
                        ocean[x][y].cellType = OCEAN;
                        ocean[xMove][yMove].age = ocean[x][y].age + 1;
                    } else {
                        ocean[xMove][yMove].age = 0;
                        ocean[x][y].age = 0;
                        ++fishPop;
                    }
                }
            }
        }
    }
}

void moveShark() {
    #pragma omp parallel for collapse(2) num_threads(4)
    for (x = 0; x < oceanWidth; ++x) {
        for (y = 0; y < oceanHeight; ++y) {
            if (ocean[x][y].cellType == SHARK && ocean[x][y].madeMove == false) {
                if (ocean[x][y].starveAge == STARVE) {
                    ocean[x][y].cell.setFillColor(sf::Color::Blue);
                    ocean[x][y].cellType = OCEAN;
                    --sharkPop;
                } else {
                    if (xMove == x && yMove == y) {
                        ocean[x][y].age++;
                        ocean[x][y].starveAge++;
                    } else {
                        xMove = x;
                        yMove = y;
                        findMove(&xMove, &yMove, SHARK);
                        if (ocean[xMove][yMove].cellType == OCEAN) {
                            ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                            ocean[xMove][yMove].cellType = SHARK;
                            ocean[xMove][yMove].madeMove = true;
                            ocean[xMove][yMove].starveAge = ocean[x][y].starveAge + 1;
                            if (ocean[x][y].age < sharkBreed) {
                                ocean[x][y].cell.setFillColor(sf::Color::Blue);
                                ocean[x][y].cellType = OCEAN;
                                ocean[xMove][yMove].age = ocean[x][y].age + 1;
                            } else {
                                ocean[xMove][yMove].age = 0;
                                ocean[xMove][yMove].starveAge = 0;
                                ocean[x][y].age = 0;
                                ++sharkPop;
                            }
                        } else if (ocean[xMove][yMove].cellType == FISH) {
                            ocean[xMove][yMove].cellType = SHARK;
                            ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                            ocean[xMove][yMove].madeMove = true;
                            ocean[xMove][yMove].starveAge = 0;
                            --fishPop;
                            if (ocean[x][y].age < sharkBreed) {
                                ocean[x][y].cell.setFillColor(sf::Color::Blue);
                                ocean[x][y].cellType = OCEAN;
                                ocean[xMove][yMove].age = ocean[x][y].age + 1;
                            } else {
                                ocean[xMove][yMove].age = 0;
                                ocean[x][y].age = 0;
                                ocean[x][y].cell.setFillColor(sf::Color::Red);
                                ++sharkPop;
                            }
                        }
                    }
                }
            }
        }
    }
}

void startSimulation() {
    const int CELL_SIZE = 10;
    int xPos = 0;
    int yPos = 0;

    for (x = 0; x < oceanWidth; ++x) {
        for (y = 0; y < oceanHeight; ++y) {
            ocean[x][y] = Cell();
            ocean[x][y].cell.setPosition(sf::Vector2f(float(yPos), float(xPos)));
            xPos += CELL_SIZE;
        }
        xPos = 0;
        yPos += CELL_SIZE;
    }

    addCreature(FISH, numFish);
    addCreature(SHARK, numShark);
}


int main() {
    initializeText();
    startSimulation();

    while (window.isOpen() && chronon < ITERATIONS) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        drawOcean();
        moveFish();
        moveShark();
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        ++chronon;
    }

    return 0;
}
