#include <stdlib.h>
#include <glut.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include<vector>
#include <string>
#include <sstream>


time_t startTime;
time_t currentTime;
int countdown = 90;

// Screen dimension constants

const int SCREEN_HEIGHT = 600;
float goalX = 70.0f;
float goalY = 70.0f;
float goalSize = 30.0f;
const int SCREEN_WIDTH = 800;

const float PLAYER_SIZE = 20.0f;
float playerX = SCREEN_WIDTH / 2;
float playerY = SCREEN_HEIGHT / 2;
float playerSpeed = 6.5f;
float playerDirection = 90.0f; // Initial direction of the player (90 degrees)
int playerLives = 5;
int gameScore = 0;
int gameTime = 90000;
int speedBoostDuration = 0;
int ShieldDuration = 0;
bool gameOver = false;
bool Loose = false;
bool showShield = false;
bool Win = false;
int powerUpDuration = 15000;
int secondstoAppear = 10000;
int powerUpSpawnTimer = 10000;
bool isDotVisible = true;
int blinkInterval = 500; // in milliseconds
std::vector<std::pair<float, float>> obstaclePositions;
std::vector<std::pair<float, float>> collectiblePositions;
std::vector<std::pair<float, float>> SpeedBoost;
std::vector<std::pair<float, float>> Shield;
std::vector<std::pair<float, float>> dotCoordinates = {
    { 400, 400.0f },
    { 200.0f, 200.0f },
    { 300.0f, 300.0f }
};


// Function to generate a random position within the screen boundaries
float generateRandomPosition(float min, float max, const std::vector<std::pair<float, float>>& existingPositions, float minDistance) {
    while (true) {
        float randomX = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
        float randomY = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));

        // Check the distance from existing positions
        bool validPosition = true;
        for (const auto& position : existingPositions) {
            if (std::sqrt(std::pow(position.first - randomX, 2) + std::pow(position.second - randomY, 2)) < minDistance) {
                validPosition = false;
                break;
            }
        }

        if (validPosition) {
            return randomX;
        }
    }
}
void drawStars() {
    glColor3f(1.0, 1.0, 1.0); // White color

    // Draw multiple stars
    for (int i = 0; i < 100; i++) {
        float x = rand() % (int)(800);
        float y = rand() % (int)600;
        int starSize = rand() % 3 + 1; // Vary the star size

        // Randomly make the star blink
        if (rand() % 4 != 0) {
            glBegin(GL_POINTS);
            glVertex2f(x, y);
            glEnd();
        }
    }
}


void updateTimer(int value) {
    countdown--;
    if (countdown > 0) {
        glutTimerFunc(1000, updateTimer, 0);
    }

    glutPostRedisplay();
}


