/**
 * @file wator.cpp
 * Sam Cullen - C00250093
 * @brief {Doxygen documentation for the Wa-tor simluation 2023
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "wator.h"

/**
 * @brief Constructs a Cell object for the Wa-tor simulation.
 *
 * The Cell constructor initializes a cell with default size, color, age, and starve age.
 * It represents an ocean cell with default values.
 */
Cell::Cell()
{
    cell.setSize(sf::Vector2f(3,3));
    cell.setFillColor(sf::Color::Blue);
    age = 0;
    starveAge = 0;
    cellType = 0; // zero represents an ocean cell
    madeMove = false;

}
