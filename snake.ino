#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN D6
#define UP_SWITCH D1
#define DOWN_SWITCH D2
#define LEFT_SWITCH D3
#define RIGHT_SWITCH D4

// MATRIX DECLARATION:
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(20, 17, PIN,
                                                NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                                    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                                                NEO_GRB + NEO_KHZ800);

const int SNAKE_MAX_LENGTH = 50;
const int INITIAL_SNAKE_LENGTH = 3;
const int INITIAL_SNAKE_X = 10;
const int INITIAL_SNAKE_Y = 8;

int snakeX[SNAKE_MAX_LENGTH];
int snakeY[SNAKE_MAX_LENGTH];
int snakeLength = INITIAL_SNAKE_LENGTH;
int foodX, foodY;
int snakeDirX = 1;
int snakeDirY = 0;
unsigned long lastMoveTime = 0;
unsigned long moveInterval = 100;

void setup()
{
    Serial.begin(9600); // Initialize serial communication
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(255);

    // Initialize snake position
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++)
    {
        snakeX[i] = INITIAL_SNAKE_X - i;
        snakeY[i] = INITIAL_SNAKE_Y;
    }

    // Generate initial food position
    generateFood();

    // Set up switches
    pinMode(UP_SWITCH, INPUT_PULLUP);
    pinMode(DOWN_SWITCH, INPUT_PULLUP);
    pinMode(LEFT_SWITCH, INPUT_PULLUP);
    pinMode(RIGHT_SWITCH, INPUT_PULLUP);
}

void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastMoveTime >= moveInterval)
    {
        lastMoveTime = currentMillis;
        readSwitches();
        moveSnake();
        checkCollisions();
        drawSnake();
    }
}

void readSwitches()
{
    if (digitalRead(UP_SWITCH) == LOW && snakeDirY != 1)
    {
        snakeDirX = 0;
        snakeDirY = -1;
        Serial.println("UP switch pressed");
    }
    else if (digitalRead(DOWN_SWITCH) == LOW && snakeDirY != -1)
    {
        snakeDirX = 0;
        snakeDirY = 1;
        Serial.println("DOWN switch pressed");
    }
    else if (digitalRead(LEFT_SWITCH) == LOW && snakeDirX != 1)
    {
        snakeDirX = -1;
        snakeDirY = 0;
        Serial.println("LEFT switch pressed");
    }
    else if (digitalRead(RIGHT_SWITCH) == LOW && snakeDirX != -1)
    {
        snakeDirX = 1;
        snakeDirY = 0;
        Serial.println("RIGHT switch pressed");
    }
}

void moveSnake()
{
    // Move the snake
    for (int i = snakeLength - 1; i > 0; i--)
    {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Update snake head position
    snakeX[0] += snakeDirX;
    snakeY[0] += snakeDirY;

    // Wrap snake around if it goes out of bounds
    if (snakeX[0] >= matrix.width())
    {
        snakeX[0] = 0;
    }
    else if (snakeX[0] < 0)
    {
        snakeX[0] = matrix.width() - 1;
    }

    if (snakeY[0] >= matrix.height())
    {
        snakeY[0] = 0;
    }
    else if (snakeY[0] < 0)
    {
        snakeY[0] = matrix.height() - 1;
    }
}

void checkCollisions()
{
    // Check collision with itself
    for (int i = 1; i < snakeLength; i++)
    {
        if (snakeX[i] == snakeX[0] && snakeY[i] == snakeY[0])
        {
            // Game over
            gameOver();
        }
    }

    // Check collision with food
    if (snakeX[0] == foodX && snakeY[0] == foodY)
    {
        // Increase snake length
        if (snakeLength < SNAKE_MAX_LENGTH)
        {
            snakeLength++;
        }
        // Generate new food
        generateFood();

        // Decrease move interval to increase speed
        if (moveInterval > 50)
        {
            moveInterval -= 4; // Adjust the decrement value as needed
        }
    }
}


void generateFood()
{
    // Generate food at random position
    foodX = random(matrix.width());
    foodY = random(matrix.height());
}

void drawSnake()
{
    matrix.fillScreen(0); // Clear screen
    matrix.fillScreen(matrix.Color(140, 140, 0)); // Fill screen with yellow color

    // Draw snake body in green color
    for (int i = 0; i < snakeLength; i++)
    {
        matrix.drawPixel(snakeX[i], snakeY[i], matrix.Color(0, 255, 0)); // Green color
    }

    // Draw snake head in white color
    matrix.drawPixel(snakeX[0], snakeY[0], matrix.Color(255, 255, 255)); // White color

    // Draw food with blue color
    matrix.drawPixel(foodX, foodY, matrix.Color(0, 0, 255)); // Blue color

    matrix.show();
}

void gameOver()
{
    matrix.fillScreen(0); // Clear screen
    matrix.setTextSize(1); // Set text size to 2
    matrix.setTextColor(matrix.Color(0, 255, 0)); // Green color

    // Display the game score
    matrix.setCursor(5, 5);
    matrix.print(snakeLength - INITIAL_SNAKE_LENGTH); // Score is the number of foods eaten
    matrix.show();
    delay(2000); // Show the score for 2 seconds

    // Initialize variables for scrolling
    int xPos = matrix.width(); // Initial x-position for scrolling
    int yPos = 2; // y-position for the message, two rows up
    int delayTime = 60; // Initial delay time for scrolling
    delay(1000); // Delay before displaying "EXCEL 2023"

    // Continuous loop for scrolling and waiting for button press
    while (true)
    {
        // Scroll the "EXCEL 2023" message from right to left
        for (int i = xPos; i >= -(12 * 12); i--)
        {
            matrix.fillScreen(0); // Clear screen
            matrix.fillScreen(matrix.Color(190, 187, 196));
            matrix.setTextSize(2);
            int x = i;
            if (x < -(12 * 12)) {
                x += matrix.width(); // Re-enter from the other end
            }
            matrix.setCursor(x, yPos); // Set cursor position
            matrix.print(F("EXCEL 2023")); // Display "EXCEL 2023" message
            matrix.show(); // Show on the matrix
            delay(delayTime); // Adjust the scrolling speed (increase/decrease delay)

            // Check if any button is pressed
            if (digitalRead(UP_SWITCH) == LOW || digitalRead(DOWN_SWITCH) == LOW || digitalRead(LEFT_SWITCH) == LOW || digitalRead(RIGHT_SWITCH) == LOW)
            {
                // Reset the game
                resetGame();
                return;
            }
        }
    }
}



void resetGame()
{
    // Reset snake position and length
    snakeLength = INITIAL_SNAKE_LENGTH;
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++)
    {
        snakeX[i] = INITIAL_SNAKE_X - i;
        snakeY[i] = INITIAL_SNAKE_Y;
    }

    // Generate new food position
    generateFood();

    // Reset snake direction
    snakeDirX = 1;
    snakeDirY = 0;
    moveInterval = 150;

    // Clear the screen
    matrix.fillScreen(0);
    matrix.show();
}
