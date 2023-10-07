#include <SFML/Graphics.hpp>
#include <sstream>

using namespace sf;

void updateBranches(int seed);

const int NUM_BRANCHES = 6;

Sprite branches[NUM_BRANCHES];

enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main() {
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

    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    // Prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");

    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(0, 800);

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

    // is the bee currently moving?
    bool beeActive = false;

    float beeSpeed = 0.0f;

    // Clouds
    Texture textureCloud;
    textureCloud.loadFromFile("graphics/cloud.png");

    Sprite spriteCloud1;
    Sprite spriteCloud2;
    Sprite spriteCloud3;
    spriteCloud1.setTexture(textureCloud);
    spriteCloud2.setTexture(textureCloud);
    spriteCloud3.setTexture(textureCloud);

    spriteCloud1.setPosition(0, 0);
    spriteCloud2.setPosition(0, 250);
    spriteCloud3.setPosition(0, 500);

    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;

    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    // Variables to control time
    Clock clock;

    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;  // should this be const?
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

    Text messageText;
    Text scoreText;

    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    messageText.setFont(font);
    scoreText.setFont(font);

    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

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

    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");

    // set texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // set the origin of the sprite at (200, 20) from (0,0)
        branches[i].setOrigin(200, 20);
    }

    // Event object
    Event event;

    while (window.isOpen()) {
        /*
       ****************************
          Handle player input
       ****************************
       */
        // Event polling
        while (window.pollEvent(event)) {
            switch (event.type) {
                // Window closed
                case Event::Closed:
                    window.close();
                    break;

                // Escape pressed: exit
                case Event::KeyPressed:
                    if (event.key.code == Keyboard::Escape) {
                        window.close();
                    } else if (event.key.code == Keyboard::Return) {
                        paused = false;
                        // reset the time and the score
                        score = 0;
                        timeRemaining = 6;

                        // make all branches dissapear - starting in the second
                        // position
                    }
                    break;

                default:
                    break;
            }
        }

        /*
        ****************************
           Update the scene
        ****************************
        */
        if (!paused) {
            Time dt = clock.restart();

            // reduce time and resize timebar
            timeRemaining -= dt.asSeconds();
            timeBar.setSize(
                Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f) {
                paused = true;
                messageText.setString("Out of time!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
            }

            if (!beeActive) {
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200;

                srand((int)time(0));
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);

                beeActive = true;

            } else {
                spriteBee.setPosition(
                    spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
                    spriteBee.getPosition().y);

                // if the bee sprite as reached the left edge of the screen
                // set up a new bee for the next frame
                if (spriteBee.getPosition().x < -100) {
                    beeActive = false;
                }
            }

            // Managing the Clouds
            // Cloud 1
            if (!cloud1Active) {
                srand((int)time(0) * 10);
                cloud1Speed = (rand() % 200);

                srand((int)time(0) * 10);
                float height = (rand() % 150);
                spriteCloud1.setPosition(-200, height);
                cloud1Active = true;
            } else {
                spriteCloud1.setPosition(spriteCloud1.getPosition().x +
                                             (cloud1Speed * dt.asSeconds()),
                                         spriteCloud1.getPosition().y);
                if (spriteCloud1.getPosition().x > 1920) {
                    cloud1Active = false;
                }
            }

            // Cloud 2

            if (!cloud2Active) {
                srand((int)time(0) * 20);
                cloud2Speed = (rand() % 200);

                srand((int)time(0) * 20);
                float height = (rand() % 300) - 150;
                spriteCloud2.setPosition(-200, height);
                cloud2Active = true;
            } else {
                spriteCloud2.setPosition(spriteCloud2.getPosition().x +
                                             (cloud2Speed * dt.asSeconds()),
                                         spriteCloud2.getPosition().y);
                if (spriteCloud2.getPosition().x > 1920) {
                    cloud2Active = false;
                }
            }
            // Cloud 3

            if (!cloud3Active) {
                srand((int)time(0) * 30);
                cloud3Speed = (rand() % 200);
                srand((int)time(0) * 30);
                float height = (rand() % 450) - 150;
                spriteCloud3.setPosition(-200, height);
                cloud3Active = true;
            } else {
                spriteCloud3.setPosition(spriteCloud3.getPosition().x +
                                             (cloud3Speed * dt.asSeconds()),
                                         spriteCloud3.getPosition().y);
                if (spriteCloud3.getPosition().x > 1920) {
                    cloud3Active = false;
                }
            }

            std::stringstream ss;
            ss << "Score = " << score;  // "<<" is overloaded
            scoreText.setString(ss.str());

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++) {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT) {
                    branches[i].setPosition(610, height);

                    // flip the sprite 108
                    branches[i].setRotation(180);
                } else if (branchPositions[i] == side::RIGHT) {
                    branches[i].setPosition(1330, height);
                    branches[i].setRotation(0);
                } else {
                    // hide the branch
                    branches[i].setPosition(3000, height);
                }
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
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);

        for (int i; i < NUM_BRANCHES; i++) {
            window.draw(branches[i]);
        }

        window.draw(spriteTree);
        window.draw(spriteBee);

        window.draw(spritePlayer);
        window.draw(spriteAxe);
        window.draw(spriteLog);
        window.draw(spriteRIP);

        window.draw(scoreText);
        window.draw(timeBar);

        if (paused) {
            window.draw(messageText);
        }

        // Display the scene
        window.display();
    }

    return 0;
}

void updateBranches(int seed) {
    // move all branches down by one place
    for (int j = NUM_BRANCHES - 1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    // spawn a new branch at position 0
    srand((int)time(0) + seed);
    int r = (rand() % 5);  // random numbers between 0 and 4

    switch (r) {
        case 0:
            branchPositions[0] = side::LEFT;
            break;
        case 1:
            branchPositions[0] = side::RIGHT;
            break;
        default:
            branchPositions[0] = side::NONE;
            break;
    }
}
