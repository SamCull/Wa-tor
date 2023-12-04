#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>

const int gridSize = 100;
const int cellSize = 5;
const int reproductionTime = 50;  // Adjust the reproduction time as needed

class Entity {
public:
    int x, y;
    bool isEaten;
    int survivalTime;

    Entity() {
        x = rand() % gridSize;
        y = rand() % gridSize;
        isEaten = false;
        survivalTime = 0;
    }

    void move() {
        // Move randomly within the boundaries
        x = std::max(0, std::min(gridSize - 1, x + rand() % 3 - 1));
        y = std::max(0, std::min(gridSize - 1, y + rand() % 3 - 1));

        // Increment survival time
        ++survivalTime;

        // Reproduction
        if (!isEaten && survivalTime >= reproductionTime) {
            // Leave a new fish in the old position
            Entity newFish;
            newFish.x = x;
            newFish.y = y;
            preyVector.push_back(newFish);

            // Reset survival time
            survivalTime = 0;
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(gridSize * cellSize, gridSize * cellSize), "Prey and Predator Simulation");

    // Seed for random number generation
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int numPrey = 10;  // Adjust the number of prey as needed
    int numPredators = 5;  // Adjust the number of predators as needed

    std::vector<Entity> preyVector(numPrey);
    std::vector<Entity> predatorVector(numPredators);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Move prey and check for collisions
        for (auto& prey : preyVector) {
            prey.move();
        }

        // Move predators and check for collisions
        for (auto& predator : predatorVector) {
            predator.move();

            // Check for collisions (predator eats prey)
            for (auto& prey : preyVector) {
                if (!prey.isEaten && predator.x == prey.x && predator.y == prey.y) {
                    // Predator eats the prey
                    prey.isEaten = true;
                }
            }
        }

        // Remove eaten prey
        preyVector.erase(std::remove_if(preyVector.begin(), preyVector.end(), [](const Entity& prey) {
            return prey.isEaten;
        }), preyVector.end());

        window.clear();

        // Draw prey
        for (const auto& prey : preyVector) {
            sf::RectangleShape preyShape(sf::Vector2f(cellSize, cellSize));
            preyShape.setFillColor(sf::Color::Green);
            preyShape.setPosition(prey.x * cellSize, prey.y * cellSize);
            window.draw(preyShape);
        }

        // Draw predators
        for (const auto& predator : predatorVector) {
            sf::RectangleShape predatorShape(sf::Vector2f(cellSize, cellSize));
            predatorShape.setFillColor(sf::Color::Red);
            predatorShape.setPosition(predator.x * cellSize, predator.y * cellSize);
            window.draw(predatorShape);
        }

        window.display();

        // Introduce a delay to slow down the simulation
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed
    }

    return 0;
}
