#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

using namespace sf;

const int MAX_NUMBER_BRANCHES = 6;
const int MAX_NUMBER_CLOUDS = 3;
const int MAX_NUMBER_LOGS = 20;
const float timebarReminder = 6.0f;

enum class side {LEFT, RIGHT, NONE};
side playerPosition, branchesPosition[MAX_NUMBER_BRANCHES];

enum class state {START, PLAYING, DEAD, TIMEOVER};
state gameState;

void updateBranches(int);
void setBranches(const side &branchPositon, const Sprite &spTree, int i, Sprite &spBranch);
void updateDecorative(const RenderWindow &window, bool &active, float &height, int heightRand, int heightBase, float &speed, int speedRand, int speedBase, float dt, Sprite &sprite);
void setStateMessage(const RenderWindow &window, String msg, Text &text);
void initializeGame(state &gameState, side &playerPosition, side branchesPosition[], RectangleShape &timebar, Vector2f timebarSize, float &timeRemaining, int &scores, bool &acceptInput, bool &hit, bool &activeBee, bool activeClouds[]);

int main() {
    // make window
    VideoMode vm(1920, 1080);
    RenderWindow window(vm, "Timber!", Style::Fullscreen);

    // make textures
    Texture txtrBackground, txtrBee, txtrCloud, txtrTree, txtrPlayer, txtrBranch, txtrLog, txtrAxe, txtrGravestone;
    txtrBackground.loadFromFile("assets/graphics/background.png");
    txtrTree.loadFromFile("assets/graphics/tree.png");
    txtrBee.loadFromFile("assets/graphics/bee.png");
    txtrCloud.loadFromFile("assets/graphics/cloud.png");
    txtrPlayer.loadFromFile("assets/graphics/player.png");
    txtrBranch.loadFromFile("assets/graphics/branch.png");
    txtrLog.loadFromFile("assets/graphics/log.png");
    txtrAxe.loadFromFile("assets/graphics/axe.png");
    txtrGravestone.loadFromFile("assets/graphics/rip.png");

    // make sprites
    Sprite spBackground, spBee, spTree, spPlayer, spClouds[MAX_NUMBER_CLOUDS], spBranches[MAX_NUMBER_BRANCHES], spLog, spAxe, spGravestone;
    spBackground.setTexture(txtrBackground);
    spBackground.setPosition(0,0);
    spTree.setTexture(txtrTree);
    spTree.setPosition(
        window.getSize().x/2 - txtrTree.getSize().x/2,
        0);
    spBee.setTexture(txtrBee);
    spBee.setPosition(window.getSize().x/2 + 200, window.getSize().y/2);
    spPlayer.setTexture(txtrPlayer);
    FloatRect pRect = spPlayer.getGlobalBounds();
    spPlayer.setOrigin(
        pRect.left + pRect.width/2,
        pRect.top + pRect.height/2
    );
    spPlayer.setPosition(
        spTree.getPosition().x + spTree.getLocalBounds().width + 10,
        spTree.getLocalBounds().height - spPlayer.getLocalBounds().height
    );

    for(int i=0; i< MAX_NUMBER_CLOUDS ; i++) {
        spClouds[i].setTexture(txtrCloud);
        spClouds[i].setPosition(0, i*spClouds[i].getLocalBounds().height);
    }

    for(int i=0; i < MAX_NUMBER_BRANCHES; i++) {
        spBranches[i].setTexture(txtrBranch);
        spBranches[i].setOrigin(
            spBranches[i].getLocalBounds().width/2,
            spBranches[i].getLocalBounds().height/2
        );
        spBranches[i].setRotation(180);
        spBranches[i].setPosition(
            spTree.getPosition().x - spBranches[i].getLocalBounds().width/2,
            i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranches[i].getLocalBounds().height/2
            );
    }

    spLog.setTexture(txtrLog);
    FloatRect logRect = spLog.getLocalBounds();
    spLog.setOrigin(
        logRect.width/2,
        logRect.height/2);
    spLog.setPosition(window.getSize().x + spLog.getLocalBounds().width, spPlayer.getGlobalBounds().top + spPlayer.getLocalBounds().height/2);

    spAxe.setTexture(txtrAxe);
    FloatRect axeRect = spAxe.getLocalBounds();
    spAxe.setOrigin(
        axeRect.width/2,
        axeRect.height/2
    );
    spAxe.setPosition(spPlayer.getGlobalBounds().left - spAxe.getGlobalBounds().width/2, spPlayer.getGlobalBounds().top + spPlayer.getGlobalBounds().height/2);

    // make HUD - Score and State messages
    Font font;
    font.loadFromFile("assets/fonts/KOMIKAP_.ttf");
    Text txtScore, txtStates;
    txtScore.setFont(font);
    txtScore.setFillColor(Color::White);
    txtScore.setPosition(20, 20);
    txtScore.setString("Score: 0");
    txtScore.setCharacterSize(75);

    txtStates.setFont(font);
    txtStates.setFillColor(Color::White);
    setStateMessage(window, "Press Enter to start!", txtStates);
    
    // make a time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400, timeBarHeight = 80, timeRemaining = 400;
    Vector2f timebarSize = Vector2f(timeBarStartWidth, timeBarHeight);
    timeBar.setSize(timebarSize);
    timeBar.setFillColor(Color::Red);
    FloatRect treeRect = spTree.getLocalBounds();
    timeBar.setPosition(
        window.getSize().x/2 - timeBarStartWidth/2,
        (window.getSize().y + treeRect.top + treeRect.height) / 2 - timeBarHeight/2
    );

    // Timer
    Clock clock;

    // Variables for representing states
    bool activeBee, activeClouds[MAX_NUMBER_CLOUDS], acceptInput, hit, axeDisplay=false;
    float beeHeight, beeSpeed, cloudsHeight[MAX_NUMBER_CLOUDS], cloudsSpeed[MAX_NUMBER_CLOUDS];
    int scores;
    Event event;

    initializeGame(gameState, playerPosition, branchesPosition, timeBar, timebarSize, timeRemaining, scores, acceptInput, hit, activeBee, activeClouds);

    // game loop
    while(window.isOpen()) {
        // input here
        // check if the key is released
        while(window.pollEvent(event)) {
            if(event.type == Event::KeyReleased && gameState == state::PLAYING) {
                acceptInput = true;
                axeDisplay = false;
            }
        }

        if(Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Return)) {
            initializeGame(gameState, playerPosition, branchesPosition, timeBar, timebarSize, timeRemaining, scores, acceptInput, hit, activeBee, activeClouds);
            gameState = state::PLAYING;
        }

        if(acceptInput) {
            if(Keyboard::isKeyPressed(Keyboard::Left)) {
                playerPosition = side::LEFT;
                hit = true;
                axeDisplay = true;
                acceptInput = false;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Right)) {
                playerPosition = side::RIGHT;
                hit = true;
                axeDisplay = true;
                acceptInput = false;
            }
        }

        // update
        Time deltaTime = clock.restart();
        float dt = deltaTime.asSeconds();

        if(gameState == state::PLAYING) {
            // update decorative objects: bee and clouds
            updateDecorative(window, activeBee, beeHeight, 500, 500, beeSpeed, 200, 200, dt, spBee);
            for(int i=0; i<MAX_NUMBER_CLOUDS; i++) {
                updateDecorative(window, activeClouds[i], cloudsHeight[i], 150, 10 * i, cloudsSpeed[i], 200, 0, dt, spClouds[i]);
            }

            // update the result of the input
            if(hit) {
                // let positions of branches down
                for(int i = MAX_NUMBER_BRANCHES-1; i > 0; i--) {
                    std::cout << "branch #" << i << " update... " << std::endl;
                    branchesPosition[i] = branchesPosition[i-1];
                }

                // make new branch at the top
                updateBranches(scores);

                bool isSquished = playerPosition == branchesPosition[5];
                if(!isSquished && hit) {
                    // add +1 score
                    scores++;

                    timeRemaining += 2.0f/(float)scores + 15.0f;

                    // set log sprite, its speed and its direction

                }
                else {
                    gameState = state::DEAD;
                    // replace player sprite with gravestone sprite
                }

                hit = false;
            }

            // update player
            Vector2f pScale_ = spPlayer.getScale();
            if(playerPosition == side::LEFT) {
                if(pScale_.x == 1) spPlayer.setScale(-1,1);
                spPlayer.setPosition(
                    spTree.getPosition().x - spPlayer.getLocalBounds().width/2 - 10,
                    spTree.getLocalBounds().height - spPlayer.getLocalBounds().height/2
                );
            }
            else if (playerPosition == side::RIGHT) {
                if(pScale_.x == -1) spPlayer.setScale(1,1);
                spPlayer.setPosition(
                    spTree.getPosition().x + spTree.getLocalBounds().width +spPlayer.getLocalBounds().width/2 + 10,
                    spTree.getLocalBounds().height - spPlayer.getLocalBounds().height/2
                );
            }

            // update branches
            for(int i = 0; i< MAX_NUMBER_BRANCHES; i++) {
                setBranches(branchesPosition[i], spTree, i, spBranches[i]);
            }

            // update log animation 2 cases
            if(axeDisplay) {
                FloatRect pRect = spPlayer.getGlobalBounds();
                FloatRect aRect = spAxe.getGlobalBounds();
                float Ax_, Ay_;
                Vector2f scale_ = spAxe.getScale();

                switch(playerPosition) {
                    case side::LEFT:
                        // set position for left
                        Ax_ = pRect.left + pRect.width + aRect.width/2;
                        Ay_ = pRect.top + pRect.height/2;
                        if(scale_.x == 1) spAxe.setScale(-1, 1);
                        spAxe.setPosition(Vector2f(Ax_, Ay_));
                        break;
                    case side::RIGHT:
                        // set position for right
                        Ax_ = pRect.left - aRect.width/2;
                        Ay_ = pRect.top + pRect.height/2;
                        if(scale_.x == -1) spAxe.setScale(1, 1);
                        spAxe.setPosition(Vector2f(Ax_, Ay_));
                        break;
                }
            }
            else {
                // set position for hiding axe sprite
                spAxe.setPosition(2000,0);
            }

            // update timeBar
            timeRemaining -= timeBarStartWidth / timebarReminder * dt;
            timeBar.setSize(Vector2f(timeRemaining, timeBarHeight));
            if(timeRemaining <= 0) {
                gameState = state::TIMEOVER;
            }

            // update scoreboard
            std::stringstream ss;
            ss << "Score: " << scores;
            txtScore.setString(ss.str());

            // update state message
            switch(gameState) {
                case state::PLAYING:
                    setStateMessage(window, "", txtStates);
                    break;
                case state::DEAD:
                    setStateMessage(window, "You are dead...", txtStates);
                    break;
                case state::TIMEOVER:
                    setStateMessage(window, "Out of time!", txtStates);
                    break;
                default:
                    setStateMessage(window, "Press Enter to start!", txtStates);
                    break;
            }
        }

        // render
        // Background and decorative graphics
        window.draw(spBackground);
        window.draw(spBee);
        for(int i=0; i < MAX_NUMBER_CLOUDS;i++) {
            window.draw(spClouds[i]);
        }

        // main game objects
        window.draw(spTree);
        for(int i=0; i < MAX_NUMBER_BRANCHES;i++) {
            window.draw(spBranches[i]);
        }
        window.draw(spLog);
        window.draw(spAxe);
        window.draw(spPlayer);

        // HUD
        window.draw(txtScore);
        
        window.draw(txtStates);
        window.draw(timeBar);

        window.display();
    }

    return 0;
}

