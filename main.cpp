// main.cpp
#include <iostream>
#include <chrono>
#include <thread>
#include "wator.h"
#include <SFML/Graphics.hpp>

const int oceanWidth = 80;
const int oceanHeight = 80;
const int numFish = 500;
const int numShark = 250;

Cell ocean[oceanWidth][oceanHeight]; // Declare ocean grid

sf::RenderWindow window(sf::VideoMode(800, 600), "Wa-Tor Simulation");
sf::Text fishCounter;
sf::Text sharkCounter;

void initializeText() {
    if (!font.loadFromFile("arial.ttf")) {
        // Handle font loading error
    }

    fishCounter.setFont(font);
    fishCounter.setCharacterSize(20);
    fishCounter.setFillColor(sf::Color::White);
    fishCounter.setPosition(10, 10);

    sharkCounter.setFont(font);
    sharkCounter.setCharacterSize(20);
    sharkCounter.setFillColor(sf::Color::White);
    sharkCounter.setPosition(10, 40);
}

void updateCounters() {
    fishCounter.setString("Fish: " + std::to_string(fishPop));
    sharkCounter.setString("Sharks: " + std::to_string(sharkPop));
}

void drawCounters() {
    window.draw(fishCounter);
    window.draw(sharkCounter);
}

void addCreature(int creature, int nCreatures) {
    for (int i = 0; i < nCreatures; ++i) {
        int x = rand() % oceanWidth;
        int y = rand() % oceanHeight;
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

void drawOcean() {
    window.clear();
    for (int x = 0; x < oceanWidth; ++x) {
        for (int y = 0; y < oceanHeight; ++y) {
            ocean[x][y].madeMove = false;
            if (ocean[x][y].cellType == OCEAN) {
                ocean[x][y].cell.setFillColor(sf::Color::Black);
            }
            window.draw(ocean[x][y].cell);
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
            // If the cell matches the specified type, increment counter
            ++counter;
        }
    }

    return counter;
}

void findMove(int *xPosition, int *yPosition, int creature) {
    int counter = checkCellType(*xPosition, *yPosition, creature);

    if (counter > 0) {
        int move = rand() % counter;

        // Find a random neighboring cell with the specified type
        for (const auto& direction : directions) {
            int newX = ((*xPosition) + direction[0] + oceanWidth) % oceanWidth;
            int newY = ((*yPosition) + direction[1] + oceanHeight) % oceanHeight;

            if (ocean[newX][newY].cellType == creature) {
                if (move == 0) {
                    // Update the creature's position
                    (*xPosition) = newX;
                    (*yPosition) = newY;
                    break;
                }
                --move;
            }
        }
    }
}

void moveFish() {
    for (int x = 0; x < oceanWidth; ++x) {
        for (int y = 0; y < oceanHeight; ++y) {
            if (ocean[x][y].cellType == FISH && !ocean[x][y].madeMove) {
                int xMove = x;
                int yMove = y;
                findMove(&xMove, &yMove, OCEAN);

                if (xMove != x || yMove != y) {
                    ocean[x][y].madeMove = true;
                    ocean[xMove][yMove].cell.setFillColor(sf::Color::Green);
                    ocean[xMove][yMove].cellType = FISH;

                    // Handle breeding logic
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
    for (int x = 0; x < oceanWidth; ++x) {
        for (int y = 0; y < oceanHeight; ++y) {
            if (ocean[x][y].cellType == SHARK && !ocean[x][y].madeMove) {
                int xMove = x;
                int yMove = y;
                findMove(&xMove, &yMove, FISH);

                if (ocean[xMove][yMove].cellType == FISH) {
                    // Shark found fish
                    ocean[xMove][yMove].cellType = SHARK;
                    ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                    ocean[xMove][yMove].madeMove = true;
                    ocean[xMove][yMove].starveAge = 0;
                    --fishPop;

                    // Handle breeding logic
                    if (ocean[x][y].age < sharkBreed) {
                        ocean[x][y].cell.setFillColor(sf::Color::Blue);
                        ocean[x][y].cellType = OCEAN;
                        ocean[xMove][yMove].age = ocean[x][y].age + 1;
                    } else {
                        ocean[xMove][yMove].age = 0;
                        ocean[x][y].age = 0;
                        ++sharkPop;
                    }
                } else {
                    // No fish found, move to an empty cell
                    findMove(&xMove, &yMove, OCEAN);
                    if (ocean[xMove][yMove].cellType == OCEAN) {
                        ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                        ocean[xMove][yMove].cellType = SHARK;
                        ocean[xMove][yMove].madeMove = true;
                        ocean[xMove][yMove].starveAge = ocean[x][y].starveAge + 1;

                        // Handle breeding logic
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

    for (int x = 0; x < oceanWidth; ++x) {
        for (int y = 0; y < oceanHeight; ++y) {
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

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        drawOcean();
        moveFish();
        moveShark();
        updateCounters();
        drawCounters();
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
