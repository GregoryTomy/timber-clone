#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>

using namespace sf;

void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

const int NUM_CLOUDS = 3;
Sprite clouds[NUM_CLOUDS];
bool cloudActive[NUM_CLOUDS];
float cloudSpeed[NUM_CLOUDS];

enum class side
{
    LEFT,
    RIGHT,
    NONE
};
side branchPositions[NUM_BRANCHES];

int main()
{
    VideoMode vm(1920, 1080);
    RenderWindow window(vm, "Timber!!", Style::Default);

    // background
    Texture textureBackground;
    textureBackground.loadFromFile("graphics/background.png");

    Sprite spriteBackground;
    spriteBackground.setTexture(textureBackground);

    spriteBackground.setPosition(0, 0);

    // Tree
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree.png");
    Texture textureTree2;
    textureTree2.loadFromFile("graphics/tree2.png");

    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    Sprite spriteTree2;
    spriteTree2.setTexture(textureTree2);
    spriteTree2.setPosition(1600, -200);

    Sprite spriteTree3;
    spriteTree3.setTexture(textureTree2);
    spriteTree3.setPosition(200, -150);

    // Prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");

    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(-100, 800);

    // Prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);

    side playerSide = side::LEFT;

    // Gravestone
    Texture textureRIP;
    textureRIP.loadFromFile("graphics/rip.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(600, 860);

    Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(700, 830);

    // Line up the axe with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    // Prepare flying log
    Texture textureLog;
    textureLog.loadFromFile("graphics/log.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);

    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // Control player input
    bool acceptInput = false;

    // Player chopping sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    // Death sound
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/death.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    // out of time
    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    // is the bee currently moving?
    bool beeActive = false;

    float beeSpeed = 0.0f;

    // Clouds
    Texture textureCloud;
    textureCloud.loadFromFile("graphics/cloud.png");

    for (int i = 0; i < NUM_CLOUDS; ++i)
    {
        clouds[i].setTexture(textureCloud);
        clouds[i].setPosition(-1000, i * 250);
        cloudActive[i] = false;
        cloudSpeed[i] = 0.0f;
    }

    // Variables to control time
    Clock clock;

    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80; // should this be const?
    // why the Vector?
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Track whether the game is running
    bool paused = true;

    int score = 0;
    int lastScore = -1;

    Text messageText;
    Text scoreText;

    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    messageText.setFont(font);
    scoreText.setFont(font);

    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(75);

    messageText.setString("Press enter to start!");
    scoreText.setString("Score = 0");

    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // Position text
    FloatRect textRect = messageText.getLocalBounds();

    // change the origin to the center of textRect
    messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);

    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
    scoreText.setPosition(20, 20);

    RectangleShape scoreBackground;
    FloatRect scoreTextRect = scoreText.getLocalBounds();
    scoreBackground.setSize(
        Vector2f(scoreTextRect.width + 60, scoreTextRect.height + 20));
    scoreBackground.setFillColor(Color(0, 0, 0, 128));
    scoreBackground.setPosition(
        scoreText.getPosition().x + scoreTextRect.left - 10,
        scoreText.getPosition().y + scoreTextRect.top - 10);

    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");

    // set texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // set the origin of the sprite at (200, 20) from (0,0)
        branches[i].setOrigin(220, 20);
    }

    // Event object
    Event event;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::KeyReleased && !paused)
            {
                // Listen for key presses again
                acceptInput = true;

                // hide the axe
                spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
            }
        }

        /*
        ****************************
          Handle player input
        ****************************
        */
        if (Keyboard::isKeyPressed(Keyboard::Escape)) { window.close(); }

        if (Keyboard::isKeyPressed(Keyboard::Return))
        {
            paused = false;

            // Reset the time and the score
            score = 0;
            timeRemaining = 6;

            for (int i = 1; i < NUM_BRANCHES; i++)
            {
                branchPositions[i] = side::NONE;
            }

            spriteRIP.setPosition(675, 2000);
            spritePlayer.setPosition(580, 720);

            acceptInput = true;
        }

        if (acceptInput)
        {
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                playerSide = side::RIGHT;
                score++;
                timeRemaining += (2 / score) + 0.15;

                spriteAxe.setPosition(AXE_POSITION_RIGHT,
                                      spriteAxe.getPosition().y);
                spritePlayer.setPosition(1200, 720);

                updateBranches(score);

                spriteLog.setPosition(810, 720);
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                chop.play();
            }

            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                playerSide = side::LEFT;
                score++;
                timeRemaining += (2 / score) + 0.15;

                spriteAxe.setPosition(AXE_POSITION_LEFT,
                                      spriteAxe.getPosition().y);
                spritePlayer.setPosition(580, 720);

                updateBranches(score);

                spriteLog.setPosition(810, 720);
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

                chop.play();
            }
        }

        /*
        ****************************
           Update the scene
        ****************************
        */
        if (!paused)
        {
            Time dt = clock.restart();

            // reduce time and resize timebar
            timeRemaining -= dt.asSeconds();
            timeBar.setSize(
                Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f)
            {
                paused = true;
                messageText.setString("Out of time!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                outOfTime.play();
            }

            if (!beeActive)
            {
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200;

                srand((int)time(0));
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);

                beeActive = true;
            }
            else
            {
                spriteBee.setPosition(spriteBee.getPosition().x -
                                          (beeSpeed * dt.asSeconds()),
                                      spriteBee.getPosition().y);

                // if the bee sprite as reached the left edge of the screen
                // set up a new bee for the next frame
                if (spriteBee.getPosition().x < -100) { beeActive = false; }
            }

            // Managing the Clouds

            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                if (!cloudActive[i])
                {
                    srand((int)time(0) * 10 * (i + 1));
                    cloudSpeed[i] = (rand() % 200);

                    srand((int)time(0) * 10 * (i + 1));
                    float height = (rand() % 150) + i * 100;
                    clouds[i].setPosition(-200, height);
                    cloudActive[i] = true;
                }
                else
                {
                    clouds[i].setPosition(clouds[i].getPosition().x +
                                              (cloudSpeed[i] * dt.asSeconds()),
                                          clouds[i].getPosition().y);

                    if (clouds[i].getPosition().x > 1920)
                    {
                        cloudActive[i] = false;
                    }
                }
            }

            if (score != lastScore)
            {
                std::stringstream ss;
                ss << "Score = " << score; // "<<" is overloaded
                scoreText.setString(ss.str());

                lastScore = score;
            }

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT)
                {
                    branches[i].setPosition(610, height);
                    branches[i].setOrigin(220, 40);
                    // flip the sprite 108
                    branches[i].setRotation(180);
                }
                else if (branchPositions[i] == side::RIGHT)
                {
                    branches[i].setPosition(1330, height);
                    branches[i].setOrigin(220, 40);
                    branches[i].setRotation(0);
                }
                else
                {
                    // hide the branch
                    branches[i].setPosition(3000, height);
                }
            }

            // handle flying log
            if (logActive)
            {
                spriteLog.setPosition(
                    spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
                    spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

                if (spriteLog.getPosition().x < -100 ||
                    spriteLog.getPosition().y > 2000)
                {
                    logActive = false;
                    spriteLog.setPosition(810, 720);
                }
            }

            // player getting squished
            if (branchPositions[5] == playerSide)
            {
                paused = true;
                acceptInput = false;

                spriteRIP.setPosition(525, 760);
                spritePlayer.setPosition(2000, 660);

                messageText.setString("SQUISHED!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
                // need to hide gthe axe as well as set the RIP at the location
                // of getting squished
                death.play();
            }
        }

        /*
        ****************************
            Draw the scene
        ****************************
        */
        // Clear the window
        window.clear();

        // Draw your scene here
        window.draw(spriteBackground);
        // window.draw(spriteCloud1);
        // window.draw(spriteCloud2);
        // window.draw(spriteCloud3);
        for (int i = 0; i < NUM_CLOUDS; i++) { window.draw(clouds[i]); }

        for (int i = 0; i < NUM_BRANCHES; i++) { window.draw(branches[i]); }

        window.draw(spriteTree);
        window.draw(spriteTree3);
        window.draw(spriteTree2);
        window.draw(spriteBee);

        window.draw(spritePlayer);
        window.draw(spriteAxe);
        window.draw(spriteLog);
        window.draw(spriteRIP);

        window.draw(scoreBackground);
        window.draw(scoreText);
        window.draw(timeBar);

        if (paused) { window.draw(messageText); }

        // Display the scene
        window.display();
    }

    return 0;
}

void updateBranches(int seed)
{
    // move all branches down by one place
    for (int j = NUM_BRANCHES - 1; j > 0; j--)
    {
        branchPositions[j] = branchPositions[j - 1];
    }

    // spawn a new branch at position 0
    srand((int)time(0) + seed);
    int r = (rand() % 5); // random numbers between 0 and 4

    switch (r)
    {
    case 0: branchPositions[0] = side::LEFT; break;
    case 1: branchPositions[0] = side::RIGHT; break;
    default: branchPositions[0] = side::NONE; break;
    }
}

// void setPositionAndSpeed(Sprite &sprite, float &speed, float maxHeight)
