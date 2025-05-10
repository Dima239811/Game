#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

const int BOARD_SIZE = 10; // Размер доски 10x10

enum class Piece {
    None,
    White,
    Black,
    WhiteKing,
    BlackKing
};

class Board {
public:
    Board();
    void initialize();
    bool isInsideBoard(int x, int y) const;
    bool canCapture(int x, int y, int dx, int dy, Piece piece) const;
    void performCapture(int x, int y, int dx, int dy);
    bool hasCaptureMoves(Piece piece);
    bool handleMove(int fromX, int fromY, int toX, int toY);
    Piece getCurrentPlayer() const;
    int getRemainingPieces(Piece piece) const;
    bool isCurrentPlayerPiece(int x, int y) const;
    bool checkWin() const;
    bool hasAvailableMoves(Piece piece);
    int evaluatePiece(int x, int y) const;
    int evaluateBoard() const;
    void printPiecesInfo();
    float calculateWinChances(int score);
    vector<Board> generatePossibleMoves(Piece) const;
    int minimax(int depth, int alpha, int beta, bool maximizingPlayer) const;
    Board findBestMove(int depth);
    void computerMove();
    Piece getPiece(int x, int y);


private:
    bool moveMade;
    vector<vector<Piece>> board;
    Piece currentPlayer;
    int whiteMoves;
    int blackMoves;
};
