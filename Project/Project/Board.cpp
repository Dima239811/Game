#include <vector>
#include <iostream>
#include <iomanip>
#include "Board.h"

Board::Board()
{
    board.resize(BOARD_SIZE, vector<Piece>(BOARD_SIZE, Piece::None));
    initialize();
    moveMade = false;
    currentPlayer = Piece::White;
    whiteMoves = 0;
    blackMoves = 0;
}

void Board::initialize()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            board[i][j] = Piece::None;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if ((i + j) % 2 == 1)
            {
                board[i][j] = Piece::Black;
            }
        }
    }

    for (int i = 6; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if ((i + j) % 2 == 1)
            {
                board[i][j] = Piece::White;
            }
        }
    }

    currentPlayer = Piece::White;
    whiteMoves = 0;
    blackMoves = 0;
}

// метод проверки, что координаты находятся в пределах доски
bool Board::isInsideBoard(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

// проверка возможности взятия
bool Board::canCapture(int x, int y, int dx, int dy, Piece piece) const
{
    // Для обычных шашек
    if (piece == Piece::White || piece == Piece::Black)
    {
        int x2 = x + dx * 2;
        int y2 = y + dy * 2;

        if (!isInsideBoard(x2, y2))
        {
            return false;
        }

        Piece target = board[x + dx][y + dy];
        Piece destination = board[x2][y2];

        if (destination == Piece::None && (target == Piece::White || target == Piece::WhiteKing || target == Piece::Black || target == Piece::BlackKing))
        {
            if ((piece == Piece::White || piece == Piece::WhiteKing) && (target == Piece::Black || target == Piece::BlackKing))
            {
                return true;
            }
            if ((piece == Piece::Black || piece == Piece::BlackKing) && (target == Piece::White || target == Piece::WhiteKing))
            {
                return true;
            }
        }
        return false;
    }

    // Для дамки
    if (piece == Piece::WhiteKing || piece == Piece::BlackKing)
    {
        int stepX = dx;
        int stepY = dy;
        int capturedCount = 0; // Счетчик вражеских шашек, которые можно взять

        // Проверяем все клетки по диагонали
        while (isInsideBoard(x + stepX, y + stepY))
        {
            Piece current = board[x + stepX][y + stepY];

            // Если пустая клетка, продолжаем движение
            if (current == Piece::None)
            {
                stepX += dx;
                stepY += dy;
                continue;
            }

            // Если нашли свою шашку — дальше нельзя
            if ((piece == Piece::WhiteKing && (current == Piece::White || current == Piece::WhiteKing)) ||
                (piece == Piece::BlackKing && (current == Piece::Black || current == Piece::BlackKing)))
            {
                break;
            }

            // Если нашли вражескую шашку
            if ((piece == Piece::WhiteKing && (current == Piece::Black || current == Piece::BlackKing)) ||
                (piece == Piece::BlackKing && (current == Piece::White || current == Piece::WhiteKing)))
            {
                // Проверяем, есть ли пустая клетка за этой шашкой
                int nextX = x + stepX + dx;
                int nextY = y + stepY + dy;

                if (isInsideBoard(nextX, nextY) && board[nextX][nextY] == Piece::None)
                {
                    capturedCount++; // Можно взять эту шашку
                    stepX += dx;
                    stepY += dy;
                }
                else
                {
                    break; // Нет пустой клетки за шашкой — дальше нельзя
                }
            }
        }

        // Если можно взять хотя бы одну шашку, возвращаем true
        return (capturedCount > 0);
    }

    return false;
}

// выполнение взятия
void Board::performCapture(int x, int y, int dx, int dy) {
    // Для обычных шашек
    if (board[x][y] == Piece::White || board[x][y] == Piece::Black) {
        int x2 = x + dx * 2;
        int y2 = y + dy * 2;

        board[x2][y2] = board[x][y];
        board[x][y] = Piece::None;
        board[x + dx][y + dy] = Piece::None;

        // Превращение в дамку, если шашка достигла последней горизонтали
        if (board[x2][y2] == Piece::White && x2 == 0) {
            board[x2][y2] = Piece::WhiteKing;
        }
        if (board[x2][y2] == Piece::Black && x2 == BOARD_SIZE - 1) {
            board[x2][y2] = Piece::BlackKing;
        }
    }
    // Для дамки
    else if (board[x][y] == Piece::WhiteKing || board[x][y] == Piece::BlackKing) {
        int stepX = dx;
        int stepY = dy;

        // Ищем шашку противника
        while (isInsideBoard(x + stepX, y + stepY)) {
            Piece current = board[x + stepX][y + stepY];

            if ((board[x][y] == Piece::WhiteKing && (current == Piece::Black || current == Piece::BlackKing)) ||
                (board[x][y] == Piece::BlackKing && (current == Piece::White || current == Piece::WhiteKing))) {

                // Нашли шашку противника
                board[x + stepX][y + stepY] = Piece::None; // Убираем шашку противника

                // Перемещаем дамку на конечную клетку
                int x2 = x + stepX + dx; // Двигаемся на две клетки по диагонали
                int y2 = y + stepY + dy;

                if (isInsideBoard(x2, y2)) {
                    board[x2][y2] = board[x][y]; // Перемещаем дамку на клетку за противником
                    board[x][y] = Piece::None; // Убираем дамку с исходной клетки
                }

                break;
            }

            stepX += dx;
            stepY += dy;
        }

        // Перемещаем дамку на конечную клетку
        int x2 = x + stepX;
        int y2 = y + stepY;

        if (isInsideBoard(x2, y2)) {
            board[x2][y2] = board[x][y];
            board[x][y] = Piece::None;
        }
    }
}

// Проверка, есть ли возможность взятия для текущего игрока
bool Board::hasCaptureMoves(Piece piece) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == piece || board[i][j] == (piece == Piece::White ? Piece::WhiteKing : Piece::BlackKing)) {
                // Проверка всех направлений для взятия
                if (canCapture(i, j, 1, 1, piece) || canCapture(i, j, 1, -1, piece) ||
                    canCapture(i, j, -1, 1, piece) || canCapture(i, j, -1, -1, piece)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Обработка хода игрока
bool Board::handleMove(int fromX, int fromY, int toX, int toY) {
    if (!isInsideBoard(fromX, fromY) || !isInsideBoard(toX, toY)) return false;

    Piece piece = board[fromX][fromY];
    if (piece == Piece::None || (currentPlayer == Piece::White && (piece != Piece::White && piece != Piece::WhiteKing)) ||
        (currentPlayer == Piece::Black && (piece != Piece::Black && piece != Piece::BlackKing))) {
        return false; // Неправильный выбор шашки
    }

    int dx = toX - fromX;
    int dy = toY - fromY;

    // Проверка, что ход происходит по диагонали
    if (abs(dx) != abs(dy)) {
        return false; // Ход не по диагонали
    }

    // Проверка на взятие
    if (abs(dx) == 2 && abs(dy) == 2) {
        if (canCapture(fromX, fromY, dx / 2, dy / 2, piece)) {
            performCapture(fromX, fromY, dx / 2, dy / 2);
            // Проверка, может ли шашка совершить еще одно взятие
            if (hasCaptureMoves(piece)) {
                return true; // Ход остается у текущего игрока
            }

            // Если больше нет взятий, меняем игрока
            currentPlayer = (currentPlayer == Piece::White) ? Piece::Black : Piece::White;
            if (currentPlayer == Piece::Black) blackMoves++;
            else whiteMoves++;

            // После успешного хода устанавливаем флаг
            moveMade = true;



            return true;
        }
    }

    // Проверка на обычный ход
    if (board[toX][toY] == Piece::None) {
        // Для обычных шашек
        if (piece == Piece::White || piece == Piece::Black) {
            // Белые шашки ходят только вверх (dx = -1)
            if ((piece == Piece::White && dx == -1 && abs(dy) == 1) ||
                (piece == Piece::Black && dx == 1 && abs(dy) == 1)) {
                board[toX][toY] = board[fromX][fromY];
                board[fromX][fromY] = Piece::None;

                // Превращение в дамку
                if (board[toX][toY] == Piece::White && toX == 0) {
                    board[toX][toY] = Piece::WhiteKing;
                }
                if (board[toX][toY] == Piece::Black && toX == BOARD_SIZE - 1) {
                    board[toX][toY] = Piece::BlackKing;
                }

                currentPlayer = (currentPlayer == Piece::White) ? Piece::Black : Piece::White; // Смена игрока
                if (currentPlayer == Piece::Black) blackMoves++;
                else whiteMoves++;

                // После успешного хода устанавливаем флаг
                moveMade = true;

                return true;
            }
        }
        // Для дамки
        else if (piece == Piece::WhiteKing || piece == Piece::BlackKing) {
            // Проверяем, что на пути нет препятствий
            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;
            int x = fromX + stepX;
            int y = fromY + stepY;

            // Проверяем все клетки по диагонали
            while (x != toX || y != toY) {
                if (board[x][y] != Piece::None) {
                    // Если нашли шашку противника, проверяем, есть ли свободная клетка за ней
                    if ((piece == Piece::WhiteKing && (board[x][y] == Piece::Black || board[x][y] == Piece::BlackKing)) ||
                        (piece == Piece::BlackKing && (board[x][y] == Piece::White || board[x][y] == Piece::WhiteKing))) {
                        // Проверяем, что за шашкой противника есть свободная клетка
                        int nextX = x + stepX;
                        int nextY = y + stepY;
                        if (isInsideBoard(nextX, nextY) && board[nextX][nextY] == Piece::None) {
                            //std::cout << "damka mozhet vzat' shashku in handlemove" << std::endl;
                            performCapture(fromX, fromY, stepX, stepY); // Выполняем взятие
                            return true; // Дамка может взять шашку
                        }
                    }
                    return false; // На пути есть препятствие
                }
                x += stepX;
                y += stepY;
            }

            // Перемещаем дамку
            board[toX][toY] = board[fromX][fromY];
            board[fromX][fromY] = Piece::None;

            currentPlayer = (currentPlayer == Piece::White) ? Piece::Black : Piece::White; // Смена игрока
            if (currentPlayer == Piece::Black) blackMoves++;
            else whiteMoves++;

            // После успешного хода устанавливаем флаг
            moveMade = true;

            return true;
        }
    }

    return false;
}


Piece Board::getCurrentPlayer() const {
    return currentPlayer;
}

int Board::getRemainingPieces(Piece piece) const {
    int count = 0;
    for (const auto& row : board) {
        for (const auto& p : row) {
            if (p == piece || p == (piece == Piece::White ? Piece::WhiteKing : Piece::BlackKing)) {
                count++;
            }
        }
    }
    return count;
}

bool Board::isCurrentPlayerPiece(int x, int y) const {
    Piece piece = board[x][y];
    return (currentPlayer == Piece::White && (piece == Piece::White || piece == Piece::WhiteKing)) ||
        (currentPlayer == Piece::Black && (piece == Piece::Black || piece == Piece::BlackKing));
}

// метод проверки есть ли победитель
bool Board::checkWin() const {
    if (getRemainingPieces(Piece::White) == 0) {
        return true;
    }
    if (getRemainingPieces(Piece::Black) == 0) {
        return true;
    }
    return false;
}

// Проверка, есть ли доступные ходы для текущего игрока
bool Board::hasAvailableMoves(Piece piece) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // Проверяем только обычные фигуры
            if (board[i][j] == piece) {
                // Для черных фигур (ход вниз)
                if (piece == Piece::Black) {
                    // Проверка обычных ходов вперёд (вниз)
                    if (isInsideBoard(i + 1, j - 1) && board[i + 1][j - 1] == Piece::None) return true;
                    if (isInsideBoard(i + 1, j + 1) && board[i + 1][j + 1] == Piece::None) return true;

                    // Проверка взятия для белых фигур (вперёд и назад, так как взятие может быть в любом направлении)
                    if (canCapture(i, j, 1, 1, piece) || canCapture(i, j, 1, -1, piece) ||
                        canCapture(i, j, -1, 1, piece) || canCapture(i, j, -1, -1, piece)) {
                        return true;
                    }
                }
                // Для белых фигур (ход вверх)
                else if (piece == Piece::White) {
                    // Проверка обычных ходов вперёд (вверх)
                    if (isInsideBoard(i - 1, j - 1) && board[i - 1][j - 1] == Piece::None) return true;
                    if (isInsideBoard(i - 1, j + 1) && board[i - 1][j + 1] == Piece::None) return true;

                    // Проверка взятия для чёрных фигур (вперёд и назад, так как взятие может быть в любом направлении)
                    if (canCapture(i, j, -1, 1, piece) || canCapture(i, j, -1, -1, piece) ||
                        canCapture(i, j, 1, 1, piece) || canCapture(i, j, 1, -1, piece)) {
                        return true;
                    }
                }
            }
        }
    }
    return false; // Если не найдено доступных ходов
}

