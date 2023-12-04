/**
 * @file main.cpp
 * Sam Cullen - C00250093
 * @brief {Doxygen documentation for the Wa-tor simluation 2023
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

const int numFish = 500; //820 450 5 5 5 100
const int numShark = 250;
const int fishBreed = 5;
const int sharkBreed = 7;
const int STARVE = 5;
const int TIME = 50; // time in milliseconds for each chronon, change to speed/slow down
const int ITERATIONS = 7000; 
const int oceanWidth = 80;

const int oceanHeight = 80;
const int screenWidth = 800;
const int screenHeight = 480;
const int OCEAN = 0; 
const int FISH = 1;
const int SHARK = -1;

int fishPop = numFish;
int sharkPop = numShark;
int chronon = 0;
int x, y, xMove, yMove;

sf::Text fishCounter;
sf::Text sharkCounter;
sf::Font font; // Default font that comes with SFML
Cell ocean[oceanWidth][oceanHeight]; // array cell objects
std::vector<std::tuple<int, int>> moves; // vector to store x y positions
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Wa-Tor Simulation");
sf::Text display;


void addCreature(int creature, int nCreatures) 
{
    
    for (int i = 0; i < nCreatures; ++i) 
    {
        x = rand() % oceanWidth;
        y = rand() % oceanHeight;
        if (ocean[x][y].Cell::cellType == OCEAN)
        {
            ocean[x][y].Cell::age = rand() % fishBreed;
            ocean[x][y].Cell::cellType = creature;

            if (creature == FISH)
            {
                ocean[x][y].cell.setFillColor(sf::Color::Green);
            }
            else // shark exists
            {
                ocean[x][y].cell.setFillColor(sf::Color::Red);
            }
        }
        else 
        {
            --i;
        }
    }
}


void drawOcean() {
    for (x = 0; x < oceanWidth; ++x) {
        for (y = 0; y < oceanHeight; ++y) {
            ocean[x][y].madeMove = false;
            if (ocean[x][y].cellType == OCEAN) {
                // Set a blank color for ocean cells
                ocean[x][y].cell.setFillColor(sf::Color::Black);
            }
            window.draw(ocean[x][y].cell);
        }
    }
}



int checkCellType(int x, int y, int cellType)
{
    int counter = 0;
    const int directions[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; // North, East, South, West

    for (const auto& direction : directions)
    {
        int newX = (x + direction[0] + oceanWidth) % oceanWidth;
        int newY = (y + direction[1] + oceanHeight) % oceanHeight;

        if (ocean[newX][newY].Cell::cellType == cellType)
        {
            moves.push_back(std::make_tuple(newX, newY));
            ++counter;
        }
    }

    return counter;
}


void findMove(int *xPosition, int *yPosition, int creature) 
{
    int counter = 0;
    int x = *xPosition;
    int y = *yPosition;
    if (creature == FISH)
    {
        counter = checkCellType(x, y , OCEAN);
        if (!moves.empty())
        {
            int move = rand() % int(counter);
            (*xPosition) = std::get<0>(moves[move]);
            (*yPosition) = std::get<1>(moves[move]);
        }
    }
    else 
    {
        counter = checkCellType(x, y , FISH);
        if (!moves.empty())
        {
            int move = rand() % int(counter);
            (*xPosition) = std::get<0>(moves[move]);
            (*yPosition) = std::get<1>(moves[move]);
        }
        else // No food found, keep looking
        {
            counter = checkCellType(x, y , OCEAN);
            if (!moves.empty())
            {
                int move = rand() % int(counter);
                (*xPosition) = std::get<0>(moves[move]);
                (*yPosition) = std::get<1>(moves[move]);
            }
        }
    }
    moves.clear();
} // end findMove


void moveFish()
{
    #pragma omp parallel for collapse(2) num_threads(4)
    for (x = 0; x < oceanWidth; ++x)
    {  
        for (y = 0; y < oceanHeight; ++y)
        {  
            if (ocean[x][y].Cell::cellType == FISH
                && ocean[x][y].Cell::madeMove == false)
            {   
                xMove = x;
                yMove = y;
                findMove(&xMove, &yMove, FISH);
                if (xMove == x && yMove == y)
                {
                    ocean[x][y].Cell::age++;
                }
                else // position available
                {
                    ocean[xMove][yMove].cell.setFillColor(sf::Color::Green);
                    ocean[xMove][yMove].Cell::cellType = FISH;
                    ocean[xMove][yMove].Cell::madeMove = true;
                    if (ocean[x][y].Cell::age < fishBreed)
                    {
                        ocean[x][y].cell.setFillColor(sf::Color::Blue);
                        ocean[x][y].Cell::cellType = OCEAN;
                        ocean[xMove][yMove].Cell::age = ocean[x][y].Cell::age + 1;
                    }
                    else 
                    {
                        ocean[xMove][yMove].Cell::age = 0;
                        ocean[x][y].Cell::age = 0;
                        ++fishPop;
                    }
                } 
            }
        }
    }
} // end moveFish


void moveShark()
{
    #pragma omp parallel for collapse(2) num_threads(4)
    for (x = 0; x < oceanWidth; ++x)
    {  
        for (y = 0; y < oceanHeight; ++y)
        {
            if (ocean[x][y].Cell::cellType == SHARK
                && ocean[x][y].Cell::madeMove == false)
            {   
                if (ocean[x][y].Cell::starveAge == STARVE)
                {
                    ocean[x][y].cell.setFillColor(sf::Color::Blue);
                    ocean[x][y].Cell::cellType = OCEAN;
                    --sharkPop;
                }
                else // shark exists
                {
                    if (xMove == x && yMove == y)
                    {
                        ocean[x][y].Cell::age++;
                        ocean[x][y].Cell::starveAge++;
                    }
                    else // we have a position to move to
                    {
                        xMove = x;
                        yMove = y;
                        findMove(&xMove, &yMove, SHARK);
                        if (ocean[xMove][yMove].Cell::cellType == OCEAN)
                        {
                            ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                            ocean[xMove][yMove].Cell::cellType = SHARK;
                            ocean[xMove][yMove].Cell::madeMove = true;
                            ocean[xMove][yMove].Cell::starveAge = ocean[x][y].Cell::starveAge +1;
                            if (ocean[x][y].Cell::age < sharkBreed)
                            {
                                ocean[x][y].cell.setFillColor(sf::Color::Blue);
                                ocean[x][y].Cell::cellType = OCEAN;
                                ocean[xMove][yMove].Cell::age = ocean[x][y].Cell::age + 1;
                            }
                            else // shark got jiggy with it
                            {
                                ocean[xMove][yMove].Cell::age = 0;
                                ocean[xMove][yMove].Cell::starveAge = 0;
                                ocean[x][y].Cell::age = 0;
                                ++sharkPop;
                            }
                        }
                        else if (ocean[xMove][yMove].Cell::cellType == FISH)
                        {
                            ocean[xMove][yMove].Cell::cellType = SHARK;
                            ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                            ocean[xMove][yMove].Cell::madeMove = true;
                            ocean[xMove][yMove].Cell::starveAge = 0;
                            --fishPop;
                            if (ocean[x][y].Cell::age < sharkBreed)
                            {
                                ocean[x][y].cell.setFillColor(sf::Color::Blue);
                                ocean[x][y].Cell::cellType = OCEAN;
                                ocean[xMove][yMove].Cell::age = ocean[x][y].Cell::age + 1;  
                            }
                            else
                            {
                                ocean[xMove][yMove].Cell::age = 0;
                                ocean[x][y].Cell::age = 0;
                                ocean[x][y].cell.setFillColor(sf::Color::Red);
                                ++sharkPop;
                            }
                        }
                    }
                } 
            }
        }
    }
} // end moveShark


void startSimulation()
{
    const int CELL_SIZE = 10;  // Adjust the cell size as needed
    int xPos = 0;
    int yPos = 0;
    //#pragma omp parallel for collapse(2)
    for (x = 0; x < oceanWidth; ++x)
    {
        for (y = 0; y < oceanHeight; ++y)
        {
            ocean[x][y] = Cell();
            ocean[x][y].cell.setPosition(sf::Vector2f(float(yPos), float(xPos)));
            xPos += CELL_SIZE;  // set size of grid to match screen
        }
        xPos = 0;
        yPos += CELL_SIZE;  
    }
    addCreature(FISH, numFish);
    addCreature(SHARK, numShark);
}


void initializeText() {
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


int main() {
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
/* Running on 1, 2, 4, 8 threads
int main() {
    startSimulation();

    // Replace (1) with 2,4,8 
    omp_set_num_threads(1);
    for (int i = 0; i < ITERATIONS; ++i) {
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


*/