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
const int TIME = 25; // time in milliseconds for each chronon, change to speed/slow down
const int ITERATIONS = 7000; 
const int oceanWidth = 80;

const int oceanHeight = 80;
const int screenWidth = 800;
const int screenHeight = 480;
const int OCEAN = 0; 
const int FISH = 1;
const int SHARK = -1;

int fishPopulation = numFish;
int sharkPopulation = numShark;
int chronon = 0;
int x, y, xMove, yMove;

sf::Text fishCounter;
sf::Text sharkCounter;
sf::Font font; // Default font that comes with SFML
Cell ocean[oceanWidth][oceanHeight]; // array cell objects
std::vector<std::tuple<int, int>> moves; // vector to store x y positions
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Wa-Tor Simulation");
sf::Text display;

/**
 * @brief Simulates the addition of creatures to the ocean grid in a Wa-tor simulation.
 *
 * The addCreature function adds a specified number of creatures to the ocean grid at random positions.
 * The Wa-tor simulation typically models a predator-prey ecosystem with fish and sharks.
 *
 * @param creature The type of creature to add (FISH or SHARK).
 * @param nCreatures The number of creatures to add.
 *
 * @details This function iterates through the specified number of creatures, randomly placing them on the ocean grid.
 * For each creature, it selects a random position on the grid, checks if the cell is an ocean cell, and if so,
 * adds the creature to that position. The function also sets the creature's age and updates the cell color
 * based on the creature type (green for fish, red for sharks).
 * If a chosen cell is not an ocean cell, the iteration counter is decremented to retry placing the creature.
 */
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


/**
 * @brief Draws the ocean grid for the Wa-tor simulation.
 *
 * The drawOcean function iterates through each cell in the ocean grid,
 * sets the color for ocean cells to black, and then draws the updated ocean grid
 * on the screen using the SFML window.
 *
 * @details This function is responsible for updating the visual representation
 * of the ocean grid by setting the color of each ocean cell to black.
 * It ensures that any changes in the ocean grid are reflected in the displayed simulation.
 */
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


/**
 * @brief Counts the number of neighboring cells with a specified type around a given position.
 *
 * The checkCellType function determines the number of neighboring cells of a specific type
 * around the cell at coordinates (x, y) in the ocean grid. It considers four cardinal directions
 * (North, East, South, West) and checks each neighboring cell for the specified cellType.
 *
 * @param x The x-coordinate of the cell.
 * @param y The y-coordinate of the cell.
 * @param cellType The type of cell to check for.
 * @return The number of neighboring cells with the specified type.
 *
 * @details This function is useful for determining the number of cells of a particular type
 * around a given cell position in the context of a cellular automaton or grid-based simulation.
 * It updates a moves vector with the coordinates of neighboring cells of the specified type.
 */
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


/**
 * @brief Finds a random move for a creature on the ocean grid.
 *
 * The findMove function determines a random move for a given creature at the specified position (x, y)
 * on the ocean grid. It considers the creature type and searches for neighboring cells of interest,
 * such as OCEAN cells for fish and FISH cells for sharks, to determine the move.
 *
 * @param xPosition Pointer to the x-coordinate of the creature's position.
 * @param yPosition Pointer to the y-coordinate of the creature's position.
 * @param creature The type of creature (FISH or SHARK).
 *
 * @details The function updates the creature's position based on the randomly chosen move.
 * If the creature is a fish, it looks for neighboring ocean cells; if it is a shark, it first looks for
 * neighboring fish cells and, if none are found, looks for neighboring ocean cells.
 */
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



/**
 * @brief Finds a random move for a creature on the ocean grid.
 *
 * The findMove function determines a random move for a creature at the given position (x, y) on the ocean grid.
 * For fish, it looks for neighboring ocean cells; for sharks, it first looks for neighboring fish cells,
 * and if none are found, it looks for neighboring ocean cells.
 *
 * @param xPosition Pointer to the x-coordinate of the creature's position.
 * @param yPosition Pointer to the y-coordinate of the creature's position.
 * @param creature The type of creature (FISH or SHARK).
 *
 * @details The function updates the creature's position based on the randomly chosen move.
 */
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
                        ++fishPopulation;
                    }
                } 
            }
        }
    }
} // end moveFish