// Оценочная функция для одной шашки
int Board::evaluatePiece(int x, int y) const
{
    Piece piece = board[x][y];
    if (piece == Piece::None)  // Пустая клетка
    {
        return 0;
    }
    int score = 0;                       // сумма очков шашки
    // Веса для различных критериев
    const int centralPositionWeight = 4; // вес для центральной позиции
    const int kingWeight = 12;           // вес для дамки
    const int checkerWeight = 6;         // вес для обычной шашки
    const int captureWeight = 10;        // вес для возможности захвата

    // 1. Позиция на доске (вес: 4)
    if ((x + y) % 2 == 1)
    {   // Шашка на темной клетке
        if (x >= 2 && x < BOARD_SIZE - 2 && y >= 2 && y < BOARD_SIZE - 2)
        {
            score += centralPositionWeight; // Центральная позиция
        }
    }

    // 2. Тип шашки (вес: 12 или 6)
    if (piece == Piece::WhiteKing || piece == Piece::BlackKing)
    {
        score += kingWeight;    // Дамка
    }

    if (piece == Piece::Black || piece == Piece::White)
    {
        score += checkerWeight; // обычная шашка
    }

    // 3. Проверка возможности взятия (вес: 10)
    bool canCaptureAny = false;

    // Проверяем все возможные направления для обычных шашек и дамок
    if (piece == Piece::White || piece == Piece::WhiteKing || piece == Piece::Black || piece == Piece::BlackKing)
    {
        canCaptureAny = canCapture(x, y, -1, -1, piece) || canCapture(x, y, -1, 1, piece) || canCapture(x, y, 1, 1, piece) || canCapture(x, y, 1, -1, piece);
    }

    // Если шашка может взять, добавляем балл
    if (canCaptureAny)
    {
        score += captureWeight;
    }

    return score;
}