void drawGoal() {
    static float rotationAngle = 0;
    // Draw the goal as an exotic shape using five primitives

    // Primitive 1: GL_POLYGON
    glColor3f(1.0, 0.5, 0.0); // Set color to orange for the goal
    glPushMatrix();
    glTranslatef(goalX, goalY, 0.0f); // Set the position of the goal at the top left
    glScalef(1.2f, 1.2f, 1.0f); // Scale the goal to adjust the size
    rotationAngle -= 0.1f;
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

    // Draw the goal using GL_POLYGON
    glBegin(GL_POLYGON);
    // Vertex 1
    glVertex2f(0.0f, 20.0f);
    // Vertex 2
    glVertex2f(10.0f, 0.0f);
    // Vertex 3
    glVertex2f(20.0f, 20.0f);
    // Vertex 4
    glVertex2f(15.0f, 30.0f);
    // Vertex 5
    glVertex2f(5.0f, 30.0f);
    glEnd();

    // Primitive 2: GL_POINTS
    // Draw additional decorations or details
    glColor3f(0.0, 1.0, 1.0); // Set color to cyan for decorations
    glPointSize(4.0);
    glBegin(GL_POINTS);
    // Draw decorative dots around the goal
    glVertex2f(5.0f, 15.0f);
    glVertex2f(15.0f, 15.0f);
    glEnd();

    // Primitive 3: GL_TRIANGLES
    // Draw additional shapes
    glColor3f(0.0, 0.5, 1.0); // Set color to blue for shapes
    glBegin(GL_TRIANGLES);
    // First triangle
    glVertex2f(-5.0f, 20.0f);
    glVertex2f(0.0f, 15.0f);
    glVertex2f(5.0f, 20.0f);
    // Second triangle
    glVertex2f(15.0f, 20.0f);
    glVertex2f(20.0f, 15.0f);
    glVertex2f(25.0f, 20.0f);
    glEnd();

    // Primitive 4: GL_LINE_LOOP
    // Draw a border around the goal
    glColor3f(1.0, 0.0, 0.0); // Set color to red for the border
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    // Draw the border using GL_LINE_LOOP
    glVertex2f(0.0f, 20.0f);
    glVertex2f(10.0f, 0.0f);
    glVertex2f(20.0f, 20.0f);
    glVertex2f(15.0f, 30.0f);
    glVertex2f(5.0f, 30.0f);
    glEnd();

    // Primitive 5: GL_QUADS
    // Draw a square inside the goal
    glColor3f(0.5, 1.0, 0.5); // Set color to light green for the square
    glBegin(GL_QUADS);
    // Draw the square using GL_QUADS
    glVertex2f(7.0f, 15.0f);
    glVertex2f(13.0f, 15.0f);
    glVertex2f(13.0f, 25.0f);
    glVertex2f(7.0f, 25.0f);
    glEnd();

    glPopMatrix();
}
void drawDots() {
    glColor3f(1.0f, 1.0f, 1.0f); // Set dot color to white
    glPointSize(10.0f); // Set dot size

    glBegin(GL_POINTS);
    for (const auto& coordinate : dotCoordinates) {
        glVertex2f(coordinate.first, coordinate.second);
    }
    glEnd();
}


void drawShield() {
    static float rotationAngle = 0;
    // Draw the main shield body
    glColor3f(0.5, 0.5, 0.5); // Set color to gray
    glPushMatrix();
    glTranslatef(playerX, playerY, 0.0f);
    rotationAngle -= 0.1f;
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360); // get the current angle

        float x = 30.0f * cosf(theta); // calculate the x component
        float y = 20.0f * sinf(theta); // calculate the y component

        glVertex2f(x, y); // output vertex
    }
    glEnd();
    glPopMatrix();

    // Draw a horizontal line in the middle of the shield
    glColor3f(0.0, 0.0, 0.0); // Set color to black
    glPushMatrix();
    glTranslatef(playerX, playerY, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(-30.0f, 0.0f);
    glVertex2f(30.0f, 0.0f);
    glEnd();
    glPopMatrix();
}
// Function to draw the shield collectible
void drawShieldCollectibles() {
    static float rotationAngle = 0;
    for (const auto& pos : Shield) {
        glPushMatrix();
        glTranslatef(pos.first, pos.second, 0.0f);
        rotationAngle -= 0.1f;
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
        // Primitive 1: GL_POLYGON
        glColor3f(0.2, 0.4, 0.6); // Set color to a shade of blue
        glBegin(GL_POLYGON);
        glVertex2f(-10.0f, -5.0f);
        glVertex2f(10.0f, -5.0f);
        glVertex2f(0.0f, 15.0f);
        glEnd();

        // Primitive 2: GL_LINE_LOOP
        glColor3f(0.2, 0.4, 0.6); // Set color to a shade of blue
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 10) {
            float theta = 3.1415926f * float(i) / 180.0f; // get the current angle

            float radius = 15.0f; // Set the radius of the shield collectibles
            float x_pos = radius * cosf(theta); // calculate the x component
            float y_pos = radius * sinf(theta); // calculate the y component

            glVertex2f(x_pos, y_pos); // output vertex
        }
        glEnd();

        // Primitive 3: GL_TRIANGLES
        glColor3f(0.5, 0.5, 0.5); // Set color to gray
        glBegin(GL_TRIANGLES);
        glVertex2f(-7.0f, -5.0f);
        glVertex2f(0.0f, -15.0f);
        glVertex2f(7.0f, -5.0f);
        glEnd();

        // Primitive 4: GL_POINTS
        glColor3f(0.8, 0.8, 0.8); // Set color to a lighter shade of gray
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glVertex2f(0.0f, 0.0f);
        glEnd();

        glPopMatrix();
    }
}



