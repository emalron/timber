#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

const int MAX_NUMBER_BRANCHES = 6;
const int MAX_NUMBER_CLOUDS = 3;
const int MAX_NUMBER_LOGS = 20;
const float timebarReminder = 6.0f;

bool loadTextures();

enum class side {LEFT, RIGHT, NONE};

int main() {
    // make window
    VideoMode vm(1920, 1080);
    RenderWindow window(vm, "Timber!", Style::Fullscreen);

    // make textures
    Texture txtrBackground, txtrBee, txtrCloud, txtrTree, txtrPlayer, txtrBranch, txtrLog;
    txtrBackground.loadFromFile("assets/graphics/background.png");
    txtrTree.loadFromFile("assets/graphics/tree.png");
    txtrBee.loadFromFile("assets/graphics/bee.png");
    txtrCloud.loadFromFile("assets/graphics/cloud.png");
    txtrPlayer.loadFromFile("assets/graphics/player.png");
    txtrBranch.loadFromFile("assets/graphics/branch.png");
    txtrLog.loadFromFile("assets/graphics/log.png");

    // make sprites
    Sprite spBackground, spBee, spTree, spPlayer, spClouds[MAX_NUMBER_CLOUDS], spBranches[MAX_NUMBER_BRANCHES], spLogs[MAX_NUMBER_LOGS];
    spBackground.setTexture(txtrBackground);
    spBackground.setPosition(0,0);
    spTree.setTexture(txtrTree);
    spTree.setPosition(
        window.getSize().x/2 - txtrTree.getSize().x/2,
        0);
    spBee.setTexture(txtrBee);
    spBee.setPosition(window.getSize().x/2 + 200, window.getSize().y/2);
    spPlayer.setTexture(txtrPlayer);
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

    for(int i=0; i<MAX_NUMBER_LOGS; i++) {
        spLogs[i].setTexture(txtrLog);
        spLogs[i].setPosition(i * spLogs[i].getGlobalBounds().width, 2000);        
    }

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
    txtStates.setString("Press Enter to start");
    txtStates.setCharacterSize(100);
    FloatRect textRect = txtStates.getLocalBounds();
    txtStates.setOrigin(
        textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f
    );
    txtStates.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    
    // make a time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    FloatRect treeRect = spTree.getLocalBounds();
    timeBar.setPosition(
        window.getSize().x/2 - timeBarStartWidth/2,
        (window.getSize().y + treeRect.top + treeRect.height) / 2 - timeBarHeight/2
    );

    // Timer
    Clock clock;

    // Variables for representing states
    bool activeBee, activeClouds[MAX_NUMBER_CLOUDS];
    float beeHeight, beeSpeed, cloudsHeight[MAX_NUMBER_CLOUDS], cloudsSpeed[MAX_NUMBER_CLOUDS];
    activeBee = false;
    for(int i=0; i<MAX_NUMBER_CLOUDS; i++) {
        activeClouds[i] = false;
    }

    side playerPosition, branchesPosition[MAX_NUMBER_BRANCHES];
    playerPosition = side::RIGHT;
    srand((int)time(0) * 10);
    for(int i = 0; i < MAX_NUMBER_BRANCHES; i++) {
        if (i == 0) {
            int temp_ = rand() % 2;
            switch(temp_) {
                case 0:
                    branchesPosition[i] = side::LEFT;
                    break;
                case 1:
                    branchesPosition[i] = side::RIGHT;
                    break;
            }

            continue;
        }
        branchesPosition[i] = side::NONE;
    }

    bool acceptInput = false;
    Event event;

    // game loop
    while(window.isOpen()) {
        // input here

        while(window.pollEvent(event)) {
            if(event.type == Event::KeyReleased) {
                acceptInput = true;
            }
        }
        if(Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        if(acceptInput) {
            if(Keyboard::isKeyPressed(Keyboard::Return)) {
                // down the position of branches
                for(int i = MAX_NUMBER_BRANCHES-1; i > 0; i--) {
                    std::cout << "branch #" << i << " update... " << std::endl;
                    branchesPosition[i] = branchesPosition[i-1];
                }

                // make new branch
                srand((int) time(0) * 20);
                int temp_ = rand() % 10;
                switch(temp_) {
                    case 0:
                    case 2:
                    case 4:
                    case 6:
                        branchesPosition[0] = side::LEFT;
                        break;
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                        branchesPosition[0] = side::RIGHT;
                        break;
                    default:
                        branchesPosition[0] = side::NONE;
                }

                acceptInput = false;
            }
        }

        // update
        Time dt = clock.restart();
        // update bee
        if(!activeBee) {
            srand((int)time(0));
            beeHeight = (rand() % 500) + 500;
            
            srand((int)time(0) * 10);
            beeSpeed = (rand() % 200) + 200;
            spBee.setPosition(window.getSize().x + spBee.getGlobalBounds().width, beeHeight);

            activeBee = true;
        }
        else {
            float beeLocation = spBee.getPosition().x - beeSpeed * dt.asSeconds();

            if(beeLocation < -spBee.getLocalBounds().width) {
                activeBee = false;
            }
            else {
                spBee.setPosition(beeLocation, beeHeight);
            }
        }
        // update clouds
        for(int i=0; i<MAX_NUMBER_CLOUDS; i++) {
            if(!activeClouds[i]) {
                srand((int)time(0) * i*10);
                cloudsHeight[i] = rand() % 150 + 10 * i;
                
                srand((int)time(0) * i*10);
                cloudsSpeed[i] = rand() % 200;
                spClouds[i].setPosition(window.getSize().x - spClouds[i].getGlobalBounds().width, cloudsHeight[i]);

                activeClouds[i] = true;
            }
            else {
                float cloudLocation = spClouds[i].getPosition().x - cloudsSpeed[i] * dt.asSeconds();

                if(cloudLocation < -spClouds[i].getLocalBounds().width) {
                    activeClouds[i] = false;
                }
                else {
                    spClouds[i].setPosition(cloudLocation, cloudsHeight[i]);
                }
            }
        }
        // update branches
        for(int i = 0; i< MAX_NUMBER_BRANCHES; i++) {
            if(branchesPosition[i] == side::LEFT) {
                spBranches[i].setRotation(180);
                spBranches[i].setPosition(
                    spTree.getPosition().x - spBranches[i].getLocalBounds().width/2,
                    i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranches[i].getLocalBounds().height/2
                );
            }
            else if (branchesPosition[i] == side::RIGHT) {
                spBranches[i].setRotation(0);
                
                spBranches[i].setPosition(
                    spTree.getPosition().x + spTree.getLocalBounds().width + spBranches[i].getLocalBounds().width/2,
                    i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranches[i].getLocalBounds().height/2
                );
            }
            else {
                spBranches[i].setPosition(
                    -2000,
                    i * spTree.getLocalBounds().height/MAX_NUMBER_BRANCHES + spBranches[i].getLocalBounds().height/2
                );
            }
        }

        // update timeBar
        float currentTimebarWidth = timeBar.getLocalBounds().width - timeBarStartWidth / timebarReminder * dt.asSeconds();
        timeBar.setSize(Vector2f(currentTimebarWidth, timeBarHeight));
        if(currentTimebarWidth <= 0) {
            txtStates.setString("Game over");
            FloatRect textR = txtStates.getLocalBounds();
            txtStates.setOrigin(
                textR.width / 2,
                textR.height/2
            );
        }

        // render
        window.draw(spBackground);
        window.draw(spBee);
        window.draw(spTree);
        for(int i=0; i < MAX_NUMBER_BRANCHES;i++) {
            window.draw(spBranches[i]);
        }
        window.draw(spPlayer);

        for(int i=0; i < MAX_NUMBER_CLOUDS;i++) {
            window.draw(spClouds[i]);
        }
        
        for(int i=0; i< MAX_NUMBER_LOGS; i++) {
            window.draw(spLogs[i]);
        }

        window.draw(txtScore);
        window.draw(txtStates);
        window.draw(timeBar);


        window.display();
    }

    return 0;
}

bool loadTextures() {
    
}