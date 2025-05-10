#include "Graphics.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Graphics::Graphics(Board& board) : board(board) {
    // �������� ����
    window.create(sf::VideoMode(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE + 150), "International Checkers");

    // �������� �������
    if (!darkTexture.loadFromFile("image/dark_new.png")) {
        std::cerr << "Failed to load dark_tile.png" << std::endl;
    }
    if (!lightTexture.loadFromFile("image/light_new.png")) {
        std::cerr << "Failed to load light_tile.png" << std::endl;
    }

    // �������� ������
    if (!font.loadFromFile("font/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }

    // ��������� ������ � �����������
    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10, BOARD_SIZE * TILE_SIZE + 10);

    // ��������� ������ ��� ������
    errorText.setFont(font);
    errorText.setCharacterSize(24);
    errorText.setFillColor(sf::Color::Red);
    errorText.setPosition(BOARD_SIZE * TILE_SIZE - 300, BOARD_SIZE * TILE_SIZE + 80);
    ///errorText.setPosition(10, BOARD_SIZE * TILE_SIZE + 80);
}

void Graphics::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // ��������� ������� ����
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    // ����������� ���������� ���� � ������� ������
                    int x = mouseY / TILE_SIZE;
                    int y = mouseX / TILE_SIZE;

                    if (selectedX == -1 && selectedY == -1) {
                        if (board.getCurrentPlayer() == Piece::White && board.isCurrentPlayerPiece(x, y)) {
                            selectedX = x;
                            selectedY = y;
                            moveAttempts = 0;
                        }
                        else {
                            showError = true;
                            errorText.setString("You cannot select the opponent's piece!");
                            errorClock.restart();
                        }
                    }
                    else {
                        if (selectedX == x && selectedY == y) {
                            moveAttempts++;
                            if (moveAttempts >= 2) {
                                showError = true;
                                errorText.setString("You cannot select the same piece twice!");
                                errorClock.restart();
                                moveAttempts = 0;
                            }
                        }
                        else {
                            // ��� �����
                            if (board.hasCaptureMoves(Piece::White)) {
                                if (abs(selectedX - x) == 2 && abs(selectedY - y) == 2) {
                                    if (!board.handleMove(selectedX, selectedY, x, y)) {
                                        showError = true;
                                        errorText.setString("Invalid capturing move!");
                                        errorClock.restart();
                                    }
                                }
                                else {
                                    showError = true;
                                    errorText.setString("You must make a capturing move!");
                                    errorClock.restart();
                                }
                            }
                            else {
                                if (!board.handleMove(selectedX, selectedY, x, y)) {
                                    showError = true;
                                    errorText.setString("Invalid move!");
                                    errorClock.restart();
                                }
                            }
                            selectedX = -1;
                            selectedY = -1;

                            // ��� ���������� (���� ������� ����� ������)
                            if (board.getCurrentPlayer() == Piece::Black) {
                                board.computerMove();
                            }
                        }
                    }
                }
            }
        }

        // �������� �� ������ ����� ������� ����
        if (board.checkWin()) {
            std::string message;
            if (board.getRemainingPieces(Piece::White) == 0) {
                message = "Black wins! \nClick 'Restart' to play again.";
            }
            else if (board.getRemainingPieces(Piece::Black) == 0) {
                message = "White wins! \nClick 'Restart' to play again.";
            }
            showMessage(message);
            continue;
        }

        if (!board.hasAvailableMoves(board.getCurrentPlayer())) {
            std::string message = (board.getCurrentPlayer() == Piece::White) ?
                "White has no moves left. \nBlack wins! \nClick 'Restart' to play again." :
                "Black has no moves left.\nWhite wins! \nClick 'Restart' to play again.";
            showMessage(message);
            continue;
        }

        // ���������� ��������� ����������
        updateInfoText();

        // ������� ��������� �� ������
        if (showError && errorClock.getElapsedTime().asSeconds() >= 2) {
            showError = false;
        }

        // ���������
        window.clear();
        drawBoard(selectedX, selectedY);
        window.draw(infoText);
        if (showError) window.draw(errorText);
        window.display();
    }
}

