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

    int move (float dt, int pl_w, int pl_h, int pl_begin, Platform platform, Platform platformNet)
        { 
        int code = 0;

        x += vel.x*dt;
        y += vel.y*dt;

        if (x < 0)
            {
            x = -x;
            vel.x *= -1;
            }
        if (y < pl_begin)
            {
            if (platformNet.pos - platformNet.width/2 < x && x < platformNet.pos + platformNet.width/2)
                {
                vel.y *= -1;
                vel.x = 5*(platformNet.pos - x);
                }
            else
                {
                code = 1;
                }
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

int main ()
    {
    sf::RenderWindow window (sf::VideoMode (500, 750), "");
    window.setFramerateLimit (60);

    sf::Image image;
    image.loadFromFile ("textures.png");
    image.createMaskFromColor (sf::Color (0, 0, 255));
    sf::Texture txtr;
    txtr.loadFromImage (image);

    sf::Font font;
    font.loadFromFile ("Bulgaria_Glorious_Cyr.ttf");
    sf::Text score_text ("", font, 16);

    sf::Clock timer;

    Ball ball (sf::Vector2f (250, 500), sf::Vector2f (250, -250));
    
    Platform platform   (250, 700);
    Platform platformAI (250, 25);
    
    int score = 0, scoreAI = 0;
    int lives = 20, livesAI = 20;

    Network net ("network.txt");
    Network net2 ("network.txt");

    while (window.isOpen () && lives > 0 && livesAI > 0)
        {
        float dt = 3*timer.getElapsedTime ().asSeconds ();
        timer.restart ();


        sf::Event ev;

        while (window.pollEvent (ev))
            {
            if (ev.type == sf::Event::Closed)
                window.close ();

            timer.restart ();
            }


        if (sf::Keyboard::isKeyPressed (sf::Keyboard::D) && platform.pos  < 450)
            platform.pos += 250*dt;
        if (sf::Keyboard::isKeyPressed (sf::Keyboard::A) && platform.pos  > 50)
            platform.pos -= 250*dt;


        if (net.getOuput(0) > 0.6 && platformAI.pos  < 450)
            platformAI.pos += 250*dt;
        if (net.getOuput (0) < 0.4 && platformAI.pos  > 50)
            platformAI.pos -= 250*dt;

        if (net2.getOuput (0) < 0.4 && platform.pos  < 450)
            platform.pos += 250*dt;
        if (net2.getOuput (0) > 0.6 && platform.pos  > 50)
            platform.pos -= 250*dt;



        net.setInput (0, (platformAI.pos-50)/400);
        net.setInput (1, ball.x/500);
        
        net2.setInput (0, 1 - (platform.pos-50)/400);
        net2.setInput (1, 1 - ball.x/500);
        
        net.update ();
        net2.update ();

        
        int code = ball.move (dt, 500, 700, 50, platform, platformAI);
        if (code == 1)
            {
            score++;
            livesAI--; 
            
            ball.x = 500/2;
            ball.y = 650;
            ball.vel = sf::Vector2f (-500/4 + rand ()%(500/2+1), -250);
            }
        if (code == -1)
            {
            lives--;
            scoreAI++;

            ball.x = 500/2;
            ball.y = 100;
            ball.vel = sf::Vector2f (-500/4 + rand ()%(500/2+1), 250);
            }
        window.clear ();

        platform.draw (window);
        platformAI.draw (window);
        ball.draw (window, &txtr);

        std::ostringstream score_str;
        score_str << "Score: " << score;
        score_text.setString (score_str.str ());
        score_text.setPosition (200, 350);
        window.draw (score_text);
        
        std::ostringstream score_str_AI;
        score_str_AI << " ScoreAI: " << scoreAI;
        score_text.setString ("");
        score_text.setString (score_str_AI.str ());
        score_text.setPosition (200, 200);
        window.draw (score_text);


        for (int i = 0; i < lives; i++)
            {
            sf::Sprite heart_sprite;
            heart_sprite.setPosition (25*i, 725);
            heart_sprite.setTexture (txtr);
            heart_sprite.setTextureRect (sf::IntRect (0, 0, 50, 50));
            heart_sprite.scale (0.5f, 0.5f);
            window.draw (heart_sprite);
            }
        for (int i = 0; i < livesAI; i++)
            {
            sf::Sprite heart_sprite;
            heart_sprite.setPosition (25*i, 0);
            heart_sprite.setTexture (txtr);
            heart_sprite.setTextureRect (sf::IntRect (0, 0, 50, 50));
            heart_sprite.scale (0.5f, 0.5f);
            window.draw (heart_sprite);
            }

        window.display ();
        }


    while (window.isOpen ())
        {
        sf::Event ev;

        while (window.pollEvent (ev))
            {
            if (ev.type == sf::Event::Closed)
                window.close ();
            }

        window.clear ();

        if (livesAI > lives)
            {
            score_text.setString ("AI wins");
            score_text.setPosition (200, 370);
            }
        else
            {
            score_text.setString ("You are AI,\n aren't you?");
            score_text.setPosition (150, 370);
            }
        score_text.setCharacterSize (32);
        window.draw (score_text);
        window.display ();
        }


    return 0;
    }