#include "LedControl.h"

LedControl lc = LedControl(12,11,10,1);

const int ROWS = 16;
const int COLS = 8;

int barRow, barCol, barLength;
boolean gameComplete;
char direction;
boolean **Map;

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
boolean throttleButton = false; // allows the button to be activated only once at a time

long interval = 1000;        // we want to move the bar at some regular intervals
long previousMillis = 0;

// generates an empty new Map
void populateNewMap() {
    Map = new boolean*[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        Map[i] = new boolean[COLS];

        for (int j = 0; j < COLS; ++j) {
            Map[i][j] = 0;
        }
    }
}

// add the bar to the specified position in the map
void addBar(int barRow, int barCol, int barLength) {
    for (int j = 0; j < barLength; ++j) {
        Map[barRow][barCol + j] = 1;
    }
}

void initGame() {
    barRow = ROWS - 1;
    barCol = 0;
    barLength = 5;
    direction = 'r';
    gameComplete = false;

    // generate a new Map
    populateNewMap();
    // add the bar at the bottom of the map
    addBar(barRow, barCol, barLength);
    Serial.print("Starting new game.");
}

// moves the bar one position to the direction that the
// 'direction' variable defines (right or left)
void moveBar() {
    boolean *rowClone = new boolean[COLS];
    for (int i = 0; i < COLS; ++i) {
        rowClone[i] = Map[barRow][i];
    }

    int moves = barLength;
    for (int i = barCol; moves > 0; --moves) {
        if ((i == 0 && direction == 'l') ||
            (i == COLS - 1 && direction == 'r')) {
            Serial.print("Invalid move");
            return;
        }

        if (direction == 'r') {
            Map[barRow][i + 1] = rowClone[i];
            if (i == 0) {
                Map[barRow][i] = 0;
            } else {
                Map[barRow][i] = rowClone[i - 1];
            }
        } else {
            Map[barRow][i - 1] = rowClone[i];
            Map[barRow][i] = 0;
        }

        ++i;
    }

    delete[] rowClone;
}

void renderGame() {
    // Show the Map on the LED Matrix
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            lc.setLed(0, j, i, Map[i][j]);
        }
    }
}

void tick() {
    if (direction == 'r' && barCol + barLength >= COLS) {
        direction = 'l';
    } else if (direction == 'l' && barCol == 0) {
        direction = 'r';
    }

    moveBar();

    barCol = direction == 'r' ? barCol + 1 : barCol - 1;

    renderGame();
    printMap();
}

// locks the bar in position and checks if we won or lost
void stickBar() {
    if (barRow < ROWS - 1) {
        int oldBarLength = barLength;
        for (int i = 0; i < oldBarLength; ++i) {
            if (Map[barRow + 1][barCol + i] == 0) {
                --barLength;
                Map[barRow][barCol + i] = 0;
            }
        }
    }

    if (barLength <= 0) {
        Serial.println("You lost! Reboot to start again.");
        gameComplete = true;
        return;
    }

    if (barRow <= 0) {
        Serial.println("Congratulations, you won! Reboot to start again.");
        gameComplete = true;
        return;
    }

    --barRow;
    barCol = 0;
    addBar(barRow, barCol, barLength);
}

// prints the map in the terminal for debugging
void printMap() {
    Serial.print('\n');
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            Serial.print(Map[i][j]);
        }
        Serial.print('\n');
    }
    Serial.print('\n');
}

void setup() {
    Serial.begin(9600);      // open the serial port at 9600 bps:

    lc.shutdown(0,false);
    // Set the brightness to a medium values
    lc.setIntensity(0, 8);
    // and clear the display
    lc.clearDisplay(0);

    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);

    initGame();
    // debugging
    printMap();
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis > interval) {
        // check if the interval time has passed since the last time
        // we moved the bar
        tick();

        previousMillis = currentMillis;
    }

    // read the state of the pushbutton value
    buttonState = digitalRead(buttonPin);
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH
    // and we should stick the bar
    if (buttonState == HIGH && !throttleButton) {
        throttleButton = true;
        if (gameComplete) {
            initGame();
        } else {
            stickBar();
        }
    } else if (buttonState == LOW) {
        throttleButton = false;
    }
}