// Оценочная функция для всей доски
int Board::evaluateBoard() const
{
    int totalScore = 0; // общее количество баллов всей доски

    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            Piece piece = board[i][j];
            if (piece == Piece::White || piece == Piece::WhiteKing)
            {
                int pieceScore = evaluatePiece(i, j); // Оценка для белой шашки
                totalScore -= pieceScore;             // Белые шашки уменьшают оценку
            }
            else if (piece == Piece::Black || piece == Piece::BlackKing)
            {
                int pieceScore = evaluatePiece(i, j); // Оценка для черной шашки
                totalScore += pieceScore;             // Черные шашки добавляют оценку
            }
        }
    }
    return totalScore;
}

// Функция для вывода данных о шашках
void Board::printPiecesInfo() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            Piece piece = board[i][j];
            if (piece == Piece::White || piece == Piece::WhiteKing) {
                int pieceScore = evaluatePiece(i, j);
                cout << "White piece at (" << i << ", " << j << ") has score: " << pieceScore << endl;
            }
            else if (piece == Piece::Black || piece == Piece::BlackKing) {
                int pieceScore = evaluatePiece(i, j);
                cout << "Black piece at (" << i << ", " << j << ") has score: " << pieceScore << endl;
            }
        }
    }
    cout << "Total board score: " << evaluateBoard() << endl;
}

