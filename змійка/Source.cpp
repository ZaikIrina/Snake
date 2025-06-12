#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <sstream>
#include <fstream>

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CELL_SIZE = 20;
const int moveSpeed = 20;
const int TOP_PANEL_HEIGHT = 40;

enum Direction { STOP, LEFT, RIGHT, UP, DOWN };
Direction dir = STOP;

bool isMouseOver(RectangleShape& button, Vector2i mousePos) {
    return button.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

bool isPositionFree(const deque<CircleShape>& snake, float x, float y) {
    for (const auto& segment : snake) {
        if (segment.getPosition() == Vector2f(x, y)) {
            return false;
        }
    }
    return true;
}

CircleShape createApple(float x, float y) {
    CircleShape apple(CELL_SIZE / 2);
    apple.setFillColor(Color(207, 17, 0));
    apple.setPosition(x, y);
    return apple;
}

CircleShape createSnakeSegment(float x, float y) {
    CircleShape segment(CELL_SIZE / 2);
    segment.setFillColor(Color(3, 157, 252));
    segment.setPosition(x, y);
    return segment;
}

int loadHighScore() {
    ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

void saveHighScore(int highScore) {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void resetGame(deque<CircleShape>& snake, CircleShape& apple, bool& gameOver, bool& snakeIsMoving, int& score, int& highScore) {
    snake.clear();
    snake = {
        createSnakeSegment(300, 300 + TOP_PANEL_HEIGHT),
        createSnakeSegment(280, 300 + TOP_PANEL_HEIGHT),
        createSnakeSegment(260, 300 + TOP_PANEL_HEIGHT)
    };

    do {
        apple.setPosition(
            CELL_SIZE * (rand() % (WINDOW_WIDTH / CELL_SIZE)),
            TOP_PANEL_HEIGHT + CELL_SIZE * (rand() % ((WINDOW_HEIGHT - TOP_PANEL_HEIGHT) / CELL_SIZE))
        );
    } while (!isPositionFree(snake, apple.getPosition().x, apple.getPosition().y));

    dir = STOP;
    gameOver = false;
    snakeIsMoving = false;
    score = 0;
}

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game");
    window.setFramerateLimit(10);

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:\\Users\\IRA\\OneDrive\\Документы\\Програмування\\змійка\\fon.png")) {
        cerr << "Failed to load background image!" << endl;
        return -1;
    }

    Sprite menuBackground(backgroundTexture);
    menuBackground.setScale(
        float(WINDOW_WIDTH) / backgroundTexture.getSize().x,
        float(WINDOW_HEIGHT) / backgroundTexture.getSize().y
    );

    Sprite gameBackground(backgroundTexture);
    gameBackground.setScale(
        float(WINDOW_WIDTH) / backgroundTexture.getSize().x,
        float(WINDOW_HEIGHT) / backgroundTexture.getSize().y
    );

    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        cerr << "Failed to load font!" << endl;
        return -1;
    }

    Color buttonColor(255, 106, 0);
    Color textColor(3, 157, 252);

    Text title("SNAKE GAME", font, 60);
    title.setFillColor(textColor);
    title.setPosition(WINDOW_WIDTH / 2 - title.getGlobalBounds().width / 2, 50);

    RectangleShape playButton(Vector2f(200, 50));
    playButton.setFillColor(buttonColor);
    playButton.setPosition(WINDOW_WIDTH / 2 - 100, 250);

    Text playText("Play", font, 30);
    playText.setFillColor(textColor);
    playText.setPosition(
        WINDOW_WIDTH / 2 - playText.getGlobalBounds().width / 2,
        260
    );

    RectangleShape exitButton(Vector2f(200, 50));
    exitButton.setFillColor(buttonColor);
    exitButton.setPosition(WINDOW_WIDTH / 2 - 100, 330);

    Text exitText("Exit", font, 30);
    exitText.setFillColor(textColor);
    exitText.setPosition(
        WINDOW_WIDTH / 2 - exitText.getGlobalBounds().width / 2,
        340
    );

    RectangleShape restartButton(Vector2f(200, 50));
    restartButton.setFillColor(buttonColor);
    restartButton.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 60);  // Змінено позицію

    Text restartText("Restart", font, 30);
    restartText.setFillColor(textColor);
    restartText.setPosition(
        WINDOW_WIDTH / 2 - restartText.getGlobalBounds().width / 2,
        WINDOW_HEIGHT / 2 + 70  // Змінено позицію
    );

    RectangleShape menuButton(Vector2f(200, 50));
    menuButton.setFillColor(buttonColor);
    menuButton.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 130);  // Змінено позицію

    Text menuText("Main Menu", font, 30);
    menuText.setFillColor(textColor);
    menuText.setPosition(
        WINDOW_WIDTH / 2 - menuText.getGlobalBounds().width / 2,
        WINDOW_HEIGHT / 2 + 140  // Змінено позицію
    );

    bool gameStarted = false;
    bool snakeIsMoving = false;
    bool gameOver = false;
    int score = 0;
    int highScore = loadHighScore();

    deque<CircleShape> snake = {
        createSnakeSegment(300, 300 + TOP_PANEL_HEIGHT),
        createSnakeSegment(280, 300 + TOP_PANEL_HEIGHT),
        createSnakeSegment(260, 300 + TOP_PANEL_HEIGHT)
    };

    CircleShape apple = createApple(400, 300 + TOP_PANEL_HEIGHT);

    RectangleShape topPanel(Vector2f(WINDOW_WIDTH, TOP_PANEL_HEIGHT));
    topPanel.setFillColor(Color(50, 50, 50));
    topPanel.setPosition(0, 0);

    Text scoreText("Score: 0", font, 20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    Text highScoreText("Record: " + to_string(highScore), font, 20);
    highScoreText.setFillColor(Color::Yellow);
    highScoreText.setPosition(WINDOW_WIDTH - 150, 10);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);

                if (!gameStarted) {
                    if (isMouseOver(playButton, mousePos)) {
                        gameStarted = true;
                    }
                    else if (isMouseOver(exitButton, mousePos)) {
                        window.close();
                    }
                }
                else if (gameOver) {
                    if (isMouseOver(restartButton, mousePos)) {
                        resetGame(snake, apple, gameOver, snakeIsMoving, score, highScore);
                    }
                    else if (isMouseOver(menuButton, mousePos)) {
                        gameStarted = false;
                        gameOver = false;
                        resetGame(snake, apple, gameOver, snakeIsMoving, score, highScore);
                    }
                }
            }

            if (gameStarted && !gameOver && event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Right && dir != LEFT) {
                    dir = RIGHT;
                    snakeIsMoving = true;
                }
                else if (event.key.code == Keyboard::Left && dir != RIGHT) {
                    dir = LEFT;
                    snakeIsMoving = true;
                }
                else if (event.key.code == Keyboard::Up && dir != DOWN) {
                    dir = UP;
                    snakeIsMoving = true;
                }
                else if (event.key.code == Keyboard::Down && dir != UP) {
                    dir = DOWN;
                    snakeIsMoving = true;
                }
            }
        }

        window.clear();

        if (!gameStarted) {
            window.draw(menuBackground);
            window.draw(title);
            window.draw(playButton);
            window.draw(playText);
            window.draw(exitButton);
            window.draw(exitText);
        }
        else {
            window.draw(gameBackground);
            window.draw(topPanel);

            stringstream ss;
            ss << "Score: " << score;
            scoreText.setString(ss.str());
            window.draw(scoreText);

            highScoreText.setString("Record: " + to_string(highScore));
            window.draw(highScoreText);

            if (!gameOver) {
                Vector2f newHeadPos = snake.front().getPosition();

                if (snakeIsMoving) {
                    if (dir == RIGHT) newHeadPos.x += moveSpeed;
                    if (dir == LEFT) newHeadPos.x -= moveSpeed;
                    if (dir == UP) newHeadPos.y -= moveSpeed;
                    if (dir == DOWN) newHeadPos.y += moveSpeed;

                    // Перевіряємо колізію з межами екрану
                    if (newHeadPos.x < 0 || newHeadPos.x >= WINDOW_WIDTH ||
                        newHeadPos.y < TOP_PANEL_HEIGHT || newHeadPos.y >= WINDOW_HEIGHT) {
                        gameOver = true;
                        if (score > highScore) {
                            highScore = score;
                            saveHighScore(highScore);
                        }
                    }

                    // Перевіряємо колізію з собою
                    for (size_t i = 1; i < snake.size(); ++i) {
                        if (snake[i].getPosition() == newHeadPos) {
                            gameOver = true;
                            if (score > highScore) {
                                highScore = score;
                                saveHighScore(highScore);
                            }
                            break;
                        }
                    }

                    if (!gameOver) {
                        snake.push_front(createSnakeSegment(newHeadPos.x, newHeadPos.y));

                        if (snake.front().getGlobalBounds().intersects(apple.getGlobalBounds())) {
                            do {
                                apple.setPosition(
                                    CELL_SIZE * (rand() % (WINDOW_WIDTH / CELL_SIZE)),
                                    TOP_PANEL_HEIGHT + CELL_SIZE * (rand() % ((WINDOW_HEIGHT - TOP_PANEL_HEIGHT) / CELL_SIZE))
                                );
                            } while (!isPositionFree(snake, apple.getPosition().x, apple.getPosition().y));

                            score++;
                            if (score > highScore) {
                                highScore = score;
                                saveHighScore(highScore);
                            }
                        }
                        else {
                            snake.pop_back();
                        }
                    }
                }
            }

            for (const auto& segment : snake) {
                window.draw(segment);
            }

            window.draw(apple);

            if (gameOver) {
                RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                overlay.setFillColor(Color(0, 0, 0, 150));
                window.draw(overlay);

                Text gameOverText("GAME OVER", font, 50);
                gameOverText.setFillColor(Color::Red);
                gameOverText.setPosition(
                    WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2,
                    WINDOW_HEIGHT / 2 - 100
                );
                window.draw(gameOverText);

                Text scoreDisplayText("Apples eaten: " + to_string(score), font, 30);
                scoreDisplayText.setFillColor(Color::White);
                scoreDisplayText.setPosition(
                    WINDOW_WIDTH / 2 - scoreDisplayText.getGlobalBounds().width / 2,
                    WINDOW_HEIGHT / 2 - 30
                );
                window.draw(scoreDisplayText);

                Text recordDisplayText("Record: " + to_string(highScore), font, 25);
                recordDisplayText.setFillColor(Color::Yellow);
                recordDisplayText.setPosition(
                    WINDOW_WIDTH / 2 - recordDisplayText.getGlobalBounds().width / 2,
                    WINDOW_HEIGHT / 2 + 10
                );
                window.draw(recordDisplayText);

                window.draw(restartButton);
                window.draw(restartText);
                window.draw(menuButton);
                window.draw(menuText);
            }
        }

        window.display();
    }

    return 0;
}