/**
 * @brief Moves and updates sharks on the ocean grid in parallel.
 *
 * The moveShark function iterates through each shark on the ocean grid, updating their positions,
 * age, and starve age based on simulation rules. The function uses parallel processing with OpenMP
 * to improve performance.
 *
 * @details The function considers shark movement, feeding, breeding, and potential starvation.
 * It adjusts the shark population and updates the ocean grid accordingly.
 */
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
                    --sharkPopulation;
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
                                ++sharkPopulation;
                            }
                        }
                        else if (ocean[xMove][yMove].Cell::cellType == FISH)
                        {
                            ocean[xMove][yMove].Cell::cellType = SHARK;
                            ocean[xMove][yMove].cell.setFillColor(sf::Color::Red);
                            ocean[xMove][yMove].Cell::madeMove = true;
                            ocean[xMove][yMove].Cell::starveAge = 0;
                            --fishPopulation;
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
                                ++sharkPopulation;
                            }
                        }
                    }
                } 
            }
        }
    }
} // end moveShark

/**
 * @brief Initializes and starts the Wa-tor simulation.
 *
 * The startSimulation function sets up the ocean grid with cells and positions them on the screen.
 * It then adds an initial population of fish and sharks to the grid to begin the simulation.
 *
 * @details The function uses a constant CELL_SIZE to adjust the size of each cell in the grid.
 * It initializes each cell in the ocean grid and positions them on the screen.
 * Finally, it adds an initial population of fish and sharks to start the simulation.
 */
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

/**
 * @brief Initializes SFML text objects for the Wa-tor simulation.
 *
 * The initializeText function sets up SFML text objects, including fish and shark counters,
 * with specific font, size, color, and positions for displaying information on the screen.
 *
 * @details The function configures text objects for displaying fish and shark counts on the screen.
 */
void initializeText() {
    // Use the default font that comes with SFML
    font = sf::Font(); // Default font

    fishCounter.setFont(font);
    fishCounter.setCharacterSize(20);
    fishCounter.setFillColor(sf::Color::White);
    fishCounter.setPosition(10, 10);

    sharkCounter.setFont(font);
    sharkCounter.setCharacterSize(20);
    sharkCounter.setFillColor(sf::Color::White);
    sharkCounter.setPosition(10, 40);
}

/**
 * @brief Main function for running the Wa-tor simulation.
 *
 * The main function initializes the simulation, runs the simulation loop,
 * and updates the display with the ocean grid and population changes.
 *
 * @details The simulation loop continues until the window is closed or the specified number
 * of iterations is reached. It includes handling window events, drawing the ocean grid,
 * moving fish and sharks, and updating the display at a specified time interval.
 *
 * @return An integer indicating the success of the program.
 */
int main() {
    // Initialize SFML window, text objects, and simulation
    startSimulation();
    initializeText();

    // Open the speedup results file for writing
    std::ofstream speedupFile("speedup_results.txt");
    if (!speedupFile.is_open()) {
        std::cerr << "Error opening speedup_results.txt file!" << std::endl;
        return 1;
    }

    // Run the simulation for different thread counts
    // Change numThreads to 1,2,4,8,16
    for (int numThreads : {16}) {
        // Set the number of threads
        omp_set_num_threads(numThreads);

        // Run the simulation
        int start = clock();

        // Continue running the simulation as long as the window is open
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
            window.display();

            std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
            ++chronon;
        }

        int end = clock();
        double simulationTime = double(end - start) / CLOCKS_PER_SEC;

        // Log the simulation time and thread count to the file
        std::cout << "Simulation time with " << numThreads << " threads: " << simulationTime << " seconds." << std::endl;
        speedupFile << numThreads << " " << simulationTime << std::endl;
    }

    // Close the speedup results file
    speedupFile.close();

    return 0;
}
