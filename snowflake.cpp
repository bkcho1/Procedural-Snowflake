#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <time.h>

const float ALPHA = 1.0f;
const float BETA = 0.8f;
const float GAMMA = 0.0f;

struct cell
{
    float u;
    float v;
    float s = BETA;
    bool receptive = false;
};

int main()
{

    const float RAD_3 = sqrt(3);
    const float CELL_RADIUS = 10.0f;
    const float AREA = ((3 * RAD_3) / 2) * CELL_RADIUS * CELL_RADIUS;
    const int GRID_WIDTH = 70;
    const int GRID_HEIGHT = 70;
    const int MAX_GENERATIONS = 61;

    int count = 0;
    int delay = 0;
    bool isPlaying = false;

    // initialize 2D arrays
    cell *grid[GRID_HEIGHT];
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        grid[i] = new cell[GRID_WIDTH];
    }

    // center
    grid[GRID_HEIGHT / 2][GRID_WIDTH / 2].s = 1.0f;
    grid[GRID_HEIGHT / 2][GRID_WIDTH / 2].receptive = true;
    // top
    grid[(GRID_HEIGHT / 2) - 1][GRID_WIDTH / 2].receptive = true;
    grid[(GRID_HEIGHT / 2) - 1][(GRID_WIDTH / 2) + 1].receptive = true;
    // mid
    grid[GRID_HEIGHT / 2][(GRID_WIDTH / 2) - 1].receptive = true;
    grid[GRID_HEIGHT / 2][(GRID_WIDTH / 2) + 1].receptive = true;
    // bot
    grid[(GRID_HEIGHT / 2) + 1][GRID_WIDTH / 2].receptive = true;
    grid[(GRID_HEIGHT / 2) + 1][(GRID_WIDTH / 2) + 1].receptive = true;

    // initialize u and v values for grid
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (grid[i][j].receptive)
            {
                grid[i][j].u = 0.0f;
                grid[i][j].v = grid[i][j].s;
            }
            else
            {
                grid[i][j].u = grid[i][j].s;
                grid[i][j].v = 0.0f;
            }
        }
    }

    cell *next_grid[GRID_HEIGHT];
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        next_grid[i] = new cell[GRID_WIDTH];
    }

    // x and y value for the window is calculated this way in order to ensure that ALL hexagons are displayed
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * 2 * (CELL_RADIUS * (RAD_3 / 2)), GRID_HEIGHT * CELL_RADIUS * 1.5 + CELL_RADIUS * 0.5), "Snowflake!", sf::Style::Default, sf::ContextSettings(0, 0, 8));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::P)
                {
                    isPlaying = !isPlaying;
                }
                break;
            }
        }

        window.clear(sf::Color::Black);
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            for (int x = 0; x < GRID_WIDTH; x++)
            {
                sf::CircleShape hex;
                if (y % 2 == 0)
                {
                    hex.setPosition(x * 2 * (CELL_RADIUS * (RAD_3 / 2)), y * CELL_RADIUS * 1.5);
                }
                else
                {
                    hex.setPosition((CELL_RADIUS * (RAD_3 / 2)) + x * 2 * (CELL_RADIUS * (RAD_3 / 2)), y * CELL_RADIUS * 1.5);
                }
                hex.setPointCount(6);
                hex.setRadius(CELL_RADIUS);
                hex.setOutlineThickness(1);
                hex.setOutlineColor(sf::Color(153, 153, 153));
                /*
                beta value has to be at least 1.0 for a cell to be 'frozen'
                */

                if (grid[y][x].receptive)
                {
                    hex.setFillColor(sf::Color(153, 153, 153));
                    if (grid[y][x].s >= 1.0f)
                    {
                        hex.setFillColor(sf::Color::White);
                    }
                    if (grid[y][x].s >= 1.1f)
                    {
                        hex.setFillColor(sf::Color(173, 216, 230));
                    }
                    window.draw(hex);
                }

                // set up next frame
                if (isPlaying && (x != 0 && y != 0 && x != GRID_WIDTH && y != GRID_HEIGHT))
                {
                    // constant addition
                    if (grid[y][x].receptive)
                    {
                        next_grid[y][x].v = grid[y][x].v + GAMMA;
                    }

                    // diffusion calculations
                    float sum;
                    float avg;
                    if (y % 2 == 1)
                    {
                        sum = grid[y - 1][x].u + grid[y - 1][x + 1].u + grid[y][x + 1].u + grid[y][x - 1].u + grid[y + 1][x].u + grid[y + 1][x + 1].u;
                    }
                    else
                    {
                        sum = grid[y - 1][x].u + grid[y - 1][x - 1].u + grid[y][x + 1].u + grid[y][x - 1].u + grid[y + 1][x].u + grid[y + 1][x - 1].u;
                    }
                    avg = sum / 6.0f;
                    next_grid[y][x].u = grid[y][x].u + (ALPHA / 2.0f) * (avg - grid[y][x].u);

                    next_grid[y][x].s = next_grid[y][x].u + next_grid[y][x].v;

                    // insert/update new data into next_grid
                    if (grid[y][x].receptive)
                    {
                        next_grid[y][x].receptive = true;
                        next_grid[y][x].u = 0.0f;
                        next_grid[y][x].v = next_grid[y][x].s;
                        next_grid[y][x].s = next_grid[y][x].s;
                    }

                    if (next_grid[y][x].s >= 1.0f)
                    {
                        // center
                        next_grid[y][x].receptive = true;
                        //  top
                        next_grid[y - 1][x].receptive = true;
                        //  mid
                        next_grid[y][x - 1].receptive = true;
                        next_grid[y][x + 1].receptive = true;
                        // bot
                        next_grid[y + 1][x].receptive = true;
                        if (y % 2 == 1)
                        {
                            // top
                            next_grid[y - 1][x + 1].receptive = true;
                            // bot
                            next_grid[y + 1][x + 1].receptive = true;
                        }
                        else
                        {
                            // top
                            next_grid[y - 1][x - 1].receptive = true;
                            // bot
                            next_grid[y + 1][x - 1].receptive = true;
                        }
                    }
                }
            }
        }

        if (isPlaying)
        {
            for (int y = 0; y < GRID_HEIGHT; y++)
            {
                for (int x = 0; x < GRID_WIDTH; x++)
                {
                    grid[y][x].s = next_grid[y][x].s;
                    grid[y][x].u = next_grid[y][x].u;
                    grid[y][x].v = next_grid[y][x].v;
                    grid[y][x].receptive = next_grid[y][x].receptive;
                }
            }
        }

        window.display();
        sf::sleep(sf::milliseconds(delay));
    }
}