// Function to draw the speed boosts
void drawSpeedBoosts() {
    static float rotationAngle = 0.0f;
    // Draw speed boosts using four primitives
    for (const auto& pos : SpeedBoost) {
        glPushMatrix();
        glTranslatef(pos.first, pos.second, 0.0f);
        // Update the rotation angle
        rotationAngle += 0.1f;
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

        // Primitive 1: GL_TRIANGLES
        glColor3f(1.0, 1.0, 0.0); // Set color to yellow for speed boosts
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 15.0f);
        glVertex2f(-10.0f, -10.0f);
        glVertex2f(10.0f, -10.0f);
        glEnd();

        // Primitive 2: GL_LINES
        glColor3f(1.0, 0.0, 0.0); // Set color to red for lines
        glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex2f(-10.0f, -10.0f);
        glVertex2f(0.0f, 15.0f);
        glVertex2f(0.0f, 15.0f);
        glVertex2f(10.0f, -10.0f);
        glVertex2f(10.0f, -10.0f);
        glVertex2f(-10.0f, -10.0f);
        glEnd();

        // Primitive 3: GL_POINTS
        glColor3f(0.0, 1.0, 1.0); // Set color to cyan for points
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glVertex2f(-5.0f, 2.0f);
        glVertex2f(5.0f, 2.0f);
        glEnd();

        // Primitive 4: GL_POLYGON
        glColor3f(0.0, 0.5, 0.5); // Set color to dark cyan for polygon
        glBegin(GL_POLYGON);
        glVertex2f(-5.0f, 0.0f);
        glVertex2f(-3.0f, -5.0f);
        glVertex2f(3.0f, -5.0f);
        glVertex2f(5.0f, 0.0f);
        glVertex2f(0.0f, 5.0f);
        glEnd();

        glPopMatrix();
    }
}


void drawPlayer() {
    if (showShield) {
        drawShield();
    }

    // Set color to yellow
    glColor3f(1.0, 1.0, 0.0);

    glPushMatrix();
    glTranslatef(playerX, playerY, 0.0f);

    glRotatef(playerDirection - 90.0f, 0.0f, 0.0f, 1.0f); // Rotate Pac-Man based on the direction

    // Draw Pac-Man's body as a circle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f); // Center of Pac-Man
    for (int i = 135; i <= 405; i++) { // Draw from 135 to 405 degrees (making a semi-circle)
        float theta = i * 3.14159 / 180;
        float x = PLAYER_SIZE * cos(theta);
        float y = PLAYER_SIZE * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw Pac-Man's mouth as a white triangle
    glColor3f(1.0, 1.0, 1.0); // Set color to white
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.0f); // Center of Pac-Man's body
    glVertex2f(PLAYER_SIZE * 0.5f, PLAYER_SIZE * 0.2f); // First point of the mouth
    glVertex2f(PLAYER_SIZE * 0.5f, -PLAYER_SIZE * 0.2f); // Second point of the mouth
    glEnd();

    // Draw eyes as lines
    glColor3f(0.0, 0.0, 0.0); // Set color to white
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex2f(PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.4f); // First point of the left eye
    glVertex2f(PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.2f); // Second point of the left eye
    glVertex2f(-PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.4f); // First point of the right eye
    glVertex2f(-PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.2f); // Second point of the right eye
    glEnd();

    // Draw a rectangle as an additional primitive
    glColor3f(0.5, 0.5, 0.5); // Set color to gray
    glTranslatef(0.0f, -PLAYER_SIZE * 0.5f, 0.0f); // Position the rectangle below Pac-Man
    glRectf(-PLAYER_SIZE * 0.2f, -PLAYER_SIZE * 0.1f, PLAYER_SIZE * 0.2f, PLAYER_SIZE * 0.1f); // Define the rectangle

    glPopMatrix();
}


