#include <SFML/Graphics.hpp>
#include <sstream>

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

class Ball: public sf::Vector2f
    {
    private:
        sf::Vector2f vel = sf::Vector2f (-125+rand ()%251, -250);

    public:
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
                y = -y;
                vel.y *= -1;
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

                    x = pl_w/2;
                    y = pl_h;
                    vel = sf::Vector2f (-pl_w/4 + rand ()%(pl_w/2+1), -vel.y);
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
    sf::RenderWindow window (sf::VideoMode (500, 550), "");
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
    
    Platform platform (250, 500);
    
    int score = 0;
    int lives = 10;

    while (window.isOpen () && lives > 0)
        {
        float dt = 3*timer.getElapsedTime ().asSeconds ();
        timer.restart ();

        if (sf::Keyboard::isKeyPressed (sf::Keyboard::D) && platform.pos  < 450)
            platform.pos += 250*dt;
        if (sf::Keyboard::isKeyPressed (sf::Keyboard::A) && platform.pos  > 50)
            platform.pos -= 250*dt;

        int code = ball.move (dt, 500, 500, platform);
        if (code == 1)
            score++;
        else if (code == -1)
            lives--;

        window.clear ();

        platform.draw (window);
        ball.draw (window, &txtr);

        std::ostringstream score_str;
        score_str << "Score: " << score;
        score_text.setString (score_str.str ());
        score_text.setPosition (200, 200);
        window.draw (score_text);

        for (int i = 0; i < lives; i++)
            {
            sf::Sprite heart_sprite;
            heart_sprite.setPosition (25*i, 525);
            heart_sprite.setTexture (txtr);
            heart_sprite.setTextureRect (sf::IntRect (0, 0, 50, 50));
            heart_sprite.scale (0.5f, 0.5f);
            window.draw (heart_sprite);
            }
        
        window.display ();
        }
    
    return 0;
    }