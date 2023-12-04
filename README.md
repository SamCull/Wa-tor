# Wator Project

## Author
Sam Cullen

## Overview
This project implements a simulation of the Wa-Tor model, a predator-prey ecosystem simulation, in C++ using the SFML library for graphics. The Wa-Tor simulation involves fish and sharks interacting in a two-dimensional grid, representing an ocean.

## Features
Fish and sharks are represented by different cell types in the ocean grid.
Fish breed after a certain age, and sharks can eat fish to survive.
Sharks can starve if they don't find food within a specified time.
Parallel processing with OpenMP is utilized to improve simulation performance.

## Dependencies
SFML library: Ensure that SFML is installed on your system.
OpenMP: The code uses OpenMP for parallel processing.

## Implemantation
The project must be implemented on Linux using the gcc compiler and documented with Doxygen. This simulation must use openMP for parallelization. If
you wish to run this on a GPU then either CUDA or openMP may be used. The
simulation must show a simple graphical output representing the population at
each step.

## Additional Information
For detailed documentation, refer to the Doxygen comments in the code.
For compiling help, refer to the Makefile 

## Project License:
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
