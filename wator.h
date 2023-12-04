/**
 * @file wator.h
 * Sam Cullen - C00250093
 * @brief {Doxygen documentation for the Wa-tor simluation 2023
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <SFML/Graphics.hpp>

/**
 * @brief Represents a cell in the Wa-tor simulation grid.
 *
 * The Cell class encapsulates properties and behavior for individual cells in the Wa-tor simulation.
 * Each cell is associated with a rectangular shape, age, starve age, cell type, and movement status.
 */
class Cell
{
public:
	sf::RectangleShape cell;
	Cell();
    int age;
    int starveAge;
    int cellType;
    bool madeMove;
};