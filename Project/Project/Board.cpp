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

// ����� ��������, ��� ���������� ��������� � �������� �����
bool Board::isInsideBoard(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

// �������� ����������� ������
bool Board::canCapture(int x, int y, int dx, int dy, Piece piece) const
{
    // ��� ������� �����
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

    // ��� �����
    if (piece == Piece::WhiteKing || piece == Piece::BlackKing)
    {
        int stepX = dx;
        int stepY = dy;
        int capturedCount = 0; // ������� ��������� �����, ������� ����� �����

        // ��������� ��� ������ �� ���������
        while (isInsideBoard(x + stepX, y + stepY))
        {
            Piece current = board[x + stepX][y + stepY];

            // ���� ������ ������, ���������� ��������
            if (current == Piece::None)
            {
                stepX += dx;
                stepY += dy;
                continue;
            }

            // ���� ����� ���� ����� � ������ ������
            if ((piece == Piece::WhiteKing && (current == Piece::White || current == Piece::WhiteKing)) ||
                (piece == Piece::BlackKing && (current == Piece::Black || current == Piece::BlackKing)))
            {
                break;
            }

            // ���� ����� ��������� �����
            if ((piece == Piece::WhiteKing && (current == Piece::Black || current == Piece::BlackKing)) ||
                (piece == Piece::BlackKing && (current == Piece::White || current == Piece::WhiteKing)))
            {
                // ���������, ���� �� ������ ������ �� ���� ������
                int nextX = x + stepX + dx;
                int nextY = y + stepY + dy;

                if (isInsideBoard(nextX, nextY) && board[nextX][nextY] == Piece::None)
                {
                    capturedCount++; // ����� ����� ��� �����
                    stepX += dx;
                    stepY += dy;
                }
                else
                {
                    break; // ��� ������ ������ �� ������ � ������ ������
                }
            }
        }

        // ���� ����� ����� ���� �� ���� �����, ���������� true
        return (capturedCount > 0);
    }

    return false;
}

// ���������� ������
void Board::performCapture(int x, int y, int dx, int dy) {
    // ��� ������� �����
    if (board[x][y] == Piece::White || board[x][y] == Piece::Black) {
        int x2 = x + dx * 2;
        int y2 = y + dy * 2;

        board[x2][y2] = board[x][y];
        board[x][y] = Piece::None;
        board[x + dx][y + dy] = Piece::None;

        // ����������� � �����, ���� ����� �������� ��������� �����������
        if (board[x2][y2] == Piece::White && x2 == 0) {
            board[x2][y2] = Piece::WhiteKing;
        }
        if (board[x2][y2] == Piece::Black && x2 == BOARD_SIZE - 1) {
            board[x2][y2] = Piece::BlackKing;
        }
    }
    // ��� �����
    else if (board[x][y] == Piece::WhiteKing || board[x][y] == Piece::BlackKing) {
        int stepX = dx;
        int stepY = dy;

        // ���� ����� ����������
        while (isInsideBoard(x + stepX, y + stepY)) {
            Piece current = board[x + stepX][y + stepY];

            if ((board[x][y] == Piece::WhiteKing && (current == Piece::Black || current == Piece::BlackKing)) ||
                (board[x][y] == Piece::BlackKing && (current == Piece::White || current == Piece::WhiteKing))) {

                // ����� ����� ����������
                board[x + stepX][y + stepY] = Piece::None; // ������� ����� ����������

                // ���������� ����� �� �������� ������
                int x2 = x + stepX + dx; // ��������� �� ��� ������ �� ���������
                int y2 = y + stepY + dy;

                if (isInsideBoard(x2, y2)) {
                    board[x2][y2] = board[x][y]; // ���������� ����� �� ������ �� �����������
                    board[x][y] = Piece::None; // ������� ����� � �������� ������
                }

                break;
            }

            stepX += dx;
            stepY += dy;
        }

        // ���������� ����� �� �������� ������
        int x2 = x + stepX;
        int y2 = y + stepY;

        if (isInsideBoard(x2, y2)) {
            board[x2][y2] = board[x][y];
            board[x][y] = Piece::None;
        }
    }
}

// ��������, ���� �� ����������� ������ ��� �������� ������
bool Board::hasCaptureMoves(Piece piece) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == piece || board[i][j] == (piece == Piece::White ? Piece::WhiteKing : Piece::BlackKing)) {
                // �������� ���� ����������� ��� ������
                if (canCapture(i, j, 1, 1, piece) || canCapture(i, j, 1, -1, piece) ||
                    canCapture(i, j, -1, 1, piece) || canCapture(i, j, -1, -1, piece)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// ��������� ���� ������
bool Board::handleMove(int fromX, int fromY, int toX, int toY) {
    if (!isInsideBoard(fromX, fromY) || !isInsideBoard(toX, toY)) return false;

    Piece piece = board[fromX][fromY];
    if (piece == Piece::None || (currentPlayer == Piece::White && (piece != Piece::White && piece != Piece::WhiteKing)) ||
        (currentPlayer == Piece::Black && (piece != Piece::Black && piece != Piece::BlackKing))) {
        return false; // ������������ ����� �����
    }

    int dx = toX - fromX;
    int dy = toY - fromY;

    // ��������, ��� ��� ���������� �� ���������
    if (abs(dx) != abs(dy)) {
        return false; // ��� �� �� ���������
    }

    // �������� �� ������
    if (abs(dx) == 2 && abs(dy) == 2) {
        if (canCapture(fromX, fromY, dx / 2, dy / 2, piece)) {
            performCapture(fromX, fromY, dx / 2, dy / 2);
            // ��������, ����� �� ����� ��������� ��� ���� ������
            if (hasCaptureMoves(piece)) {
                return true; // ��� �������� � �������� ������
            }

            // ���� ������ ��� ������, ������ ������
            currentPlayer = (currentPlayer == Piece::White) ? Piece::Black : Piece::White;
            if (currentPlayer == Piece::Black) blackMoves++;
            else whiteMoves++;

            // ����� ��������� ���� ������������� ����
            moveMade = true;



            return true;
        }
    }

    // �������� �� ������� ���
    if (board[toX][toY] == Piece::None) {
        // ��� ������� �����
        if (piece == Piece::White || piece == Piece::Black) {
            // ����� ����� ����� ������ ����� (dx = -1)
            if ((piece == Piece::White && dx == -1 && abs(dy) == 1) ||
                (piece == Piece::Black && dx == 1 && abs(dy) == 1)) {
                board[toX][toY] = board[fromX][fromY];
                board[fromX][fromY] = Piece::None;

                // ����������� � �����
                if (board[toX][toY] == Piece::White && toX == 0) {
                    board[toX][toY] = Piece::WhiteKing;
                }
                if (board[toX][toY] == Piece::Black && toX == BOARD_SIZE - 1) {
                    board[toX][toY] = Piece::BlackKing;
                }

                currentPlayer = (currentPlayer == Piece::White) ? Piece::Black : Piece::White; // ����� ������
                if (currentPlayer == Piece::Black) blackMoves++;
                else whiteMoves++;

                // ����� ��������� ���� ������������� ����
                moveMade = true;

                return true;
            }
        }
        // ��� �����
        else if (piece == Piece::WhiteKing || piece == Piece::BlackKing) {
            // ���������, ��� �� ���� ��� �����������
            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;
            int x = fromX + stepX;
            int y = fromY + stepY;

            // ��������� ��� ������ �� ���������
            while (x != toX || y != toY) {
                if (board[x][y] != Piece::None) {
                    // ���� ����� ����� ����������, ���������, ���� �� ��������� ������ �� ���
                    if ((piece == Piece::WhiteKing && (board[x][y] == Piece::Black || board[x][y] == Piece::BlackKing)) ||
                        (piece == Piece::BlackKing && (board[x][y] == Piece::White || board[x][y] == Piece::WhiteKing))) {
                        // ���������, ��� �� ������ ���������� ���� ��������� ������
                        int nextX = x + stepX;
                        int nextY = y + stepY;
                        if (isInsideBoard(nextX, nextY) && board[nextX][nextY] == Piece::None) {
                            //std::cout << "damka mozhet vzat' shashku in handlemove" << std::endl;
                            performCapture(fromX, fromY, stepX, stepY); // ��������� ������
                            return true; // ����� ����� ����� �����
                        }
                    }
                    return false; // �� ���� ���� �����������
                }
                x += stepX;
                y += stepY;
            }

            // ���������� �����
            board[toX][toY] = board[fromX][fromY];
            board[fromX][fromY] = Piece::None;

            currentPlayer = (currentPlayer == Piece::White) ? Piece::Black : Piece::White; // ����� ������
            if (currentPlayer == Piece::Black) blackMoves++;
            else whiteMoves++;

            // ����� ��������� ���� ������������� ����
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

// ����� �������� ���� �� ����������
bool Board::checkWin() const {
    if (getRemainingPieces(Piece::White) == 0) {
        return true;
    }
    if (getRemainingPieces(Piece::Black) == 0) {
        return true;
    }
    return false;
}

// ��������, ���� �� ��������� ���� ��� �������� ������
bool Board::hasAvailableMoves(Piece piece) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // ��������� ������ ������� ������
            if (board[i][j] == piece) {
                // ��� ������ ����� (��� ����)
                if (piece == Piece::Black) {
                    // �������� ������� ����� ����� (����)
                    if (isInsideBoard(i + 1, j - 1) && board[i + 1][j - 1] == Piece::None) return true;
                    if (isInsideBoard(i + 1, j + 1) && board[i + 1][j + 1] == Piece::None) return true;

                    // �������� ������ ��� ����� ����� (����� � �����, ��� ��� ������ ����� ���� � ����� �����������)
                    if (canCapture(i, j, 1, 1, piece) || canCapture(i, j, 1, -1, piece) ||
                        canCapture(i, j, -1, 1, piece) || canCapture(i, j, -1, -1, piece)) {
                        return true;
                    }
                }
                // ��� ����� ����� (��� �����)
                else if (piece == Piece::White) {
                    // �������� ������� ����� ����� (�����)
                    if (isInsideBoard(i - 1, j - 1) && board[i - 1][j - 1] == Piece::None) return true;
                    if (isInsideBoard(i - 1, j + 1) && board[i - 1][j + 1] == Piece::None) return true;

                    // �������� ������ ��� ������ ����� (����� � �����, ��� ��� ������ ����� ���� � ����� �����������)
                    if (canCapture(i, j, -1, 1, piece) || canCapture(i, j, -1, -1, piece) ||
                        canCapture(i, j, 1, 1, piece) || canCapture(i, j, 1, -1, piece)) {
                        return true;
                    }
                }
            }
        }
    }
    return false; // ���� �� ������� ��������� �����
}

// ��������� ������� ��� ����� �����
int Board::evaluatePiece(int x, int y) const
{
    Piece piece = board[x][y];
    if (piece == Piece::None)  // ������ ������
    {
        return 0;
    }
    int score = 0;                       // ����� ����� �����
    // ���� ��� ��������� ���������
    const int centralPositionWeight = 4; // ��� ��� ����������� �������
    const int kingWeight = 12;           // ��� ��� �����
    const int checkerWeight = 6;         // ��� ��� ������� �����
    const int captureWeight = 10;        // ��� ��� ����������� �������

    // 1. ������� �� ����� (���: 4)
    if ((x + y) % 2 == 1)
    {   // ����� �� ������ ������
        if (x >= 2 && x < BOARD_SIZE - 2 && y >= 2 && y < BOARD_SIZE - 2)
        {
            score += centralPositionWeight; // ����������� �������
        }
    }

    // 2. ��� ����� (���: 12 ��� 6)
    if (piece == Piece::WhiteKing || piece == Piece::BlackKing)
    {
        score += kingWeight;    // �����
    }

    if (piece == Piece::Black || piece == Piece::White)
    {
        score += checkerWeight; // ������� �����
    }

    // 3. �������� ����������� ������ (���: 10)
    bool canCaptureAny = false;

    // ��������� ��� ��������� ����������� ��� ������� ����� � �����
    if (piece == Piece::White || piece == Piece::WhiteKing || piece == Piece::Black || piece == Piece::BlackKing)
    {
        canCaptureAny = canCapture(x, y, -1, -1, piece) || canCapture(x, y, -1, 1, piece) || canCapture(x, y, 1, 1, piece) || canCapture(x, y, 1, -1, piece);
    }

    // ���� ����� ����� �����, ��������� ����
    if (canCaptureAny)
    {
        score += captureWeight;
    }

    return score;
}

// ��������� ������� ��� ���� �����
int Board::evaluateBoard() const
{
    int totalScore = 0; // ����� ���������� ������ ���� �����

    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            Piece piece = board[i][j];
            if (piece == Piece::White || piece == Piece::WhiteKing)
            {
                int pieceScore = evaluatePiece(i, j); // ������ ��� ����� �����
                totalScore -= pieceScore;             // ����� ����� ��������� ������
            }
            else if (piece == Piece::Black || piece == Piece::BlackKing)
            {
                int pieceScore = evaluatePiece(i, j); // ������ ��� ������ �����
                totalScore += pieceScore;             // ������ ����� ��������� ������
            }
        }
    }
    return totalScore;
}

