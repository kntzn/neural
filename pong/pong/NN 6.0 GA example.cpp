#include <SFML/Graphics.hpp>
#include <sstream>
#include "Network.h"

struct Platform
    {
    sf::RectangleShape platform;
    
    float pos = 250;
    float width = 100;
    float level = 500;

    Platform (float position, float height)
        { 
        pos = position;
        level = height;
        platform.setSize (sf::Vector2f (100, 25)); 
        platform.setOrigin (sf::Vector2f (width/2, 0));
        }

    void draw (sf::RenderWindow &window)
        { 
        platform.setPosition (pos, level);
        window.draw (platform);
        }

    };

struct Ball: public sf::Vector2f
    {
    sf::Vector2f vel = sf::Vector2f (-125+rand ()%251, -250);

    Ball (sf::Vector2f pos, sf::Vector2f velocity)
        { 
        x = pos.x;
        y = pos.y;

        vel = velocity;
        }

    int move (float dt, int pl_w, int pl_h, Platform platform)
        { 
        int code = 0;

        x += vel.x*dt;
        y += vel.y*dt;

        if (x < 0)
            {
            x = -x;
            vel.x *= -1;
            }
        if (y < 0)
            {
            vel.y *= -1;
            y = -y;

            code = 1;
            }
        if (x > pl_w)
            {
            x = 2*pl_w - x;
            vel.x *= -1;
            }
        if (y > pl_h)
            {
            if (platform.pos - platform.width/2 < x && x < platform.pos + platform.width/2)
                {
                vel.y *= -1;
                vel.x = 5*(platform.pos - x);
                }
            else
                {
                code = -1;
                }
            }

        return code;
        }

    void draw (sf::RenderWindow &window, sf::Texture* txtr)
        { 
        sf::CircleShape ball;
        ball.setRadius (12);
        ball.setOrigin (12, 12);
        ball.setTexture (txtr);
        ball.setPosition (x, y);
        ball.setTextureRect (sf::IntRect (50, 0, 50, 50));
        window.draw (ball);
        }

    };

struct Player
    {
    Ball ball = Ball (sf::Vector2f (250, 500), sf::Vector2f (250, -250));

    Platform platform = Platform (250, 500);

    Network net = Network (2, 3, 1);

    float score = 0;

    bool alive = true;
    };


int main ()
    {
    sf::RenderWindow window (sf::VideoMode (500, 550), "");

    sf::Image image;
    image.loadFromFile ("textures.png");
    image.createMaskFromColor (sf::Color (0, 0, 255));
    sf::Texture txtr;
    txtr.loadFromImage (image);

    sf::Clock timer;

    const int N_AI = 50;
    const int maxScore = 100;
    const float platformSpeed = 250;

    
    Player AIs [N_AI] = { };

    while (window.isOpen ())
        {
        // Sfml timer (not used while learning)
        float dt = 0.016f; //3*timer.getElapsedTime ().asSeconds ();
        //timer.restart ();

        // Sfml events
        sf::Event ev;
        while (window.pollEvent (ev))
            {
            if (ev.type == sf::Event::Closed)
                window.close ();

            timer.restart ();
            }

        // Networks
        for (int i = 0; i < N_AI; i++)
            {
            AIs [i].net.setInput (0, 1 - (AIs [i].platform.pos-50)/400);
            AIs [i].net.setInput (1, 1 - AIs [i].ball.x/500);

            AIs [i].net.update ();

            if (AIs [i].net.getOuput (0) < 0.4 && AIs [i].platform.pos  < 450)
                AIs [i].platform.pos += platformSpeed*dt;
            if (AIs [i].net.getOuput (0) > 0.6 && AIs [i].platform.pos  > 50)
                AIs [i].platform.pos -= platformSpeed*dt;
            }

        // Physics
        bool reachedMaxScore = false;
        bool allDied = true;

        for (int i = 0; i < N_AI; i++)
            {
            if (AIs [i].score >= maxScore)
                reachedMaxScore = true;

            if (AIs [i].alive)
                {
                allDied = false;

                int code = AIs [i].ball.move (dt, 500, 500, AIs [i].platform);
                if (code == 1)
                    {
                    AIs [i].score++;
                    }
                if (code == -1)
                    {
                    AIs [i].alive = false;
                    AIs [i].ball.x = 500/2;
                    AIs [i].ball.y = 500;
                    AIs [i].ball.vel = sf::Vector2f (-500/4 + rand ()%(500/2+1), -250);
                    AIs [i].score -= fabs (AIs [i].platform.pos - AIs [i].ball.x)/450;
                    }
                }
            }

        // Logs
        for (int i = 0; i < N_AI; i++)
            printf ("%d", AIs [i].alive);
        printf ("\n");

        // Saving best network
        if (sf::Keyboard::isKeyPressed (sf::Keyboard::Space))
            for (int i = 0; i < N_AI; i++)
                {
                if (AIs [i].alive)
                    {
                    AIs [i].net.saveToFile ("network.txt");
                    break;
                    }
                }

        // New generation case
        if (allDied || reachedMaxScore)
            {
            // Sortng AIs by their scores using bubblesort (sorry)
            for (int i = 0; i < N_AI; i++)
                for (int j = 0; j < N_AI-1; j++)
                    if (AIs [j].score < AIs [j+1].score)
                        std::swap (AIs [j], AIs [j+1]);


            if (reachedMaxScore)
                {
                int nReachedMaxScore = 0;
                for (int i = 0; i < N_AI; i++)
                    {
                    if (AIs [i].score >= maxScore)
                        nReachedMaxScore++;

                    // If net hasn't reached maxScore it is being copied from net that has reached it (and being mutated also)
                    if (AIs [i].score < maxScore)
                        {
                        AIs [i].net = Network (AIs [i%nReachedMaxScore].net, AIs [i%nReachedMaxScore].net);
                        AIs [i].net.mutate (5, 0.1);
                        }
                    }

                if (nReachedMaxScore == N_AI)
                    {
                    AIs [0].net.saveToFile ("topNet");
                    }
                }
            else // all died
                {
                for (int i = 0; i < N_AI; i++)
                    {
                    // If net hitted the ball at least 1 time, we would mutate it moderately
                    if (AIs [i].score > 1)
                        AIs [i].net.mutate (5, 0.1);
                    // Otherwise net is wery bad and we should try to fix it with hard mutations
                    else
                        AIs [i].net.mutate (50, 0.5);

                    }
                }

            for (int i = 0; i < N_AI; i++)
                {
                AIs [i].score = 0;
                AIs [i].alive = true;
                }
            }

        // Graphics
        window.clear ();
        for (int i = N_AI-1; i >= 0; i--)
            {
            if (i == 0)
                AIs [i].platform.platform.setFillColor (sf::Color::Cyan);
            else
                AIs [i].platform.platform.setFillColor (sf::Color::White);

            if (AIs [i].alive)
                {
                AIs [i].platform.draw (window);
                AIs [i].ball.draw (window, &txtr);
                }
            }


        window.display ();
        }
    return 0;
    }