float Board::calculateWinChances(int score)
{
    // Динамический диапазон для нормализации оценки
    const int minPossibleScore = -500; // Минимальная возможная оценка
    const int maxPossibleScore = 500;  // Максимальная возможная оценка

    // Нормализуем оценку в диапазоне [minPossibleScore, maxPossibleScore]
    float normalizedScore = static_cast<float>(score);
    normalizedScore = max(static_cast<float>(minPossibleScore), min(normalizedScore, static_cast<float>(maxPossibleScore)));

    // Преобразуем оценку в проценты с использованием сигмоиды для более плавного перехода
    float whiteChance = 1.0f / (1.0f + exp(-normalizedScore / 100.0f)); // Сигмоида
    whiteChance *= 100.0f; // Переводим в проценты

    // Ограничиваем шансы в диапазоне [0, 100]
    whiteChance = max(0.0f, min(whiteChance, 100.0f));

    return whiteChance; // Возвращаем шансы белых (шансы черных = 100 - whiteChance)
}

// Метод для генерации всех возможных ходов для текущего игрока
vector<Board> Board::generatePossibleMoves(Piece player) const {
    vector<Board> possibleMoves;

    // Сначала проверяем обязательные взятия
    bool captureAvailable = false;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((player == Piece::White && (board[i][j] == Piece::White || board[i][j] == Piece::WhiteKing)) ||
                (player == Piece::Black && (board[i][j] == Piece::Black || board[i][j] == Piece::BlackKing))) {

                // Проверяем все возможные направления для взятия
                for (int dx = -1; dx <= 1; dx += 2) {
                    for (int dy = -1; dy <= 1; dy += 2) {
                        if (canCapture(i, j, dx, dy, board[i][j])) {
                            captureAvailable = true;
                            Board newBoard = *this;
                            newBoard.performCapture(i, j, dx, dy);
                            possibleMoves.push_back(newBoard);
                        }
                    }
                }
            }
        }
    }

    // Если есть обязательные взятия, возвращаем только их
    if (captureAvailable) {
        return possibleMoves;
    }

    // Если нет обязательных взятий, генерируем обычные ходы
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((player == Piece::White && (board[i][j] == Piece::White || board[i][j] == Piece::WhiteKing)) ||
                (player == Piece::Black && (board[i][j] == Piece::Black || board[i][j] == Piece::BlackKing))) {

                // Для обычных шашек
                if (board[i][j] == Piece::White || board[i][j] == Piece::Black) {
                    int direction = (board[i][j] == Piece::White) ? -1 : 1;

                    // Проверяем два возможных направления движения
                    for (int dy = -1; dy <= 1; dy += 2) {
                        int newX = i + direction;
                        int newY = j + dy;

                        if (isInsideBoard(newX, newY) && board[newX][newY] == Piece::None) {
                            Board newBoard = *this;
                            newBoard.board[newX][newY] = newBoard.board[i][j];
                            newBoard.board[i][j] = Piece::None;

                            // Проверка на превращение в дамку
                            if ((newBoard.board[newX][newY] == Piece::White && newX == 0) ||
                                (newBoard.board[newX][newY] == Piece::Black && newX == BOARD_SIZE - 1)) {
                                newBoard.board[newX][newY] = (newBoard.board[newX][newY] == Piece::White) ?
                                    Piece::WhiteKing : Piece::BlackKing;
                            }

                            possibleMoves.push_back(newBoard);
                        }
                    }
                }
                // Для дамок
                else if (board[i][j] == Piece::WhiteKing || board[i][j] == Piece::BlackKing) {
                    // Проверяем все 4 направления
                    for (int dx = -1; dx <= 1; dx += 2) {
                        for (int dy = -1; dy <= 1; dy += 2) {
                            int step = 1;
                            while (true) {
                                int newX = i + dx * step;
                                int newY = j + dy * step;

                                if (!isInsideBoard(newX, newY) || board[newX][newY] != Piece::None) {
                                    break;
                                }

                                Board newBoard = *this;
                                newBoard.board[newX][newY] = newBoard.board[i][j];
                                newBoard.board[i][j] = Piece::None;
                                possibleMoves.push_back(newBoard);

                                step++;
                            }
                        }
                    }
                }
            }
        }
    }

    return possibleMoves;
}