// Initialize collectibles at random positions
void initCollectibles() {
    srand(static_cast<unsigned>(time(0))); // Seed the random number generator

    for (int i = 0; i < 5; ++i) { // Generate five collectible positions
        float collectibleX = generateRandomPosition(50, SCREEN_WIDTH - 50, obstaclePositions, 50.0f); // Adjust the range as needed
        float collectibleY = generateRandomPosition(50, SCREEN_HEIGHT - 50, obstaclePositions, 50.0f); // Adjust the range as needed
        collectiblePositions.emplace_back(collectibleX, collectibleY);
    }
    for (int i = 0; i < 3; ++i) { // Generate five collectible positions
        float SpeedUPX = generateRandomPosition(50, SCREEN_WIDTH - 50, obstaclePositions, 50.0f); // Adjust the range as needed
        float SpeedUPY = generateRandomPosition(50, SCREEN_HEIGHT - 50, obstaclePositions, 50.0f); // Adjust the range as needed
        SpeedBoost.emplace_back(SpeedUPX, SpeedUPY);
    }
    for (int i = 0; i < 2; ++i) { // Generate five collectible positions
        float ShieldX = generateRandomPosition(50, SCREEN_WIDTH - 50, obstaclePositions, 50.0f); // Adjust the range as needed
        float ShieldY = generateRandomPosition(50, SCREEN_HEIGHT - 50, obstaclePositions, 50.0f); // Adjust the range as needed
        Shield.emplace_back(ShieldX, ShieldY);
    }
}
// Adjust the duration as needed

// Function to update power-up durations and remove them after a certain time
void updatePowerUps() {
    if (speedBoostDuration > 0) {
        speedBoostDuration--;
        if (speedBoostDuration == 0) {
            playerSpeed = 6.5f; // Set the speed back to the original value
        }
    }

    if (ShieldDuration > 0) {
        ShieldDuration--;
        if (ShieldDuration == 0) {
            showShield = false; // Set the shield status to false
        }
    }

    if (powerUpDuration > 0) {
        powerUpDuration--;
        if (powerUpDuration == 0) {
            SpeedBoost.clear(); // Remove all SpeedBoost power-ups
            Shield.clear(); // Remove all Shield power-ups
            powerUpDuration = 10000; // Reset the timer for power-up disappearance
        }
    }
}

// Function to draw the collectibles
void drawCollectibles() {
    static float rotationAngle = 0;
    // Draw a cherry as a red circle

    glColor3f(1.0, 0.0, 0.0); // Set color to red for the cherry
    for (const auto& pos : collectiblePositions) {
        glPushMatrix();
        glTranslatef(pos.first, pos.second, 0.0f);

        glBegin(GL_POLYGON);
        const int num_segments = 100;
        for (int i = 0; i < num_segments; i++) {

            float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // get the current angle
            float x = 5.0f * cosf(theta); // calculate the x component
            float y = 5.0f * sinf(theta); // calculate the y component
            glVertex2f(x, y); // output vertex
        }

        glEnd();

        glPopMatrix();
    }

    // Draw leaves for each cherry
    glColor3f(0.0, 0.8, 0.0); // Set color to green for the leaves
    for (const auto& pos : collectiblePositions) {
        glPushMatrix();
        glTranslatef(pos.first + 3.0f, pos.second + 5.0f, 0.0f); // Adjust the position of the first leaf
        rotationAngle -= 0.05f;
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
        glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-2.0f, 5.0f);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(4.0f, 7.0f);
        glVertex2f(6.0f, 3.0f);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(pos.first - 3.0f, pos.second + 5.0f, 0.0f); // Adjust the position of the second leaf
        rotationAngle -= 0.05f;
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
        glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-2.0f, 5.0f);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(4.0f, 7.0f);
        glVertex2f(6.0f, 3.0f);
        glEnd();
        glPopMatrix();
    }
}

