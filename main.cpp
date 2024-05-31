#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BASKET_SIZE = 50; // resized basket size
const int GRADE_SIZE = 30;
const float SPAWN_INTERVAL = 1.0f; // time between grade spawns (seconds)
const int GAME_DURATION = 60; // game duration (seconds)
const int WINNING_SCORE = 20; // expected score to win

class Grade : public sf::Sprite {
public:
    int value;
    sf::Vector2f velocity;

    Grade(int val, sf::Texture& texture, sf::Vector2f pos, sf::Vector2f vel) : value(val), velocity(vel) {
        setTexture(texture);
        setPosition(pos);
        setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
        setScale(0.2f, 0.2f); // resize grade to fit on screen
    }

    void moveGrade(const sf::Time &elapsed) {
        move(velocity * elapsed.asSeconds());
    }
};

class Basket : public sf::Sprite {
public:
    float velocityX = 0;
    float velocityY = 0;
    float rotationSpeed = 0;

    Basket(sf::Texture& texture, sf::Vector2f position) {
        setTexture(texture);
        setPosition(position);
        setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
        setScale(0.2f, 0.2f); // resize basket to fit on screen
    }

    void setSpeed(float x, float y, float r) {
        velocityX = x;
        velocityY = y;
        rotationSpeed = r;
    }

    void moveInDirection(const sf::Time &elapsed) {
        move(velocityX * elapsed.asSeconds(), velocityY * elapsed.asSeconds());
        rotate(rotationSpeed * elapsed.asSeconds());

        // Keep basket within window bounds
        sf::Vector2f pos = getPosition();
        if (pos.x < getTexture()->getSize().x / 2 * 0.2f) pos.x = getTexture()->getSize().x / 2 * 0.2f;
        if (pos.x > WINDOW_WIDTH - getTexture()->getSize().x / 2 * 0.2f) pos.x = WINDOW_WIDTH - getTexture()->getSize().x / 2 * 0.2f;
        if (pos.y < getTexture()->getSize().y / 2 * 0.2f) pos.y = getTexture()->getSize().y / 2 * 0.2f;
        if (pos.y > WINDOW_HEIGHT - getTexture()->getSize().y / 2 * 0.2f) pos.y = WINDOW_HEIGHT - getTexture()->getSize().y / 2 * 0.2f;
        setPosition(pos);
    }
};

sf::Vector2f getRandomEdgePosition() {
    int side = rand() % 4;
    switch (side) {
    case 0: return sf::Vector2f(rand() % WINDOW_WIDTH, 0); // Top
    case 1: return sf::Vector2f(rand() % WINDOW_WIDTH, WINDOW_HEIGHT); // Bottom
    case 2: return sf::Vector2f(0, rand() % WINDOW_HEIGHT); // Left
    case 3: return sf::Vector2f(WINDOW_WIDTH, rand() % WINDOW_HEIGHT); // Right
    }
    return sf::Vector2f(0, 0);
}

sf::Vector2f getRandomVelocity() {
    float angle = rand() % 360;
    float speed = 100 + rand() % 200;
    return sf::Vector2f(speed * cos(angle * 3.14159265359 / 180), speed * sin(angle * 3.14159265359 / 180));
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Grade Catcher Game");

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("/Users/riya.mathur/grade_catcher/class.png")) {
        std::cerr << "Failed to load background image!" << std::endl;
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(WINDOW_WIDTH / backgroundTexture.getSize().x, WINDOW_HEIGHT / backgroundTexture.getSize().y);

    sf::Texture basketTexture;
    if (!basketTexture.loadFromFile("/Users/riya.mathur/grade_catcher/basket.png")) {
        cout << "Failed to load player texture!" << endl;
        return -1;
    }

    Basket basket(basketTexture, sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2)); // center the basket
    basket.setSpeed(300, 300, 200);

    vector<sf::Texture> gradeTextures(5);
    for (int i = 0; i < 5; ++i) {
        if (!gradeTextures[i].loadFromFile("/Users/riya.mathur/grade_catcher/" + to_string(i + 1) + ".png")) {
            cout << "Failed to load grade texture " << (i + 1) << "!" << endl;
            return -1;
        }
    }

    vector<Grade> grades;
    float timeSinceLastSpawn = 0;
    sf::Clock clock;
    sf::Clock gameClock;
    int score = 0;
    int lowGradesCaught = 0;

    sf::Font font;
    if (!font.loadFromFile("/Users/riya.mathur/grade_catcher/OpenSans-Regular.ttf")) {
        cout << "Failed to load font!" << endl;
        return -1;
    }
    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::Black);
    timerText.setPosition(10, 10);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(10, 40);

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        timeSinceLastSpawn += elapsed.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            basket.velocityY = -300;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            basket.velocityY = 300;
        } else {
            basket.velocityY = 0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            basket.velocityX = -300;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            basket.velocityX = 300;
        } else {
            basket.velocityX = 0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            basket.rotationSpeed = -200;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            basket.rotationSpeed = 200;
        } else {
            basket.rotationSpeed = 0;
        }

        basket.moveInDirection(elapsed);

        if (timeSinceLastSpawn >= SPAWN_INTERVAL) {
            timeSinceLastSpawn = 0;
            int gradeValue = 1 + rand() % 5;
            sf::Vector2f pos = getRandomEdgePosition();
            sf::Vector2f vel = getRandomVelocity();
            grades.push_back(Grade(gradeValue, gradeTextures[gradeValue - 1], pos, vel));
        }

        for (auto &grade : grades) {
            grade.moveGrade(elapsed);
        }

        auto it = grades.begin();
        while (it!= grades.end()) {
            if (it->getGlobalBounds().intersects(basket.getGlobalBounds())) {
                score += it->value;
                if (it->value < 3) {
                    lowGradesCaught++;
                }
                it = grades.erase(it);
            } else if (it->getPosition().x < 0 || it->getPosition().x > WINDOW_WIDTH || it->getPosition().y < 0 || it->getPosition().y > WINDOW_HEIGHT) {
                it = grades.erase(it);
            } else {
                ++it;
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(basket);
        for (const auto &grade : grades) {
            window.draw(grade);
        }

        int remainingTime = GAME_DURATION - gameClock.getElapsedTime().asSeconds();
        timerText.setString("Time: " + to_string(remainingTime));
        scoreText.setString("Score: " + to_string(score));
        window.draw(timerText);
        window.draw(scoreText);

        window.display();

        if (remainingTime <= 0 || lowGradesCaught >= 3) {
            cout << (score >= WINNING_SCORE? "You Win!" : "Game Over!") << " Score: " << score << endl;
            break;
        }
    }

    return 0;
}