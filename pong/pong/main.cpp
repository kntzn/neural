#include <SFML/Graphics.hpp>
#include "Network.h"
#include <ctime>

struct Player
    {
    sf::Vector2f ballPosition = sf::Vector2f (250, 250);
    sf::Vector2f ballVelocity = sf::Vector2f (-125, -250);
    sf::RectangleShape platform;

    Network net = Network (3, 3, 1);

    float platformPosition = 250;

    double score = 0;

    bool alive = true;

    Player ()
        {
        platform.setSize (sf::Vector2f (100, 25));
        platform.setOrigin (sf::Vector2f (50, 0));
        }
    };

int main ()
    {
    srand (time (0));

    sf::RenderWindow window (sf::VideoMode (500, 550), "");
    
#define POPULATION 50

    Player players [POPULATION];

    sf::Clock timer;
    while (window.isOpen ())
        {
        float dt = 5*timer.getElapsedTime ().asSeconds ();
        timer.restart ();

        // -------------------------
        bool newGeneration = true;

        for (int i = 0; i < POPULATION; i++)
            {
            printf ("%d", players[i].alive);

            if (players [i].alive)
                newGeneration = false;

            players [i].net.setInput (0, players [i].platformPosition);
            players [i].net.setInput (1, players [i].ballPosition.x);
            players [i].net.setInput (2, players [i].ballPosition.y);

            players [i].net.update ();

            if (players [i].alive)
                if (players [i].net.getOuput (0) > 0.6 && players [i].platformPosition < 450)
                    players [i].platformPosition += 250*dt;
            if (players [i].alive)
                if (players [i].net.getOuput (0) < 0.4 && players [i].platformPosition > 50)
                    players [i].platformPosition -= 250*dt;
            // -------------------------

            players [i].ballPosition += players [i].ballVelocity*dt;

            if (players [i].ballPosition.x < 0)
                {
                players [i].ballPosition.x = -players [i].ballPosition.x;
                players [i].ballVelocity.x *= -1;
                }
            if (players [i].ballPosition.y < 0)
                {
                players [i].ballPosition.y = -players [i].ballPosition.y;
                players [i].ballVelocity.y *= -1;

                players [i].score++;
                }
            if (players [i].ballPosition.x > 500)
                {
                players [i].ballPosition.x = 1000 - players [i].ballPosition.x;
                players [i].ballVelocity.x *= -1;
                }
            if (players [i].ballPosition.y > 500)
                {
                if (players [i].platformPosition-50 < players [i].ballPosition.x && players [i].ballPosition.x < players [i].platformPosition+50)
                    {
                    if (players [i].alive)
                        {
                        players [i].ballVelocity.y *= -1;
                        players [i].ballVelocity.x = 5*(players [i].platformPosition - players [i].ballPosition.x);
                        }
                    }
                else
                    {
                    if (players [i].alive)
                        players [i].score -= fabs (players [i].platformPosition - players [i].ballPosition.x)/400;

                    players [i].alive = false;
                    }
                }
            }
        
        printf ("\n");
        // ------------------------

        if (newGeneration)
            {
            for (int j = 0; j < POPULATION; j++)
                for (int k = 0; k < POPULATION-1; k++)
                    {
                    if (players [k].score < players [k+1].score)
                        std::swap (players [k], players [k+1]);
                    }


            for (int i = 0; i < POPULATION; i++)
                printf ("%lg\n", players [i].score);


            bool copyFlag = false;
            int lastOriginal = 0;
            for (int i = 0; i < POPULATION; i++)
                {
                if (i == 0)
                    players [i].platform.setFillColor (sf::Color::Green);
                else
                    players [i].platform.setFillColor (sf::Color::White);

                if (players [i].score < 1)
                    {
                    players [i].net.mutate (50, 0.3);
                    }
                }

            for (int i = 0; i < POPULATION; i++)
                {
                players [i].score = 0;
                players [i].net.mutate (5, 0.2);
                players [i].alive = true;
                players [i].ballPosition = sf::Vector2f (250, 250);
                players [i].ballVelocity = sf::Vector2f (-125, -250);
                players [i].platformPosition = 250;
                }

            }

        window.clear ();

        sf::CircleShape ball;
        ball.setRadius (5);
        ball.setOrigin (5, 5);
        for (int i = POPULATION-1; i >= 0; i--)
            {
            ball.setPosition (players [i].ballPosition);
            window.draw (ball);

            players [i].platform.setPosition (sf::Vector2f (players [i].platformPosition, 500));
            
            if (players [i].alive)
                window.draw (players [i].platform);
            }

        window.display ();
        }

    return 0;
    }