void updateCollectibles() {
    // Decrease the timer for collectible disappearance
    if (secondstoAppear > 0) {
        secondstoAppear--;
    }
    else {
        // Remove all the collectibles and generate new ones
        collectiblePositions.clear();
        for (int i = 0; i < 5; ++i) { // Generate five new collectible positions
            float collectibleX = generateRandomPosition(50, SCREEN_WIDTH - 50, obstaclePositions, 50.0f); // Adjust the range as needed
            float collectibleY = generateRandomPosition(50, SCREEN_HEIGHT - 50, obstaclePositions, 50.0f); // Adjust the range as needed
            collectiblePositions.emplace_back(collectibleX, collectibleY);
        }
        secondstoAppear = 10000; // Reset the timer for collectible disappearance
    }
}


// Function to draw the obstacles
void drawObstacles() {
    // Draw obstacles based on the positions stored in the vector
    for (const auto& pos : obstaclePositions) {
        glPushMatrix();
        glTranslatef(pos.first, pos.second, 0.0f);

        // Draw the body of the ghost
        glColor3f(1.0, 1.0, 1.0); // Set color to white for the ghost
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f); // Bottom-left vertex
        glVertex2f(20.0f, 0.0f); // Bottom-right vertex
        glVertex2f(20.0f, 20.0f); // Top-right vertex
        glVertex2f(0.0f, 20.0f); // Top-left vertex
        glEnd();

        // Draw the eyes of the ghost
        glColor3f(0.0, 0.0, 0.0); // Set color to black for the eyes
        glPointSize(6.0);
        glBegin(GL_POINTS);
        glVertex2f(6.0f, 12.0f); // Left eye
        glVertex2f(14.0f, 12.0f); // Right eye
        glEnd();

        // Draw the mouth of the ghost
        glColor3f(1.0, 0.0, 0.0); // Set color to red for the mouth
        glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex2f(6.0f, 6.0f); // First point of the mouth
        glVertex2f(14.0f, 6.0f); // Second point of the mouth
        glEnd();

        glPopMatrix();
    }
}



// Function to initialize obstacles at specific positions
void initObstacles() {
    // Add specific positions for 5 obstacles
    obstaclePositions.emplace_back(100, 100);
    obstaclePositions.emplace_back(200, 200);
    obstaclePositions.emplace_back(300, 300);
    obstaclePositions.emplace_back(400, 400);
    obstaclePositions.emplace_back(500, 500);
}
std::pair<float, float> generateRandomPowerUpPosition() {
    float powerUpX = generateRandomPosition(50, SCREEN_WIDTH - 50, obstaclePositions, 50.0f); // Adjust the range as needed
    float powerUpY = generateRandomPosition(50, SCREEN_HEIGHT - 50, obstaclePositions, 50.0f); // Adjust the range as needed
    return std::make_pair(powerUpX, powerUpY);
}
void drawHealthBar() {
    int maxHealth = 5; // Maximum health of the player
    int currentHealth = playerLives; // Current health of the player

    float healthBarWidth = 100.0f; // Width of the health bar
    float healthBarHeight = 20.0f; // Height of the health bar
    float healthBarX = 50.0f; // X position of the health bar
    float healthBarY = SCREEN_HEIGHT - 50.0f; // Y position of the health bar

    // Draw the outline of the health bar
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(healthBarX, healthBarY);
    glVertex2f(healthBarX + healthBarWidth, healthBarY);
    glVertex2f(healthBarX + healthBarWidth, healthBarY + healthBarHeight);
    glVertex2f(healthBarX, healthBarY + healthBarHeight);
    glEnd();

    // Calculate the current health percentage
    float healthPercentage = static_cast<float>(currentHealth) / maxHealth;

    // Draw the filled portion of the health bar
    glColor3f(0.0f, 1.0f, 0.0f); // Set color to green
    glBegin(GL_QUADS);
    glVertex2f(healthBarX, healthBarY);
    glVertex2f(healthBarX + healthBarWidth * healthPercentage, healthBarY);
    glVertex2f(healthBarX + healthBarWidth * healthPercentage, healthBarY + healthBarHeight);
    glVertex2f(healthBarX, healthBarY + healthBarHeight);
    glEnd();
}

