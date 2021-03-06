let ROWS = 16;
let COLS = 8;

function generateMap(ROWS, COLS) {
    let map = [];
    for (let i = 0; i < ROWS; ++i) {
        map[i] = [];
        for (let j = 0; j < COLS; ++j) {
            map[i][j] = 0;
        }
    }

    return map;
}

function moveBar() {
    let rowClone = map[barRow].slice(0);
    let moves = barLength;
    for (let i = barCol; moves > 0; --moves) {
        if ((i == 0 && direction == 'l') ||
            (i == COLS - 1 && direction == 'r')) {
            throw new Error('Invalid move ' + direction);
        }

        if (direction == 'r') {
            map[barRow][i + 1] = rowClone[i];
            if (i == 0) {
                map[barRow][i] = 0;
            } else {
                map[barRow][i] = rowClone[i - 1];
            }
        } else {
            map[barRow][i - 1] = rowClone[i];
            map[barRow][i] = 0;
        }

        ++i;
    }
}

function addBar(barRow, barCol, barLength) {
    for (j = 0; j < barLength; ++j) {
        map[barRow][barCol + j] = 1;
    }

    return map;
}


let barRow = ROWS - 1, barCol = 0, barLength = 5, direction = 'r';
let map = generateMap(ROWS, COLS);

map = addBar(barRow, barCol, barLength);

function renderGame() {
    let game = document.getElementById('game');
    while (game.firstChild) {
        game.removeChild(game.firstChild);
    }
    for (let i = 0; i < ROWS; ++i) {
        let row = document.createElement('div');
        row.id = 'row' + i;
        row.className = 'row';

        for (let j = 0; j < COLS; ++j) {
            let block = document.createElement('div');
            block.id = 'block' + i;
            block.className = 'block';
            if (map[i][j] == 1) {
                block.className += ' active';
            }
            row.appendChild(block);
        }

        game.appendChild(row);
    }
}

function tick() {
    if (direction == 'r' && barCol + barLength >= COLS) {
        direction = 'l';
    } else if (direction == 'l' && barCol == 0) {
        direction = 'r';
    }

    console.log(barCol, direction);
    moveBar(barRow,barCol, barLength, direction, map, COLS);
    barCol = direction == 'r' ? barCol + 1 : barCol - 1;

    renderGame(map);
}

function stickBar() {
    if (barRow < ROWS - 1) {
        let oldBarLength = barLength;
        for (let i = 0; i < oldBarLength; ++i) {
            if (map[barRow + 1][barCol + i] == 0) {
                --barLength;
                map[barRow][barCol + i] = 0;
            }
        }
    }

    if (barLength == 0) {
        clearInterval(tickInterval);
        alert('You lost! Refresh to start again.');
        return;
    }
    if (barRow == 0) {
        clearInterval(tickInterval);
        alert('Congratulations, you won! Refresh to start again.');
        return;
    }

    --barRow;
    barCol = 0;
    map = addBar(barRow, barCol, barLength);
}

let button = document.getElementById('button');
button.onclick = stickBar;
window.addEventListener('keypress', function(e) {
    if (e.keyCode == 32) {
        stickBar();
    }
});

let tickInterval = setInterval(tick.bind(this, map), 120);