void Graphics::drawBoard(int selectedX, int selectedY) {
    sf::Sprite tileSprite;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // ����� �������� ��� ������
            if ((i + j) % 2 == 0) {
                tileSprite.setTexture(lightTexture);
            }
            else {
                tileSprite.setTexture(darkTexture);
            }

            // ��������� ������� �������
            tileSprite.setPosition(j * TILE_SIZE, i * TILE_SIZE);
            window.draw(tileSprite);

            // �������� ����� � �����
            Piece piece = board.getPiece(i, j);

            // ��������� �����
            if (piece != Piece::None) {
                sf::CircleShape pieceShape(TILE_SIZE / 2 - 5);
                pieceShape.setPosition(j * TILE_SIZE + 5, i * TILE_SIZE + 5);
                switch (piece) {
                case Piece::White:
                    pieceShape.setFillColor(sf::Color::White);
                    break;
                case Piece::Black:
                    pieceShape.setFillColor(sf::Color::Black);
                    break;
                case Piece::WhiteKing:
                    pieceShape.setFillColor(sf::Color::White);
                    break;
                case Piece::BlackKing:
                    pieceShape.setFillColor(sf::Color::Black);
                    break;
                }
                window.draw(pieceShape);

                // ��������� ����� "D" ��� �����
                if (piece == Piece::WhiteKing || piece == Piece::BlackKing) {
                    sf::Text text;
                    text.setFont(font);
                    text.setString("D");
                    text.setCharacterSize(24);
                    text.setFillColor(piece == Piece::WhiteKing ? sf::Color::Black : sf::Color::White);
                    text.setPosition(j * TILE_SIZE + 20, i * TILE_SIZE + 15);
                    window.draw(text);
                }
            }
        }
    }

    // ��������� ��������� �����
    if (selectedX != -1 && selectedY != -1) {
        sf::RectangleShape highlight(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineThickness(3);
        highlight.setOutlineColor(sf::Color::Red);
        highlight.setPosition(selectedY * TILE_SIZE, selectedX * TILE_SIZE);
        window.draw(highlight);
    }
}

void Graphics::updateInfoText() {
    std::ostringstream oss;
    oss << "Current Player: " << (board.getCurrentPlayer() == Piece::White ? "White" : "Black") << "\n"
        << "White Pieces Left: " << board.getRemainingPieces(Piece::White) << "\n"
        << "Black Pieces Left: " << board.getRemainingPieces(Piece::Black);

    // ������ ������ �� ������
    int score = board.evaluateBoard();
    float whiteChance = board.calculateWinChances(score);
    float blackChance = 100.0f - whiteChance;

    oss << "\n" << "White Win Chance: " << std::fixed << std::setprecision(1) << whiteChance << "%\n"
        << "Black Win Chance: " << std::fixed << std::setprecision(1) << blackChance << "%\n";

    infoText.setString(oss.str());
}

void Graphics::showMessage(std::string message) {
    sf::RenderWindow messageWindow(sf::VideoMode(400, 400), "Game Over");
    sf::Font font;
    if (!font.loadFromFile("font/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }

    // ����� ���������
    sf::Text text;
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setPosition(50, 100);

    // ������ "������ ������"
    sf::RectangleShape restartButton(sf::Vector2f(200, 50));
    restartButton.setFillColor(sf::Color::Green);
    restartButton.setPosition(100, 200);

    sf::Text restartButtonText;
    restartButtonText.setFont(font);
    restartButtonText.setString("Restart");
    restartButtonText.setCharacterSize(24);
    restartButtonText.setFillColor(sf::Color::White);
    restartButtonText.setPosition(150, 210);

    // ������ "��������� ����"
    sf::RectangleShape quitButton(sf::Vector2f(200, 50));
    quitButton.setFillColor(sf::Color::Red);
    quitButton.setPosition(100, 300);

    sf::Text quitButtonText;
    quitButtonText.setFont(font);
    quitButtonText.setString("Quit");
    quitButtonText.setCharacterSize(24);
    quitButtonText.setFillColor(sf::Color::White);
    quitButtonText.setPosition(170, 310);

    while (messageWindow.isOpen()) {
        sf::Event event;
        while (messageWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
            {
                messageWindow.close();
                resetGameState();
                return;
            }
               

            // ��������� ������� �� ������ "Restart"
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    // ��������, ������ �� ������ "Restart"
                    if (mouseX >= 100 && mouseX <= 300 && mouseY >= 200 && mouseY <= 250) {
                        messageWindow.close(); // ��������� ���� ���������
                        resetGameState();
                        return;
                    }

                    // ��������, ������ �� ������ "Quit"
                    if (mouseX >= 100 && mouseX <= 300 && mouseY >= 300 && mouseY <= 350) {
                        messageWindow.close(); // ��������� ���� ���������
                        std::exit(0); // ��������� ���������
                    }
                }
            }
        }

        messageWindow.clear(sf::Color::White);
        messageWindow.draw(text);
        messageWindow.draw(restartButton);
        messageWindow.draw(restartButtonText);
        messageWindow.draw(quitButton);
        messageWindow.draw(quitButtonText);
        messageWindow.display();
    }
}


void Graphics::resetGameState() {
    board.initialize(); // ���������� �����
    gameOver = false; // ���������� ���� ��������� ����
    selectedX = -1; // ���������� ��������� �����
    selectedY = -1;
    moveAttempts = 0; // ���������� ������� �������
    showError = false; // ���������� ���� ������
}