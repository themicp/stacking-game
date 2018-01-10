const int ROWS = 16;
const int COLS = 8;

int barRow = ROWS - 1,
    barCol = 0,
    barLength = 5;
char direction = 'r';

int **Map;

void populateNewMap() {
    Map = new int*[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        Map[i] = new int[COLS];

        for (int j = 0; j < COLS; ++j) {
            Map[i][j] = 0;
        }
    }
}

void addBar(int barRow, int barCol, int barLength) {
    for (int j = 0; j < barLength; ++j) {
        Map[barRow][barCol + j] = 1;
    }
}

void moveBar() {
    int *rowClone = new int[COLS];
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

void tick() {
    if (direction == 'r' && barCol + barLength >= COLS) {
        direction = 'l';
    } else if (direction == 'l' && barCol == 0) {
        direction = 'r';
    }

    moveBar();

    barCol = direction == 'r' ? barCol + 1 : barCol - 1;
}

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

    if (barLength == 0) {
        Serial.print("You lost! Refresh to start again.");
        return;
    }

    if (barRow == 0) {
        Serial.print("Congratulations, you won! Refresh to start again.");
        return;
    }

    --barRow;
    barCol = 0;
    addBar(barRow, barCol, barLength);
}

void printMap() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            Serial.print(Map[i][j]);
        }
        Serial.print('\n');
    }
    Serial.print('\n');
}

void setup() {
}

void loop() {
}