// Function to draw the game environment
void drawEnvironment() {
    glColor3f(1.0, 1.0, 1.0); // Set color to white

    // Top boundary
    glRectf(10, 10, SCREEN_WIDTH - 10, 20);
    glBegin(GL_TRIANGLES);
    glVertex2f(10, 20);
    glVertex2f(SCREEN_WIDTH / 2, 40);
    glVertex2f(SCREEN_WIDTH - 10, 20);
    glEnd();

    // Bottom boundary
    glRectf(10, SCREEN_HEIGHT - 20, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10);
    glBegin(GL_TRIANGLES);
    glVertex2f(10, SCREEN_HEIGHT - 20);
    glVertex2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40);
    glVertex2f(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 20);
    glEnd();

    // Left boundary
    glRectf(10, 10, 20, SCREEN_HEIGHT - 10);
    glBegin(GL_TRIANGLES);
    glVertex2f(10, 20);
    glVertex2f(20, 10);
    glVertex2f(20, 30);
    glEnd();
    // Right boundary
    glRectf(SCREEN_WIDTH - 20, 10, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10);
    glBegin(GL_TRIANGLES);
    glVertex2f(SCREEN_WIDTH - 10, 20);
    glVertex2f(SCREEN_WIDTH - 20, 10);
    glVertex2f(SCREEN_WIDTH - 20, 30);
    glEnd();

    if (playerX - PLAYER_SIZE < 10 || playerX + PLAYER_SIZE > SCREEN_WIDTH - 10 ||
        playerY - PLAYER_SIZE < 10 || playerY + PLAYER_SIZE > SCREEN_HEIGHT - 10) {
        if (ShieldDuration > 0) {
            // If the shield is active, remove the shield and continue
            ShieldDuration = 0;
            showShield = false;
            playerX = SCREEN_WIDTH / 2; // Reset player position to the center
            playerY = SCREEN_HEIGHT / 2;
        }
        else {
            playerLives--; // Decrement player lives on collision
            playerX = SCREEN_WIDTH / 2; // Reset player position to the center
            playerY = SCREEN_HEIGHT / 2;
            if (playerLives == 0) {
                gameOver = true; // Set game over when lives reach 0
                Loose = true;
            }
        }
    }

    // Adjust the collision logic for the top and bottom boundaries
    if (playerY - PLAYER_SIZE < 20 && playerY + PLAYER_SIZE > 20) {
        float leftBoundary = 10 + (playerY - 20) * (SCREEN_WIDTH / 2 - 10) / 20;
        float rightBoundary = SCREEN_WIDTH - 10 - (playerY - 20) * (SCREEN_WIDTH / 2 - 10) / 20;

        if (playerX - PLAYER_SIZE < leftBoundary || playerX + PLAYER_SIZE > rightBoundary) {
            if (ShieldDuration > 0) {
                // If the shield is active, remove the shield and continue
                ShieldDuration = 0;
                showShield = false;
                playerX = SCREEN_WIDTH / 2; // Reset player position to the center
                playerY = SCREEN_HEIGHT / 2;
            }
            else {
                playerLives--; // Decrement player lives on collision
                playerX = SCREEN_WIDTH / 2; // Reset player position to the center
                playerY = SCREEN_HEIGHT / 2;
                if (playerLives == 0) {
                    gameOver = true; // Set game over when lives reach 0
                    Loose = true;
                }
            }
        }
    }

    drawGoal();

    // Check for collisions with the goal
    if (playerX + PLAYER_SIZE > goalX - goalSize / 2 && playerX - PLAYER_SIZE < goalX + goalSize / 2 &&
        playerY + PLAYER_SIZE > goalY - goalSize / 2 && playerY - PLAYER_SIZE < goalY + goalSize / 2) {
        std::cout << "Congratulations! You Win!" << std::endl;
        gameOver = true;
        Win = true;
    }





    // Check for collisions with the collectibles
    for (auto it = collectiblePositions.begin(); it != collectiblePositions.end();) {
        float collectibleX = it->first;
        float collectibleY = it->second;

        if (playerX + PLAYER_SIZE > collectibleX - 10 && playerX - PLAYER_SIZE < collectibleX + 10 &&
            playerY + PLAYER_SIZE > collectibleY - 10 && playerY - PLAYER_SIZE < collectibleY + 10) {
            it = collectiblePositions.erase(it); // Remove the collectible when the player collides with it
            gameScore++; // Increase the game score
        }
        else {
            ++it;
        }
    }
    for (auto it = SpeedBoost.begin(); it != SpeedBoost.end();) {
        float SpeedBoostX = it->first;
        float SpeedBoostY = it->second;
        if (playerX + PLAYER_SIZE > SpeedBoostX - 10 && playerX - PLAYER_SIZE < SpeedBoostX + 10 &&
            playerY + PLAYER_SIZE > SpeedBoostY - 10 && playerY - PLAYER_SIZE < SpeedBoostY + 10) {
            it = SpeedBoost.erase(it); // Remove the collectible when the player collides with it
            gameScore++; // Increase the game score
            playerSpeed = 8.0f;
            speedBoostDuration = 10000;

        }
        else {
            ++it;
        }
    }
    for (auto it = Shield.begin(); it != Shield.end();) {
        float ShieldCollectibleX = it->first;
        float ShieldCollectibleY = it->second;
        if (playerX + PLAYER_SIZE > ShieldCollectibleX - 10 && playerX - PLAYER_SIZE < ShieldCollectibleX + 10 &&
            playerY + PLAYER_SIZE > ShieldCollectibleY - 10 && playerY - PLAYER_SIZE < ShieldCollectibleY + 10) {
            it = Shield.erase(it); // Remove the shield collectible when the player collides with it
            // Add shield functionality here
            gameScore++;
            showShield = true;
            ShieldDuration = 10000;

        }
        else {
            ++it;
        }
    }
    if (powerUpSpawnTimer <= 0) {
        for (int i = 0; i < 1; ++i) { // Generate new SpeedBoost power-ups
            SpeedBoost.push_back(generateRandomPowerUpPosition());
        }
        for (int i = 0; i < 1; ++i) { // Generate new Shield power-ups
            Shield.push_back(generateRandomPowerUpPosition());
        }
        powerUpSpawnTimer = 10000; // Reset the timer for power-up spawning
    }
    else {
        powerUpSpawnTimer--; // Decrement the power-up spawn timer
    }

    // Draw collectibles and power-ups
    drawCollectibles();
    drawObstacles();
    drawSpeedBoosts();
    drawShieldCollectibles();
    //Update
    updateCollectibles();
    updatePowerUps();


    // Check for collisions with the obstacles
    for (const auto& pos : obstaclePositions) {
        if (playerX + PLAYER_SIZE > pos.first && playerX - PLAYER_SIZE < pos.first + 20 &&
            playerY + PLAYER_SIZE > pos.second && playerY - PLAYER_SIZE < pos.second + 20) {
            if (ShieldDuration > 0) {
                // If the shield is active, remove the shield and continue
                ShieldDuration = 0;
                showShield = false;
                playerX = SCREEN_WIDTH / 2; // Reset player position to the center
                playerY = SCREEN_HEIGHT / 2;
            }
            else {
                playerLives--; // Decrement player lives on collision with an obstacle
                playerX = SCREEN_WIDTH / 2; // Reset player position to the center
                playerY = SCREEN_HEIGHT / 2;
                if (playerLives == 0) {
                    gameOver = true; // Set game over when lives reach 0
                    Loose = true;
                }
            }
            break;
        }
    }

    // Draw the player
    drawPlayer();
    drawHealthBar();


    // Draw game score
    glRasterPos2i(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - 30);
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", gameScore);
    for (int i = 0; scoreStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    }

    // Draw remaining game time
    glRasterPos2i(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 30);
    char timeStr[20];
    sprintf(timeStr, "Time: %d", countdown);
    for (int i = 0; timeStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeStr[i]);
    }


}



// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 0.0); // Set clear (background) color to black
    if (!gameOver) {
        drawEnvironment();

        // Update the timer
        currentTime = std::time(nullptr);
        int elapsedSeconds = currentTime - startTime;
        countdown = std::max(90 - elapsedSeconds, 0);

        // Check if the game time has reached zero
        if (countdown == 0) {
            std::cout << "Game Over: Time's up!" << std::endl;
            gameOver = true;
            Loose = true;
        }
    }
    else {
        // Game over logic
        if (Loose) {
            glPushMatrix();
            glColor3f(1.0, 0.0, 0.0); // Set color to red
            glRasterPos2i(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2);
            std::string gameOverText = "Game Over";
            for (char& c : gameOverText) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
            }
            glPopMatrix();
        }
        // Win logic
        if (Win) {
            glPushMatrix();
            glColor3f(0.0, 1.0, 0.0); // Set color to green
            glRasterPos2i(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 + 20);
            std::string winText = "You Win!";
            for (char& c : winText) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
            }
            glPopMatrix();
        }
    }
    drawStars();

    // Display the countdown


    // Continuously update the display only when the game is not over
    glutPostRedisplay();
    glFlush();
}
void updateAnimation(int value) {
    isDotVisible = !isDotVisible; // Toggle the visibility of dots
    glutPostRedisplay();

    // Call the update function after a certain interval
    glutTimerFunc(blinkInterval, updateAnimation, 0);
}

// Initialization function
void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0); // Set clear (background) color to white
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);
}

// Special keys callback function for handling player movement and rotation
void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        playerDirection += 5.0f; // Rotate the player to the left
        break;
    case GLUT_KEY_RIGHT:
        playerDirection -= 5.0f; // Rotate the player to the right
        break;
    case GLUT_KEY_UP: {
        float nextX = playerX + playerSpeed * cos(playerDirection * 3.14159 / 180.0);
        float nextY = playerY + playerSpeed * sin(playerDirection * 3.14159 / 180.0);
        if (nextX - PLAYER_SIZE > 0 && nextX + PLAYER_SIZE < SCREEN_WIDTH && nextY - PLAYER_SIZE > 0 && nextY + PLAYER_SIZE < SCREEN_HEIGHT) {
            playerX = nextX; // Move in the direction the player is pointing
            playerY = nextY; // Move in the direction the player is pointing
        }
        break;
    }
    }
    glutPostRedisplay();
}



// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("2D Game Environment with OpenGL");
    init();
    initObstacles();
    initCollectibles();
    startTime = time(nullptr);
    currentTime = time(nullptr);
    glutTimerFunc(1000, updateTimer, 0);
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, updateAnimation, 0);
    glutMainLoop();
    return 0;
}
