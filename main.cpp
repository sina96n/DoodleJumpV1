#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include "point.h"

using namespace std;
using namespace sf;
using namespace std::this_thread;

// function declarations
void set_seed(int n);
void game_board();
void load_textures();
void load_font();
void setup_game_environment();
void draw_platforms();
void init();
void move();
void reset_platform_positioins();
void game_over();
void step_on_platform();
void rearrange_environment();
void run();
int level();
void writeFile(char username[]);
void conclusion();


// Global variables
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 650;
const int RIGHT_MOVEMENT = 0.5f;
const int LEFT_MOVEMENT = 33.0f;
const int NUM_PLATFORMS = 10;
const int PLATFORM_WIDTH = 68;
const int PLATFORM_HEIGHT = 14;
// game window
RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DoodleJumpV1");
// game textures
Texture backgroundTexture, platformTexture, doodleTexture, gameoverTexture;
Sprite background, platform, doodle, gameover;
// text font
Font font;
Text scoreText;

point platforms[NUM_PLATFORMS];
// initial position
int x = 100, y = 100, h = 200;
float dx = 0, dy = 0;

enum class GameState { Menu, Playing, GameOver };
GameState gameState = GameState::Menu;

RectangleShape startButton(Vector2f(200, 50));
Text buttonText("Start", font, 30);

int score = 0;
char levels[5][15] = { 
    "Level 1", "Level 2", "Level 3", "Level 4", "Level 5" 
};
fstream f("Scores.txt", ios::app | ios::out);


int main()
{
    // load game and setup environment
    init();
    // play background music in a loop
    Music backgroundMusic;
    backgroundMusic.openFromFile("music\\filem.ogg");
    backgroundMusic.setLoop(true);
    backgroundMusic.play();

    load_font();

    startButton.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 25);
    startButton.setFillColor(Color::Green);

    buttonText.setPosition(startButton.getPosition().x + 50, startButton.getPosition().y + 5);


    // main game loop
    while (app.isOpen()) {
        switch (gameState)
        {
        case GameState::Menu:
        {
            Event e;
            while (app.pollEvent(e)) {
                if (e.type == Event::Closed)
                    app.close();
                else if (e.type == Event::MouseButtonPressed) {
                    if (e.mouseButton.button == Mouse::Left) {
                        Vector2i mousePos = Mouse::getPosition(app);
                        Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                        if (startButton.getGlobalBounds().contains(mousePosF)) {
                            gameState = GameState::Playing;
                        }
                    }
                }
            }

            app.clear();
            app.draw(background);
            app.draw(startButton);
            app.draw(buttonText);
            app.display();
        }
        break;

        case GameState::Playing:
            run();
            break;

        case GameState::GameOver:
            // handle game over screen input here if needed
            break;
        }
    }
    
    // console output after exitting game
    conclusion();

    return 0;
}


void set_seed(int n) {
    srand(n);
}


void game_board(){
    cout << "DoodleJumpV1" << endl << endl;
    cout << "Implementing DoodleJump in Cpp using SFML library" << endl;

    cout << "\n\nMovements:\n";
    cout << "1. <- and -> keys form horizontal movement\n" << endl;


    cout << "The game will start in 5 seconds\n";

    flush(cout);
}


void load_textures() {
    // all the textures can be easily changed
    if (!backgroundTexture.loadFromFile("images\\background.png"))
        std::cout << "Failed to load background texture." << std::endl;
    if (!platformTexture.loadFromFile("images\\platform.png"))
        std::cout << "Failed to load platform texture." << std::endl;
    if (!doodleTexture.loadFromFile("images\\doodle.png"))
        std::cout << "Failed to load doodle texture." << std::endl;
    if (!gameoverTexture.loadFromFile("images/gameover.jpg"))
        std::cout << "Failed to load gameover texture." << std::endl;
}

void load_font() {
    if (!font.loadFromFile("fonts\\arial.ttf")) {
        cout << "Failed to load font." << endl;
    }
}


