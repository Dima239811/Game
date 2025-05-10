#include <SFML/Graphics.hpp>
#include "Board.h"

const int TILE_SIZE = 60;                           // ������ ������

class Graphics {
public:
    bool gameOver = false;                          // ���� ��������� ����
    Graphics(Board& board); 
    void run();                                     // �������� ����� ��� ������� �������
    void showMessage(string message);               // ����� ��� ����������� ��������� � ������
    void resetGameState();
    int selectedX = -1, selectedY = -1;             // ��������� �����
    int moveAttempts = 0;                           // ������� ������� ������ ����� � ��� �� �����
    bool showError = false;                         // ���� ��� ����������� ������
    sf::Clock errorClock;                           // ������ ��� ������� ������

private:
    void drawBoard(int selectedX, int selectedY);   // ��������� �����
    void updateInfoText();                          // ���������� ��������� ����������

    Board& board;                                   // ������ �� ������ �����
    sf::RenderWindow window;                        // ���� ��� ���������
    sf::Texture darkTexture;                        // �������� ��� ������ ������
    sf::Texture lightTexture;                       // �������� ��� ������� ������
    sf::Font font;                                  // ����� ��� ������
    sf::Text infoText;                              // ����� � ����������� � ������� ��������� ����
    sf::Text errorText;                             // ����� ��� ����������� ������
};