// Минимакс алгоритм с альфа-бета отсечением
int Board::minimax(int depth, int alpha, int beta, bool maximizingPlayer) const {
    if (depth == 0 || checkWin()) {
        return evaluateBoard();
    }

    if (maximizingPlayer) {
        int maxEval = numeric_limits<int>::min();
        vector<Board> possibleMoves = generatePossibleMoves(Piece::Black);

        for (const Board& move : possibleMoves) {
            int eval = move.minimax(depth - 1, alpha, beta, false);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                break; // Альфа-бета отсечение
            }
        }
        return maxEval;
    }
    else {
        int minEval = numeric_limits<int>::max();
        vector<Board> possibleMoves = generatePossibleMoves(Piece::White);

        for (const Board& move : possibleMoves) {
            int eval = move.minimax(depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break; // Альфа-бета отсечение
            }
        }
        return minEval;
    }
}

// Нахождение лучшего хода с использованием минимакса
Board Board::findBestMove(int depth) {
    vector<Board> possibleMoves = generatePossibleMoves(Piece::Black); // Black - компьютер
    Board bestMove;
    int bestValue = numeric_limits<int>::min();

    for (const Board& move : possibleMoves) {
        int moveValue = move.minimax(depth - 1, numeric_limits<int>::min(),
            numeric_limits<int>::max(), true);

        if (moveValue > bestValue) {
            bestMove = move;
            bestValue = moveValue;
        }
    }

    return bestMove;
}

void Board::computerMove() 
{
    // Находим лучший ход с глубиной поиска 3
    Board bestMove = findBestMove(3);

    // Применяем лучший ход к текущей доске
    *this = bestMove;
    currentPlayer = Piece::White; // Передаём ход игроку

    // Обновляем состояние
    moveMade = true;
    blackMoves++;
}

Piece Board::getPiece(int x, int y) {
    if (isInsideBoard(x, y)) {
        return board[x][y];
    }
    return Piece::None; // Возвращаем None, если координаты вне доски
}