void setup_game_environment() {
    // settifng fps to 30
    app.setFramerateLimit(30);
    // loading texture for objects
    load_textures();
    // initializing platforms
    for (int i = 0; i < 10; i++){
        platforms[i].x = rand() % WINDOW_WIDTH;
        platforms[i].y = rand() % WINDOW_HEIGHT;
    }

    app.draw(scoreText);
    // settinf textures for Sprites
    background.setTexture(backgroundTexture);
    platform.setTexture(platformTexture);
    doodle.setTexture(doodleTexture);
}

void draw_platforms() {
    for (int i = 0; i < 10; i++)
    {
        platform.setPosition(platforms[i].x, platforms[i].y);
        app.draw(platform);
    }
}

void init() {
    // setting seed for random platform placement
    set_seed(time(0));
    // user manual
    game_board();
    // game starter chronometer
    sleep_until(std::chrono::system_clock::now() + chrono::seconds(5));

    setup_game_environment();

    gameState = GameState::Menu;
}


void move() {
    if (Keyboard::isKeyPressed(Keyboard::Right)) x += 3;
    if (Keyboard::isKeyPressed(Keyboard::Left)) x -= 3;
}


void reset_platform_positioins() {
    for (int i = 0; i < 10; i++)
    {
        y = h;
        platforms[i].y = platforms[i].y - dy;
        if (platforms[i].y > WINDOW_HEIGHT) {
            platforms[i].y = 0;
            platforms[i].x = rand() % (WINDOW_WIDTH - 80) + 40;
        }
    }
}

void game_over() {
    dy = -10;
    cout << "\nGAME OVER!\n\n";
    gameover.setTexture(gameoverTexture);
    app.draw(gameover);
    app.close();
}

void step_on_platform() {
    for (int i = 0; i < NUM_PLATFORMS; i++) {
        if ((x + 50 > platforms[i].x)                   && 
            (x + 20 < platforms[i].x + PLATFORM_WIDTH)  && 
            (y + 70 > platforms[i].y)                   && 
            (y + 70 < platforms[i].y + PLATFORM_HEIGHT) && 
            (dy > 0)) 
        {

            dy = -10;
            // raise score based on stepping on platform
            score++;
        }
    }
}

void rearrange_environment() {
    app.draw(background);
    app.draw(doodle);

    draw_platforms();

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Score: " + to_string(score));
    app.draw(scoreText);
}

void run()
{
    switch (gameState)
    {
    case GameState::Menu:
        app.draw(background);
        app.draw(startButton);
        app.draw(buttonText);
        break;

    case GameState::Playing:

        // read player movements
        move();

        dy += 0.2;
        y += dy;
        // loose condition
        if (y > WINDOW_HEIGHT - 33)
            game_over();
        // reset platform postions while moving upwards
        if (y < h)
            reset_platform_positioins();
        // check for landing
        step_on_platform();
        // update doodle position
        doodle.setPosition(x, y);
        rearrange_environment();


        app.draw(scoreText);
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
            else if (e.type == Event::MouseButtonPressed)
            {
                if (e.mouseButton.button == Mouse::Left)
                {
                    Vector2f mousePos = app.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });
                    if (startButton.getGlobalBounds().contains(mousePos))
                    {
                        gameState = GameState::Playing;
                    }
                }
            }
        }
        break;

    case GameState::GameOver:
        // your existing code for the game over screen
        break;
    }

    app.display();
}


int level() {
    if (score > 500) return 5;
    else if (score > 200) return 4;
    else if (score > 80) return 3;
    else if (score > 20) return 2;
    else return 1;
}

void writeFile(char username[]) {
    f << username << ":" << score << "-" << levels[level() - 1] << endl;
}

void conclusion() {
    char save_flag[2], name[15];
    cout << "Achived score: " << score << endl;
    cout << "Achived level: " << levels[level() - 1] << endl << endl;
    cout << "Do you want to save your score? (y/n) : ";
    cin >> save_flag;
    if (save_flag[0] == 'y') {
        cout << "\nUsername:";
        cin >> name;
        writeFile(name);
        cout << "sucessfully saved.";
    }
    f.close();
}