// ������� ��� ������ ������ � ������
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
    // ������������ �������� ��� ������������ ������
    const int minPossibleScore = -500; // ����������� ��������� ������
    const int maxPossibleScore = 500;  // ������������ ��������� ������

    // ����������� ������ � ��������� [minPossibleScore, maxPossibleScore]
    float normalizedScore = static_cast<float>(score);
    normalizedScore = max(static_cast<float>(minPossibleScore), min(normalizedScore, static_cast<float>(maxPossibleScore)));

    // ����������� ������ � �������� � �������������� �������� ��� ����� �������� ��������
    float whiteChance = 1.0f / (1.0f + exp(-normalizedScore / 100.0f)); // ��������
    whiteChance *= 100.0f; // ��������� � ��������

    // ������������ ����� � ��������� [0, 100]
    whiteChance = max(0.0f, min(whiteChance, 100.0f));

    return whiteChance; // ���������� ����� ����� (����� ������ = 100 - whiteChance)
}

// ����� ��� ��������� ���� ��������� ����� ��� �������� ������
vector<Board> Board::generatePossibleMoves(Piece player) const {
    vector<Board> possibleMoves;

    // ������� ��������� ������������ ������
    bool captureAvailable = false;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((player == Piece::White && (board[i][j] == Piece::White || board[i][j] == Piece::WhiteKing)) ||
                (player == Piece::Black && (board[i][j] == Piece::Black || board[i][j] == Piece::BlackKing))) {

                // ��������� ��� ��������� ����������� ��� ������
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

    // ���� ���� ������������ ������, ���������� ������ ��
    if (captureAvailable) {
        return possibleMoves;
    }

    // ���� ��� ������������ ������, ���������� ������� ����
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((player == Piece::White && (board[i][j] == Piece::White || board[i][j] == Piece::WhiteKing)) ||
                (player == Piece::Black && (board[i][j] == Piece::Black || board[i][j] == Piece::BlackKing))) {

                // ��� ������� �����
                if (board[i][j] == Piece::White || board[i][j] == Piece::Black) {
                    int direction = (board[i][j] == Piece::White) ? -1 : 1;

                    // ��������� ��� ��������� ����������� ��������
                    for (int dy = -1; dy <= 1; dy += 2) {
                        int newX = i + direction;
                        int newY = j + dy;

                        if (isInsideBoard(newX, newY) && board[newX][newY] == Piece::None) {
                            Board newBoard = *this;
                            newBoard.board[newX][newY] = newBoard.board[i][j];
                            newBoard.board[i][j] = Piece::None;

                            // �������� �� ����������� � �����
                            if ((newBoard.board[newX][newY] == Piece::White && newX == 0) ||
                                (newBoard.board[newX][newY] == Piece::Black && newX == BOARD_SIZE - 1)) {
                                newBoard.board[newX][newY] = (newBoard.board[newX][newY] == Piece::White) ?
                                    Piece::WhiteKing : Piece::BlackKing;
                            }

                            possibleMoves.push_back(newBoard);
                        }
                    }
                }
                // ��� �����
                else if (board[i][j] == Piece::WhiteKing || board[i][j] == Piece::BlackKing) {
                    // ��������� ��� 4 �����������
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

// �������� �������� � �����-���� ����������
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
                break; // �����-���� ���������
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
                break; // �����-���� ���������
            }
        }
        return minEval;
    }
}

// ���������� ������� ���� � �������������� ���������
Board Board::findBestMove(int depth) {
    vector<Board> possibleMoves = generatePossibleMoves(Piece::Black); // Black - ���������
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
    // ������� ������ ��� � �������� ������ 3
    Board bestMove = findBestMove(3);

    // ��������� ������ ��� � ������� �����
    *this = bestMove;
    currentPlayer = Piece::White; // ������� ��� ������

    // ��������� ���������
    moveMade = true;
    blackMoves++;
}

Piece Board::getPiece(int x, int y) {
    if (isInsideBoard(x, y)) {
        return board[x][y];
    }
    return Piece::None; // ���������� None, ���� ���������� ��� �����
}