void updateBranches(int seed) {
    srand(seed);
    int temp_ = rand() % 5;

    switch(temp_) {
        case 1:
            branchesPosition[0] = side::LEFT;
            break;
        case 2:
            branchesPosition[0] = side::RIGHT;
            break;
        default:
            branchesPosition[0] = side::NONE;
    }
}

void setBranches(const side &branchPosition, const Sprite &spTree, int i, Sprite &spBranch) {
    if(branchesPosition[i] == side::LEFT) {
        spBranch.setRotation(180);
        spBranch.setPosition(
            spTree.getPosition().x - spBranch.getLocalBounds().width/2,
            i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranch.getLocalBounds().height/2
        );
    }
    else if (branchesPosition[i] == side::RIGHT) {
        spBranch.setRotation(0);
        
        spBranch.setPosition(
            spTree.getPosition().x + spTree.getLocalBounds().width + spBranch.getLocalBounds().width/2,
            i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranch.getLocalBounds().height/2
        );
    }
    else {
        spBranch.setPosition(
            -2000,
            i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranch.getLocalBounds().height/2
        );
    }
}

void updateDecorative(const RenderWindow &window, bool &active, float &height, int heightRand, int heightBase, float &speed, int speedRand, int speedBase, float dt, Sprite &sprite) {
    if(!active) {
        srand((int)time(0));
        height = (rand() % heightRand) + heightBase;
        
        srand((int)time(0) * height);
        speed = (rand() % speedRand) + speedBase;

        float x_ = window.getSize().x + sprite.getLocalBounds().width;
        float y_ = height;

        std::cout << "(x, y) of sprite: " << x_ << ", " << y_ << std::endl;
        sprite.setPosition(x_, y_);

        active = true;
    }
    else {
        float location = sprite.getPosition().x - speed * dt;

        bool boundaryCheck = location < -sprite.getLocalBounds().width;
        if(boundaryCheck) {
            active = false;
        }
        else {
            sprite.setPosition(location, height);
        }
    }
}

void setStateMessage(const RenderWindow &window, String msg, Text &text) {
    text.setString(msg);
    text.setCharacterSize(100);
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(
        textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f
    );
    text.setPosition(window.getSize().x / 2, window.getSize().y / 2);
}

void initializeGame(state &gameState, side &playerPosition, side branchesPosition[], RectangleShape &timebar, Vector2f timebarSize, float &timeRemaining, int &scores, bool &acceptInput, bool &hit, bool &activeBee, bool activeClouds[]) {
    gameState = state::START;

    activeBee = false;
    for(int i=0; i<MAX_NUMBER_CLOUDS; i++) {
        activeClouds[i] = false;
    }

    // initialize positions
    playerPosition = side::RIGHT;
    for(int i = 0; i < MAX_NUMBER_BRANCHES; i++) {
        if (i == 0) {
            updateBranches((int)time(0));
            continue;
        }
        branchesPosition[i] = side::NONE;
    }

    timebar.setSize(timebarSize);
    timeRemaining = timebarSize.x;

    scores = 0;

    acceptInput = false;
    hit = false;
}